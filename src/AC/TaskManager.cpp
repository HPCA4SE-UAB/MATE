
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
// TaskManager.cpp
// 
// TaskManager - manager of mpi tasks
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#include "TaskManager.h"
#include "Syslog.h"
#include <cassert>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

static char * SeverityName [] = 
{
	"fatal", "serious", "warning", "info"
};

TaskManager * TaskManager::_taskMngr = 0;

//typedef void (*BPatchExecCallback)(BPatch_thread *thr)
void OnExecCallback (BPatch_thread * thread)
{
	  Syslog::Warn ("[DynInst Exec Callback]");
}

// Not used
void OnPreForkCallback (BPatch_thread * parent, BPatch_thread * child)
{
	  Syslog::Warn ("[DynInst PRE Fork Callback %d -> %d]", parent!=NULL?parent->getTid ():-1, 
	  	child!=NULL?child->getTid ():-1);
}

// Not used
void OnPostForkCallback (BPatch_thread * parent, BPatch_thread * child)
{
	  Syslog::Warn ("[DynInst POST Fork Callback %d -> %d]", parent!=NULL?parent->getTid ():-1, 
	  	child!=NULL?child->getTid ():-1);
	  Syslog::Warn ("[DynInst POST Fork Callback: detaching child process %d", child->getTid ());
	  child->getProcess ()->detach (1);
	  Syslog::Warn ("[DynInst POST Fork Callback: child process detached");
}

TaskManager::TaskManager (Config & cfg) 
	: _handler (0), _started (false), 
	  _tuner (_tasks), _monitor (_tasks),
	  _analyzerPort (0),
	  _taskDebugLevel (0),
	  _taskDebugStdErr (0),
	  _disableExitCallback (false)
{
   	assert (_taskMngr == 0);
	// init static pointer   
   	_taskMngr = this;
   	BPatch & bp = DynInst::Instance ();
	bp.registerErrorCallback (DynInstErrorCallback);
   	bp.registerExitCallback (ThreadExitCallback);
  	bp.registerExecCallback (OnExecCallback);
	// Not used, as of new Dyninst versions the mutator will not be
	// attached to any childs of the main process by default.
   	//bp.registerPreForkCallback (OnPreForkCallback);
   	//bp.registerPostForkCallback (OnPostForkCallback);
   	// load preinstrumentation (no longer implemented, nor used)
   	//_monitor.LoadPreinstrumentation (cfg);
   	_DMLibName = cfg.GetStringValue ("AC", "DMLib");
}

TaskManager::~TaskManager ()
{
	assert (_taskMngr == this);
	CleanupTasks ();
	_taskMngr = 0;
}

void TaskManager::CleanupTasks ()
{
 	_disableExitCallback = true;		
	// determine a number of tasks possibly running (and therefore to kill)
	const int nTasksLeft = _tasks.GetCount ();
	Syslog::Info ("Cleaning up tasks (%d tasks left)", nTasksLeft);
	for (int i=0; i<nTasksLeft; ++i)
	{
		Task * t = _tasks [i];
		if (t != 0)
		{
			t->Terminate ();
			_tasks.Delete (i);
		}
	}
	_tasks.Clear ();
	Syslog::Info ("Clean up done");
}

void TaskManager::SetAnalyzerHost (string const & analyzerHost, int collectorPort)
{
	Syslog::Info ("[TaskManager] Analyzer host is %s, collector port %d", 
		analyzerHost.c_str (), collectorPort);
	_analyzerHost = analyzerHost;
	_analyzerPort = collectorPort;
}

bool TaskManager::StartMPITask (const std::string& path, char ** args)
{
	Syslog::Info ("[TaskManager] start mpi task %s", path.c_str());
	bool b = StartTask (path, args, 0, 0);
	_started = b;
	if (b)
		Syslog::Debug ("[TaskManager] MPI task started");	
	else
		Syslog::Error ("[TaskManager] MPI task start failed");	
	return b;
}

bool TaskManager::StartTask (const std::string& path, char ** args, int envc, char ** env)
{
	Syslog::Debug ("[TaskManager] start task %s", path.c_str());
	int i = 0;
	while (args [i] != 0)
	{
		Syslog::Debug ("[TaskManager] Arg %d=%s", i, args [i]);
		++i;
	}
	Syslog::Debug ("[TaskManager] starting task...");
	// required by PVM task
	cout << _analyzerHost << endl;
	cout << _analyzerPort << endl;
	Task * t = new Task (path, args, _clockDiff, _analyzerHost, _analyzerPort, 
		_taskDebugLevel, _taskDebugStdErr, _DMLibName);	
	assert (t != 0);
	//_monitor.PreInstrument (*t);
	if (t->IsStopped ())
    {
		Syslog::Debug ("[TaskManager] Task is created but stopped. Continuing");
        t->Continue (); 
	}
    auto_ptr<Task> pTask (t);
	_tasks.Add (pTask);

    _started = true;
	return _started;
}

void TaskManager::OnExitCallback (BPatch_thread * proc, int exitCode)
{  
	Syslog::Debug ("[TaskManager] OnExitCallback called");
	if (_disableExitCallback) return;	
  	assert (proc != 0);
  	BPatch_process * p = proc->getProcess ();
   	int pid = p->getPid ();
   	int taskIdx = _tasks.FindByPid (pid);
   	if (taskIdx != TaskCollection::NotFound)
   	{
   		Task const * task = _tasks [taskIdx];
   		Syslog::Info ("[TaskManager] ExitCallback from task pid %d", pid);
   		if (_handler != 0)
    		_handler->HandleTaskExit (*task, exitCode);
   		_tasks.Delete (taskIdx);
   	}
   	else 
   	   	Syslog::Error ("[TaskManager] taskIdx -> TaskCollection::NotFound");
   	// note: it is possible that the task is created
   	// and fails before it is added to the _tasks list.
}

void TaskManager::OnErrorCallback (BPatchErrorLevel severity, int number, const char* const* params)
{
    /*if (number == 0) 
    {
        // conditional reporting of warnings and informational messages
        
        if (severity == BPatchInfo)
        { 
        	//printf("[Info]: %s\n", params[0]); 
        } 
        else
        {
            printf("[Other]:%s", params[0]);
        }
		Syslog::Info ("[DynInst says] Message: (%s).", params[0]);        
    } 
    else 
    {    			
		char const * str = SeverityName [(int)severity];
		char msg [1024]; // check the max size
		BPatch & bp = DynInst::Instance ();
		char const * msgFormat = bp.getEnglishErrorString (number);
		if (msgFormat != 0)
		 	bp.formatErrorString (msg, sizeof(msg), msgFormat, params);
		else
			sprintf (msg, "Unknown");
		Syslog::Error ("[DynInst %s] Num: %d. Message: (%s).", str, number, msg);
	}*/
	char const * str = SeverityName [(int)severity];
	if (severity == BPatchInfo)
    { 
    	//printf("[DynInst %s]: %s\n", str, params[0]); 
    	//Syslog::Info ("[DynInst says] Message: (%s).", params[0]);  
    } 
    else if (severity == BPatchWarning)
    {
        printf("[DynInst %s]: %s", str, params[0]);
        Syslog::Info ("[DynInst says] Message: (%s).", params[0]);  
    } 
    else  //fatal o serious
    {    			
    	printf("[DynInst %s]: %s", str, params[0]);
		Syslog::Error ("[DynInst %s] Num: %d. Message: (%s).", str, number, params[0]);
	}	      
}

void TaskManager::ThreadExitCallback (BPatch_thread * proc, BPatch_exitType exitCode)
{ 
	Syslog::Debug ("[TaskManager] ThreadExitCallback called");
	switch (exitCode)
	{
	case NoExit:
		Syslog::Error ("[TaskManager] Exit code: no exit");
		break;
	case ExitedNormally:
		Syslog::Debug ("[TaskManager] Exit code: exited normally");
		break;
	case ExitedViaSignal:
		int signal = proc->getProcess()->stopSignal ();
		Syslog::Error ("[TaskManager] Exit code: exit via signal, %d", signal);
		break;
	}
	assert (_taskMngr != 0);
	_taskMngr->OnExitCallback (proc, (int)exitCode);
}

void TaskManager::DynInstErrorCallback (BPatchErrorLevel severity, int number, const char* const* params)
{
	assert (_taskMngr != 0);
	_taskMngr->OnErrorCallback (severity, number, params);	
}

void TaskManager::HandleEvents ()
{
	BPatch & bp = DynInst::Instance ();
	bool status = bp.pollForStatusChange ();
	if (status)
	{
		// maybe we have a breakpoint hit
		CheckBreakpointHit ();
	}
}

void TaskManager::CheckBreakpointHit ()
{
	Syslog::Info ("[TaskMgr] CheckBreakpointHit ");	
	
	for (int i=0; i<_tasks.GetCount (); ++i)
	{
		if (_tasks [i]->IsStoppedOnBreakpoint ())
		{
			_tasks [i]->ProcessBreakpoint (_tuner);
		}
	}
}

