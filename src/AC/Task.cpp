
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
// Task.cpp
// 
// Task - represents a MPI task
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#include "Task.h"
#include "Tuner.h"
#include "SysException.h"
//#include "FuncScanner.h"
#include "SnippetMaker.h"
#include "Syslog.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>	
#include <sys/param.h>		// for const variable MAXPATHLEN used in realpath
#include <unistd.h>
#include <libgen.h>



#include "papiStdEventDefs.h"
#include "papi.h"

Task::Task (const std::string& path,
			char * args[], 
			TimeValue const & clockDiff, 
			string const & analyzerHost, 
			int analyzerPort, 
			int debugLevel, 
			int debugStdErr,
			string const & DMLibName)
	: _path (strdup (path.c_str())),
	  _process (_path, args),
      _image (_process)
{
	Syslog::Debug ("Task %s created with pid: %d, time diff: %lld", _path, GetPid (), clockDiff.GetMilliseconds ());
	char DMLib [512];
	strcpy (DMLib, DMLibName.c_str());
 	Syslog::Debug ("Loading dynamic library %s", DMLib);
  	_process.loadLibrary (DMLib);
	Syslog::Debug ("Dynamic library loaded");
	InitLibrary (analyzerHost, analyzerPort, clockDiff, debugLevel, debugStdErr);
	Syslog::Debug ("Dynamic library initialized"); 	
}

Task::~Task ()
{

	if (_pendingTuningReqs.size () > 0)
	{
		Syslog::Debug ("Deleting %d deffered tunings that were not executed.", _pendingTuningReqs.size ());
		// delete not executed requests
		TuningRequests::const_iterator it = _pendingTuningReqs.begin ();
		while (it != _pendingTuningReqs.end ())
		{
			TuningRequest * req = (*it);
			delete req;
		}
	}
	Syslog::Debug ("IsTerminated?");
	if (!IsTerminated ())
	{
		Syslog::Debug ("UnloadLibrary");
		UnloadLibrary ();
	}
	free (_path);
	Syslog::Debug ("Task %d has finished.", GetPid ());
}

void Task::InitLibrary (string const & analyzerHost, 
						int analyzerPort, 
					    TimeValue const & clockDiff,
					    int debugLevel,
					    int debugStdErr)
{
	typedef BPatch_Vector<BPatch_snippet*> SnippetVector;
	SnippetVector statements;
	std::string configFile("DMLib.ini");
	// call create
	//void DMLib_Init (int pid, char const * taskName, char const * analyzerHost, int analyzerPort);
	DiFunction creatFunc(_image, "DMLib_Init");	
	//BPatch_constExpr taskIdExpr (GetPid ());
	//BPatch_constExpr taskNameExpr (_path);
	BPatch_snippet *taskNameExpr = new BPatch_constExpr(_path);
	BPatch_snippet *analyzerHostExpr = new BPatch_constExpr(analyzerHost.c_str ());
	BPatch_constExpr analyzerPortExpr (analyzerPort);
	BPatch_snippet *configFileExpr = new BPatch_constExpr(configFile.c_str());
	
	// put these parameters into vector
	BPatch_Vector<BPatch_snippet*> args1;
	//args1.push_back (&taskIdExpr);	
	args1.push_back (taskNameExpr);	
	args1.push_back (analyzerHostExpr);	
	args1.push_back (&analyzerPortExpr);
	args1.push_back (configFileExpr);
	BPatch_funcCallExpr creatCall (creatFunc, args1);	

	//void DMLib_SetDiff (int lowDiff, int highDiff);
	// prepare difference time
	DiFunction diffFunc(_image, "DMLib_SetDiff");	
	long_t diff = clockDiff.GetTotalMicroseconds ();
	int_t lowDiff, highDiff;
	LONG2INTS (diff, highDiff, lowDiff);
	BPatch_constExpr lowDiffExpr (lowDiff);	
	BPatch_constExpr highDiffExpr (highDiff);	
	// put these parameters into vector
	BPatch_Vector<BPatch_snippet*> args2;
	args2.push_back (&lowDiffExpr);	
	args2.push_back (&highDiffExpr);	

	// create function call snippet and excecute it once
	BPatch_funcCallExpr diffCall (diffFunc, args2);	
	statements.push_back (&creatCall);
	statements.push_back (&diffCall);
	
	BPatch_sequence seq (statements);
	if (!IsStopped ())
	{
		Syslog::Debug ("Stopping execution");
		_process.StopExecution ();
	}
	// insert into MPI_INIT exit
	Syslog::Debug ("Task::InitLibrary -> inserting into MPI_Init exit");
	DiPoint point (_image, "MPI_Init", BPatch_exit);
	_process.InsertSnippetAfter (seq, point);

	DiFunction doneFunc(_image, "DMLib_Done");	
	SnippetVector statements1;
	
	// put these parameters into vector
	BPatch_Vector<BPatch_snippet*> nullArgs;

	// create function call snippet and excecute it once
	BPatch_funcCallExpr doneCall (doneFunc, nullArgs);	
	statements1.push_back (&doneCall);
	Syslog::Debug ("Task::InitLibrary -> inserting into MPI_Finalize exit");
	DiPoint point1 (_image, "MPI_Finalize", BPatch_exit);
	BPatch_sequence seq1 (statements1);
	_process.InsertSnippetAfter (seq1, point1);

	
	//_process.StopExecution (); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//_process.ContinueExecution ();
}

//void DMLib_DoneLogger ();
void Task::UnloadLibrary ()
{
	assert (!IsTerminated ());
	Syslog::Debug ("Unloading library from task %d", GetPid ());
	//-----------------------------------------------------------
	DiFunction doneFunc (_image, "DMLib_Done");
	BPatch_Vector<BPatch_snippet *> nullArgs;
	
	BPatch_funcCallExpr doneCall (doneFunc, nullArgs);
	// insert the snippet into exit of main
	_process.OneTimeCode (doneCall);
	Syslog::Debug ("Unload library done for task %d", GetPid ());
}

void Task::AddDelayedTuning (TuningRequest * req)
{
	_pendingTuningReqs.push_back (req);
}

void Task::ProcessBreakpoint (Tuner & t)
{
	Syslog::Debug ("[Task %d] processing deffered tuning on breakpoint", GetPid ());
	TuningRequest * req = _pendingTuningReqs.front ();
	_pendingTuningReqs.pop_front ();
	
	assert (req != 0);	
	// remove the breakpoint
	t.RemoveLastBreakpoint (*this);
	req->ClearBreakpoint ();
	// insert the deferred tuning
	t.Process (req);
	// continue execution of the task
	_process.ContinueExecution ();
}
