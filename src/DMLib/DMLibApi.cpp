
/**************************************************************************
*                    Universitat Autonoma de Barcelona,					  *
*              HPC4SE: http://grupsderecerca.uab.cat/hpca4se/             *
*                        Analysis and Tuning Group, 					  *
*					            2002 - 2018                  			  */
/**************************************************************************
*	  See the LICENSE.md file in the base directory for more details      *
*									-- 									  *
*	This file is part of MATE.											  *	
*																		  *
*	MATE is free software: you can redistribute it and/or modify		  *
*	it under the terms of the GNU General Public License as published by  *
*	the Free Software Foundation, either version 3 of the License, or     *
*	(at your option) any later version.									  *
*																		  *
*	MATE is distributed in the hope that it will be useful,				  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 		  *
*	GNU General Public License for more details.						  *
*																		  *
*	You should have received a copy of the GNU General Public License     *
*	along with MATE.  If not, see <http://www.gnu.org/licenses/>.         *
*																		  *
***************************************************************************/

//------------------------------------------------------------------
// DMLibApi.cpp
// Dynamic Monitoring Library loaded into mutatee process
// it connects to the analyzer and sends events
//
// (c) Anna Sikora, UAB, 2010
//------------------------------------------------------------------

#include "DMLibApi.h"
#include "Error.h"
#include "Syslog.h"
#include "Utils.h"
#include "ECPProxy.h"
#include "EventMsgWriter.h"
#include "Types.h"
#include "Config.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <sys/param.h>
#include <signal.h>
#include "papiStdEventDefs.h"
#include "papi.h"
#include <string.h> 
#include <semaphore.h>
#include <fcntl.h>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <iterator>
#include <vector>

using namespace DMLib;

// global library variables
static int						_initialized = 1; // init flag
//static double 					_diff;		// difference in millisecs to host machine
static long_t					_diff;	
static int  					_pid;		// process id
static int  					_mpiRank;		// mpi process rank
static long_t					_startTime;	// starting time
static char 					_localHost [MAXHOSTNAMELEN];
static char 					_analyzerHost [MAXHOSTNAMELEN];  // analyzer host
static int 						_port;
static EventCollectorProxy * 	_proxy = 0;
static EventMsgWriter			_eventWriter;
static char						_taskName [MAXPATHLEN+1];
string 		 					_configFile;
Config 							_cfg;

static std::map<std::string, std::pair<int,int> > map_EventSets;

static long long int			_eventValues[20];
//static int EventSet[10]={PAPI_NULL};


void DMLib_LogInfo ()
{
	Syslog::Info ("Pid %d, mpiRank: %d, Local host %s, Analyzer host:port (%s:%d)", 
		_pid, _mpiRank, _localHost, _analyzerHost, _port);
	Syslog::Info ("Clock diff to master host is: %lld ms", _diff);
	Syslog::Info ("Library connected to the Analyzer.");
}

long_t DMLib_Now ()
{
    struct timeval tv ;
    //time in seconds + microseconds that have elapsed since 1/1/1970 UTC 0:0:0.0.
    gettimeofday ( & tv, NULL );
    //time in milliseconds that have elapsed since 1/1/1970 UTC.
	return ((long_t)tv.tv_sec) * 1000 + (long_t)(tv.tv_usec/1000);
}

void SetHosts (string const & analyzerHost, int analyzerPort)
{
	// set local host
	int status = gethostname (_localHost, sizeof (_localHost));
	if (status == -1)
		strcpy (_localHost, "unknown");
 	// set analyzer host
 	if (analyzerHost.size() == 0)
 		strcpy (_analyzerHost, "unknown");
 	else
		strcpy (_analyzerHost, analyzerHost.c_str());
	_port = analyzerPort;
}

void DMLib_InitStartTime (int lowDiff, int highDiff)
{
	long long diff_micros;
	unsigned long long h64 = (unsigned long long)highDiff;
	h64 <<= 32;
	diff_micros = (long long)(h64 | (unsigned long long)lowDiff);
	// we store time difference as seconds (with decimals)
	//_diff = (double)diff_micros / 1000000.0;
	
	_diff = diff_micros / 1000;
	
	_startTime = DMLib_Now () - _diff;
	Syslog::Debug ("[API] DMLib low %d, high %d, diff is: %lld ms", lowDiff, highDiff, _diff);
}
	
bool DMLib_InitProxy (string const & analyzerHost, int analyzerPort)
{
	SetHosts (analyzerHost, analyzerPort);

	try{ // connect to Analyzer
		Syslog::Debug ("[API] Creating proxy for analyzerHost: %s, port: %d", _analyzerHost, _port);

		_proxy = new EventCollectorProxy (_analyzerHost, _port);

		int ACport = 100;
		Syslog::Debug ("[API] Registering DMLib library for the process id %d, mpiRank: %d, application: %s, on machine %s, ACport: %d",
			_pid, _mpiRank, _taskName, _localHost, ACport);
		_proxy->RegisterLib (_pid, _mpiRank, _localHost, _taskName, ACport);
		Syslog::Debug ("[API] DMLib_InitProxy DONE!!!");
		return true;
	} catch (SysException e) {
		Syslog::Error ("[API] %s, error code: %d\n", e.GetErrorMessage().c_str(), e.GetError());
		return false;
	}
}

bool DMLib_SysLogConfigure (string const & configFile)
{	
	try{

		_configFile.assign(configFile);
		char curDirBuf [PATH_MAX];
		printf ("[API] Reading config file: %s, curDir: %s \n", _configFile.c_str(),
				getcwd (curDirBuf, sizeof (curDirBuf)));

		fflush (stdout);
		_cfg = ConfigHelper::ReadFromFile (_configFile);
		Syslog::Configure (_cfg);
		Syslog::Debug ("[API] Syslog configured");
		return true;
	} catch (ConfigException e) {
		printf ("[API] %s \n", e.GetErrorMessage().c_str());
		//std::cerr << "[API]" << e.GetErrorMessage().c_str() <<"\n";
		return false;
	}
}

bool GetMPIRankVariable ()
{
  char * mpiRankStr = getenv ("OMPI_COMM_WORLD_RANK");
  if (mpiRankStr == NULL){
    Syslog::Error ("Cannot get OMPI_COMM_WORLD_RANK environment variable");
    return false;
  }
  _mpiRank = atoi (mpiRankStr);
  return true;
}

void signalWait()
{
	std::stringstream ss;
	ss << getpid();
	std::cout << ss.str() << std::endl; 
	sem_t *sem = sem_open(ss.str().c_str(), O_CREAT, S_IRWXU, 0);
	if(sem == SEM_FAILED)
	{
		printf("SEM_FAILED");
		return;
	}
	Syslog::Debug ( "WAITING SIGNAL pid:%d...\n", getpid());
	if(sem_wait(sem)!=0)
		printf("error\n");
	Syslog::Debug ( "SIGNAL RECEIVED pid:%d...\n", getpid());
	sem_unlink(ss.str().c_str());
	/*printf("[JORDI] Check EventSetMap:\n");
	for(std::map<std::string, std::pair<int, int> > ::const_iterator it = map_EventSets.begin(); 
		it != map_EventSets.end(); ++it)
    {
        std::cout << "[" << it->first << "] " << it->second.first << " " << it->second.second << std::endl;
    }*/
}

// --------------------------- API --------------------------- 
bool DMLib::DMLib_Init (const char *taskName, const char *analyzerHost, int analyzerPort,
		const char *configFile)
{
	// initialize Syslog
	if (!DMLib_SysLogConfigure (configFile))
		return false;

	signal (SIGPIPE, SIG_IGN);
	Syslog::Info ("[DMLib API] DMLib log initialized");

	// initialize library implementation
	_pid = getpid ();
	if (!GetMPIRankVariable ())
		return false;
	Syslog::Info ("[DMLib API] DMLib process: Pid=%d, mpiRank=%d", _pid, _mpiRank);
	
	
	int retval;
	retval = PAPI_library_init( PAPI_VER_CURRENT );
	if (retval != PAPI_VER_CURRENT) {
		Syslog::Debug ( "PAPI error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!: ");
		if(retval == PAPI_EINVAL)
			Syslog::Debug ( "PAPI_EINVAL\n");
		if(retval == PAPI_ENOMEM)
			Syslog::Debug ( "PAPI_ENOMEM\n");
		if(retval == PAPI_ESBSTR)
			Syslog::Debug ( "PAPI_ESBSTR\n");
		if(retval == PAPI_ESYS)
			Syslog::Debug ( "PAPI_ESYS\n");
		return false;
	}
	
	strcpy (_taskName, taskName);
	if(!DMLib_InitProxy (analyzerHost, analyzerPort))
		return false;
	_initialized = 1;
	signalWait();
	return true;
}

//EventSet al inicio debe ser PAPI_NULL, después tener el valor asignado al finalizar la función
void DMLib::DMLib_ECreate(const char* func_name)
{
	Syslog::Debug ( "[API] DMLib_PAPICreate: %s",  func_name );
	
	
	
	std::map<std::string, std::pair<int, int> >::iterator find_eventset = map_EventSets.find(func_name);
	if(find_eventset == map_EventSets.end())
	{
		int retval;
		int eventSetId = PAPI_NULL;
		// Create an EventSet 
		if ((retval=PAPI_create_eventset(&eventSetId)) != PAPI_OK)
			Syslog::Debug ( "[API] DMLib_PAPICreate, PAPI error: %d", retval );
		else
		{
			Syslog::Debug ( "[API] DMLib_PAPICreate EventSetId: %d created", eventSetId );
			//Create in map
			map_EventSets[func_name] = std::make_pair(eventSetId, 0);
			if ( ( retval = PAPI_assign_eventset_component( eventSetId, 0 ) ) != PAPI_OK )
				Syslog::Debug ( "[API] DMLib_PAPICreate, PAPI error: %d", retval );

			PAPI_option_t opt;
			memset(&opt,0x0,sizeof(PAPI_option_t));
			opt.inherit.eventset = eventSetId;
			opt.inherit.inherit = PAPI_INHERIT_ALL;
			if (PAPI_set_opt(PAPI_INHERIT, &opt) != PAPI_OK)
				Syslog::Debug ( "[API] DMLib_PAPICreate, PAPI error: %d", retval );
		}
	}
	else
	{
		Syslog::Debug ( "ERROR: FUNCTION %s was created before!!!!!!!!!\n", func_name);
	}
	
	//printf("[JORDI]DMLib_ECreate END %d %d!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", n_EventSet, EventSet[n_EventSet]);
}

void DMLib::DMLib_PAPIAdd(char* EventName, const char* func_name)
{
	int retval;
	PAPI_event_info_t info;
	Syslog::Debug ("[API] DMLib_PAPIAdd EventName: %s to %s \n", EventName, func_name );


	//std::cout << "[JORDI] DMLib_PAPIAdd EventName: " << EventName << " in " << func_name << std::endl;
	std::map<std::string, std::pair<int, int> >::iterator find_eventset = map_EventSets.find(func_name);
	if(find_eventset == map_EventSets.end())
	{
		Syslog::Debug ( "ERROR: FUNCTION %s NOT FOUND IN MAP!!!!!!!!!\n", func_name);
	}
	int eventSetId= find_eventset->second.first;
	// find_eventset->second.second;
	if(eventSetId != PAPI_NULL)
	{
		int event_code;
		if ( PAPI_event_name_to_code( EventName, &event_code ) == PAPI_OK ) 
		{
			if ( PAPI_get_event_info( event_code, &info ) != PAPI_OK )
			{
				Syslog::Debug ("[API] DMLib_PAPIAdd failed, event not available");
				return;
			}
      else
      {
     	  if ((retval=PAPI_add_event(eventSetId, event_code)) != PAPI_OK)
    		{
    			Syslog::Debug ( "[API] DMLib_PAPIAdd failed, PAPI error: %d", retval );
    		}
    		else
    		{
    			Syslog::Debug ( "[API] DMLib_PAPIAdd success!");
    			find_eventset->second.second ++;
    		}
      }
		} 
		else 
		{
			Syslog::Debug ( "[API] DMLib_PAPIAdd failed, event %s does not exist", EventName );
		}   
	}
	else
	{
		Syslog::Debug ( "ERROR: EventSet not initialized!\n");
	}
}

void DMLib::DMLib_PAPI_Start(const char *func_name)
{
	Syslog::Debug ( "DMLib_PAPI_Start %s\n", func_name);
	int retval;
	std::map<std::string, std::pair<int, int> >::iterator find_eventset = map_EventSets.find(func_name);
	if(find_eventset == map_EventSets.end())
		Syslog::Debug ( "ERROR: FUNCTION NOT FOUND IN MAP!!!!!!!!!\n");
	int eventSetId = find_eventset->second.first;
	/* Start counting */
	if ((retval=PAPI_start(eventSetId)) != PAPI_OK)
		Syslog::Debug ( "[API] DMLib_PAPI_Start, PAPI error: %d", retval );
}

void DMLib::DMLib_PAPI_Stop(const char *func_name)
{
  Syslog::Debug ( "DMLib_PAPI_Stop %s\n", func_name);
	std::map<std::string, std::pair<int, int> >::iterator find_eventset = map_EventSets.find(func_name);
	if(find_eventset == map_EventSets.end())
		Syslog::Debug ( "ERROR: FUNCTION NOT FOUND IN MAP!!!!!!!!!\n");
	int eventSetId = find_eventset->second.first;
	int nMetrics = find_eventset->second.second;
	int retval;
	/* Stop events and read*/
	if ((retval=PAPI_stop(eventSetId, _eventValues)) != PAPI_OK)
	{
		Syslog::Debug ( "[API] DMLib_PAPI_Add_Event failed, PAPI error: %d", retval );
		return;
	}
	int i;
	//std::cout << "[JORDI]Metrics: ";
	for (i=0; i<nMetrics; i++)
	{
		//std::cout << _eventValues[i] << " "; 
		double eventValue;
		memcpy(&eventValue, &_eventValues[i], sizeof(eventValue));
		//std::cout << "("<<  eventValue << ") "; 
		DMLib_AddDoubleParam(eventValue);
		//int eventValue = (int)_eventValues[i];
		//DMLib_AddIntParam(eventValue);
	}
	//std::cout << std::endl; 

}

void DMLib::DMLib_PAPI_Destroy_EventSet(const char *func_name)
{
  Syslog::Debug ( "DMLib_PAPI_Destroy_EventSet %s\n", func_name);
	int retval;
	std::map<std::string, std::pair<int, int> >::iterator find_eventset = map_EventSets.find(func_name);
	if(find_eventset == map_EventSets.end())
		Syslog::Debug ( "ERROR: FUNCTION NOT FOUND IN MAP!!!!!!!!!\n");
	int eventSetId = find_eventset->second.first;
    if ((retval=PAPI_cleanup_eventset(eventSetId)) != PAPI_OK)
  	 	Syslog::Debug ( "[API] DMLib_PAPI_Destroy_EventSet failed, PAPI error: %d", retval );
  
  	if ((retval=PAPI_destroy_eventset(&eventSetId)) != PAPI_OK)
  		Syslog::Debug ( "[API] DMLib_PAPI_Destroy_EventSet failed, PAPI error: %d", retval );
}

void DMLib::DMLib_SetDiff (int lowDiff, int highDiff)
{
	DMLib_InitStartTime (lowDiff, highDiff);
	_initialized = 1;
	Syslog::Info ("[DMLib API] DMLib done for the app %s", _taskName);
}

void DMLib::DMLib_Done ()
{
	Syslog::Info ("[DMLib API] DMLib_Done initialized?");
	if (!_initialized) return;
	_proxy->UnregisterLibrary (_pid);
	delete (_proxy);
	_proxy = 0;
	Syslog::Info ("[DMLib API] DMLib_Done initialized? UnregisterLibrary");
}

void DMLib::DMLib_OpenEvent (int eventId, InstrPlace instrPlace, int paramCount)//---
{
	Syslog::Debug ("[API] DMLib_OpenEvent");
	if (!_initialized) {
		Syslog::Debug ("[API] DMLib_OpenEvent failed, DMLib not initialized");
		return;
	}
	long_t timestamp = DMLib_Now () - _startTime;
	Syslog::Debug ("[API] DMLib_OpenEvent -> eventId: %d, instrPlace: %d, paramCount: %d", 
			eventId, instrPlace, paramCount);
	// create new event object
	_eventWriter.OpenEvent (timestamp, eventId, (InstrPlace)instrPlace, paramCount);
}

void DMLib::DMLib_AddIntParam (int value)
{
	if (!_initialized) return;
	Syslog::Debug ("[API] IntParam %d", value);
	_eventWriter.AddIntParam (value);
}

void DMLib::DMLib_AddFloatParam (float value)
{
	if (!_initialized) return;
	Syslog::Debug ("[API] FloatParam %f", value);
	_eventWriter.AddFloatParam (value);
}

void DMLib::DMLib_AddDoubleParam (double value)
{
	if (!_initialized) return;
	Syslog::Debug ("[API] DoubleParam %lf", value);
	_eventWriter.AddDoubleParam (value);
}

void DMLib::DMLib_AddCharParam (char value)
{
	if (!_initialized) return;
	Syslog::Debug ("[API] CharParam %c", value);
	_eventWriter.AddCharParam (value);
}

void DMLib::DMLib_AddStringParam (string const & value)
{
	if (!_initialized) return;
	Syslog::Debug ("[API] StringParam ");
	_eventWriter.AddStringParam (value);
}

void DMLib::DMLib_CloseEvent ()
{
	if (!_initialized) return;
	EventMsg const & event = _eventWriter.CloseEvent ();
	Syslog::Debug ("[API] DMLib_CloseEvent 0");
	_proxy->SendEvent (event);
	Syslog::Debug ("[API] DMLib_CloseEvent 1");
}

