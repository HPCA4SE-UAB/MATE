
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
// AppModel.cpp
// 
// Abstract Application Model Classes
//
// Anna Sikora, UAB, 2004
//
//----------------------------------------------------------------------

#include "AppModel.h"
#include "Syslog.h"
#include "ACProxy.h"
#include <assert.h>

#include <netdb.h>
#include <sys/param.h>

using namespace Model;


Application::Application (char const * appPath, 
						  int argc, 
						  char const ** argv)
: _queue (MaxQueueSize),
  _name (appPath),
  _argc (argc),
  _argv (argv),
  _masterTask (0),
  _status (stNotStarted) 	
{}
		
void Application::Start ()
{
	Syslog::Debug ("[App:Model] Application::Start"); 
	if (_status != stNotStarted) {
		Syslog::Warn ("App has already been started");
		return;
	}
	/*Syslog::Debug ("[App:Model] ACProxy proxy on masterHost %s", _masterHost.c_str()); 
	ACProxy proxy (_masterHost);
	// prepare analyser host to be sent together with the message
	char analyzerHost [MAXHOSTNAMELEN];  // analyzer host
	int status = gethostname (analyzerHost, sizeof (analyzerHost));
	if (status == -1)
		strcpy (analyzerHost, "unknown");
	Syslog::Debug ("[App:Model] STARTING APP........................."); 
	proxy.StartApplication (_name.c_str (), _argc, _argv, analyzerHost);*/
	_status = stStarting;
	Syslog::Debug ("[App:Model] APP STARTED"); 
}

int Application::ProcessEvents(bool block) {
	int nProcessed = 0;
	ECPMessage * msg = 0;

	if (block) { // blocking
		_queue.GetB (msg);
		ProcessEvent (msg);
		++nProcessed;
	}
	// non-blocking
	while (_queue.Get(msg)) {
		ProcessEvent(msg);
		++nProcessed;
	}
	return nProcessed;
}

void Application::ProcessEvent(ECPMessage * msg) {
	assert(msg != 0);
	ECPMsgType type = msg->GetType();
	int size = msg->GetDataSize();
	if (type == ECPEvent) {
		Syslog::Debug ("[App:Model] EVENT MESSAGE");
		EventMsg * em = (EventMsg*) msg;
		DispatchEvent (*em);
	} else if (type == ECPReg) {
		if (_status == stStarting)
			_status = stRunning;		
		RegisterMsg * rm = (RegisterMsg*) msg;
		Syslog::Debug ("[App:Model] REGISTERING task PID=%d, mpi rank: %d",
						rm->GetPid (), rm->GetMpiRank ());
		Host & host = AddHost (rm->GetHost ());
		AddTask (rm->GetPid (), rm->GetMpiRank (), rm->GetTaskName (), host);
	} else if (type == ECPUnReg) {
		Syslog::Debug ("[App:Model] ECPUnReg");
		UnRegisterMsg * unreg = (UnRegisterMsg*) msg;
		Syslog::Debug ("[App:Model] UNREGISTERING task PID=%d",
						unreg->GetPid ());
		RemoveTask (unreg->GetPid ());
	}
	//else ignore
	delete msg;
}

void Application::DispatchEvent(EventMsg const & msg) {
	// find task by id
	Syslog::Debug ("[App:Model] Event from task %d, T:%lld", msg.GetTid (),
					msg.GetTimestamp ());
	Task & t = _tasks.GetById (msg.GetTid ());
	t.DispatchEvent (msg);
}

int Application::AddEvent(Event const & e) {
	int count = 0;
	for (int i = 0; i < _tasks.Size(); ++i) {
		Task * t = _tasks [i];
		if (t != 0) {
			t->AddEvent (e);
			++count;
		}
	}
	return count;
	// NOTE: this instrumentation should be also inserted in ALL tasks
	// that WILL start later ?
}
				
int Application::RemoveEvent(int eventId, InstrPlace place) {
	int count = 0;
	for (int i = 0; i < _tasks.Size (); ++i) {
		Task * t = _tasks[i];
		if (t != 0) {
			bool b = t->RemoveEvent(eventId, place);
			if (b)
				++count;
		}
	}
	return count;
}

int Application::LoadLibrary(std::string const & libPath) {
	int count = 0;
	for (int i = 0; i < _tasks.Size(); ++i) {
		Task * t = _tasks[i];
		if (t != 0) {
			t->LoadLibrary (libPath);
			++count;
		}
	}
	return count;	
}

int Application::SetVariableValue (std::string const & varName,
				AttributeValue const & varValue, Breakpoint * brkpt)
{
	int count = 0;
	for (int i = 0; i < _tasks.Size(); ++i) {
		Task * t = _tasks [i];
		if (t != 0) {
			t->SetVariableValue(varName, varValue, brkpt);
			++count;
		}
	}
	return count;		
}

int Application::ReplaceFunction(std::string const & oldFunc,
							std::string const & newFunc, Breakpoint * brkpt) {
	int count = 0;
	for (int i = 0; i < _tasks.Size (); ++i) {
		Task * t = _tasks [i];
		if (t != 0) {
			t->ReplaceFunction(oldFunc, newFunc, brkpt);
			++count;
		}
	}
	return count;	
}
					
int Application::InsertFunctionCall(std::string const & funcName, int nAttrs,
						Attribute * attrs, std::string const & destFunc,
						InstrPlace destPlace, Breakpoint * brkpt) {
	int count = 0;
	for (int i = 0; i < _tasks.Size(); ++i)	{
		Task * t = _tasks[i];
		if (t != 0) {
			t->InsertFunctionCall(funcName, nAttrs, attrs, destFunc, destPlace,
								  brkpt);
			++count;
		}
	}
	return count;		
}	

int Application::OneTimeFuncCall (std::string const & funcName, int nAttrs,
									Attribute * attrs, Breakpoint * brkpt) {
	int count = 0;
	for (int i = 0; i < _tasks.Size(); ++i) {
		Task * t = _tasks[i];
		if (t != 0) {
			t->OneTimeFuncCall(funcName, nAttrs, attrs, brkpt);
			++count;
		}
	}
	return count;		
}

int Application::RemoveFuncCall (std::string const & funcName, 
					std::string const & callerFunc, Breakpoint * brkpt) {
	int count = 0;
	for (int i = 0; i < _tasks.Size(); ++i) {
		Task * t = _tasks[i];
		if (t != 0) {
			t->RemoveFuncCall (funcName, callerFunc, brkpt);
			++count;
		}
	}
	return count;		
}
					
int Application::FuncParamChange (std::string const & funcName, int paramIdx,
									int newValue, int * requiredOldValue,
										Breakpoint * brkpt) {
	int count = 0;
	for (int i = 0; i < _tasks.Size(); ++i) {
		Task * t = _tasks [i];
		if (t != 0) {
			t->FuncParamChange(funcName, paramIdx, newValue, requiredOldValue,
								brkpt);
			++count;
		}
	}
	return count;		
}

void Application::SetTaskHandler (TaskHandler & h) {
	_taskHandlers.push_back (&h);
}

void Application::SetHostHandler(HostHandler & h) {
	_hostHandlers.push_back (&h);
}

Host & Application::AddHost(std::string const & name) {
	// find if exists
	for (int i = 0; i < _hosts.size(); ++i) {
		if (_hosts [i]->GetName() == name)
			return *_hosts [i];
	}

	// add new host
	Host * h = new Host(name);
	auto_ptr<Host> pHost (h);
	_hosts.push_back(pHost);

	// notify handlers
	HostHandlers::iterator it = _hostHandlers.begin ();
	while (it != _hostHandlers.end()) {
		(*it)->HostAdded (*h);
		it++;
	}
	return *h;
}

void Application::AddTask(int pid, int mpiRank, string const & name, Host & h) {
	Task * t = new Task (pid, mpiRank, name, h);
	auto_ptr<Task> pTask (t);
	_tasks.Add (pTask);
	//if (_tasks.Size () == 1)
	if (mpiRank == 0) {
		_masterTask = t;
		t->SetMaster (true);
	}
	// notify handlers
	TaskHandlers::iterator it = _taskHandlers.begin();
	while (it != _taskHandlers.end()) {
		/*TaskHandler * handler = (*it);
		if (handler != 0)
			handler->TaskStarted (*t);*/
		(*it)->TaskStarted (*t);
		it++;
	}
}

void Application::RemoveTask(int tid) {
	auto_ptr<Task> pTask = _tasks.Remove(tid);
	if (pTask.get() == 0) {
		Syslog::Warn ("Removing non-existing task with TID=%d", tid);
	} else {
		//Syslog::Debug ("Task removed. %d tasks left", _tasks.Size ());
			
		// notify handlers
		TaskHandlers::iterator it = _taskHandlers.begin ();
		while (it != _taskHandlers.end()) {
			(*it)->TaskTerminated(*pTask.get());
			it++;
		}

		// is master task terminated ?
		if (pTask.get() == _masterTask) {
			_masterTask = 0;
		}

		// any tasks left ?
		if (_tasks.Size() == 0) {
			// app has terminated
			_status = stFinished;
			Syslog::Info ("Application has finished");
		}		
	}
}

void Application::OnEvent(ECPMessage * msg) {
	_queue.Put(msg);
}

void Application::OnFatalError () {
	_status = stAborted;
}

