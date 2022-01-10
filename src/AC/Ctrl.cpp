
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

//----------------------------------------------------------------------
//
// ctrl.cpp
//
// AC (Tracer/Tuner Controller) implementation
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#include <sys/param.h>
#include <stdio.h>
#include <sys/timeb.h>
#include "Ctrl.h"
#include "Reactor.h"
#include "Utils.h"
#include "Syslog.h"
#include "ShutDownSlave.h"
#include "SysException.h"
#include "PTPAcceptor.h"

static char * DefConfigFile = "AC.ini";


#include "papiStdEventDefs.h"
#include "papi.h"




Controller * Controller::TheInstance = 0;

void Controller::SignalHandler (int signo){
   if (signo == SIGINT || signo == SIGABRT || signo == SIGTERM) // Check for any terminal signal
   {
    	if (Controller::TheInstance)
    		Controller::TheInstance->Interrupt ();
   }
}

Controller::Controller (CommandLine & cmdLine) 
	: _cmdLine (cmdLine), _fInterrupted (0)
{
	assert (TheInstance == 0);
	TheInstance = this;
	
	Configure ();
	SetupSignals ();
}

Controller::~Controller ()
{
	TheInstance = 0;
	Syslog::Debug ("Controller exists");
}

void Controller::SetupSignals ()
{
	// Bind the signals with their handler
	signal (SIGINT, &SignalHandler);
	signal (SIGABRT, &SignalHandler);
	signal (SIGTERM, &SignalHandler);
}

void Controller::Interrupt ()
{
	// called by signal handler and the shut down system
	Syslog::Debug ("Stopping AC");
	_fInterrupted = 1;
	Syslog::Debug ("fInterrupted = %d", _fInterrupted);
}

// Test if file exists
void Controller::CheckFile (char const * fileName)
{
	FILE * file = fopen (fileName, "r");
	if (file == NULL)
	{
		char buf [255];
		sprintf (buf, "File %s does not exists", fileName);
		throw SysException (buf);
	}
	else
		fclose (file);
	Syslog::Debug ("CheckFile: file %s exists", fileName);
}

void Controller::Configure ()
{
	Syslog::Debug ("b4 checking has config.");
    if (_cmdLine.HasConfig ())
    	_configFile = _cmdLine.GetConfigFileName ();
    else
        _configFile = DefConfigFile;
    char curDirBuf [PATH_MAX];
	printf ("[CTRL] Reading config file: %s, curDir: %s \n", _configFile, 
			getcwd (curDirBuf, sizeof (curDirBuf)));
	_cfg = ConfigHelper::ReadFromFile(_configFile);
	Syslog::Configure (_cfg);
	Syslog::Debug ("[CTRL] Syslog configured");
	
	char localHost [MAXHOSTNAMELEN];
	int status = gethostname (localHost, sizeof (localHost));

	Syslog::Info ("AC ready on host %s", localHost);
}

void Controller::Run () 
{
	int level = 0;
	TaskManager taskMngr (_cfg);
	
	int dmLibDebugLevel = _cfg.GetIntValue ("TraceLib", "DebugLevel", -1);
	int dmLibStdErr = _cfg.GetIntValue ("TraceLib", "StdErr", 0);
	taskMngr.SetTaskDebug (dmLibDebugLevel, dmLibStdErr);
	
	string analyzerHost = _cfg.GetStringValue ("Analyzer", "Host");
	int analyzerPort = _cfg.GetIntValue ("Analyzer", "Port");
	taskMngr.SetAnalyzerHost (analyzerHost, analyzerPort);
	
	int acPort = _cfg.GetIntValue("AC","PTPAcceptorPort");
	Syslog::Debug("PTPAcceptor base port AC is %d", acPort);

  
	char * mpiRankStr = getenv ("OMPI_COMM_WORLD_RANK");
  //char * mpiRankStr = getenv ("PMI_RANK");
	if (mpiRankStr == NULL)
	{
		Syslog::Error("Cannot get OMPI_COMM_WORLD_RANK environment variable");

	}
	else
	{
		Syslog::Debug("Process mpiRank: %d\n",atoi (mpiRankStr));
	}

	//acPort=acPort+getpid();
	acPort = acPort+atoi (mpiRankStr);
  
	Syslog::Debug("PTPAcceptor port AC is %d", acPort);
	Reactor reactor;
	
	PTPAcceptor acceptor (reactor, taskMngr, acPort);
	
	cout << "CTRL" << pthread_self() << endl;
	//ShutDownSlave SDS (analyzerHost, analyzerPort, *this);

	InitTracer (taskMngr); // run master monitor
		
	Syslog::Info ("Running main event loop...");
	int i=0;

	while (!_fInterrupted)
	{
		Syslog::Debug ("fInterrupted = %d", _fInterrupted);
		Syslog::Debug ("taskMngr.IsDone () = %d", taskMngr.IsDone ());

		TimeValue waitTime (1); // check but do not wait for events
		Syslog::Debug ("Reactor -> handle events");
		reactor.HandleEvents (&waitTime);
		// handle taskMngr events (from DI)
		taskMngr.HandleEvents ();
		if (i % 30 == 0)
			Syslog::Info ("Reactor loop");
		i++;
		// check if the master AC finishes the work
		if (_fInterrupted || taskMngr.IsDone ())
		{
			if(_fInterrupted)
				Syslog::Info ("AC has been interrupted.");
			else
				Syslog::Info ("AC is done.");
			break;
		}
	}
	Syslog::Info ("AC has finished.");
}

void Controller::InitTracer (TaskManager & taskMngr)
{
	TimeValue clockDiff;
	taskMngr.SetClockDiff (clockDiff);
	Syslog::Debug ("Clock diff to time server: %lld [microsec]", clockDiff.GetTotalMicroseconds ());	
	
	// start the application
	if (!taskMngr.StartMPITask ((char * const)_cmdLine.GetAppPath (), _cmdLine.GetAppArgv ()))
	{
		// revisit
		throw "MPI task start failed";
	}
}
