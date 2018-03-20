
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
// AppTask.cpp
// 
// Abstract Application Model Classes
//
// Anna Sikora, UAB, 2004
//
//----------------------------------------------------------------------

#include "AppTask.h"
#include "AppEvent.h"
#include "AppModel.h"
#include "Syslog.h"
#include "ACProxy.h"
#include <assert.h>

using namespace Model;


//Tasks========================================================================
void Tasks::Add(auto_ptr<Task> & task) {
	_arr.push_back(task);
	++_count;
}

void Tasks::Delete(int index) {
	assert(_count > 0);
   _arr.remove_direct(index, 0);
   --_count;
}

Task const * Tasks::operator[](int index) const {
	return _arr [index];
}
	
Task * Tasks::operator[](int index) {
	return _arr [index];
}

int Tasks::FindById (int id) {
	auto_vector<Task>::iterator iter = _arr.begin();
	int idx = 0;
	while (iter != _arr.end()) {
    	Task * t = (*iter);
    	if (t != 0) {
      		if (t->GetPid() == id)
				return idx;
    	}  
    	++iter;
    	++idx;
 	}
	return NotFound;
}  

int Tasks::Size() const {
	return _count;
}

Task & Tasks::GetById (int id) {
	int taskIdx = FindById (id);
	if (taskIdx == NotFound) {
		// task doesn't exist or its already finished
		char buf[16];
		sprintf(buf, "Id=%d", id);
		throw Exception("Task not found", buf);
	}
	Task * t = _arr[taskIdx];
	assert(t != 0);
	return *t;
}	

auto_ptr<Task> Tasks::Remove(int id) {
	int taskIdx = FindById (id);
	if (taskIdx == NotFound) {
		// task doesn't exist 
		return auto_ptr<Task>(0);
	}
	--_count;
	return _arr.acquire(taskIdx);
}

//Task=========================================================================
Task::Task (int pid, int mpiRank, string const & name, Host & h)
 : _pid(pid), _mpiRank(mpiRank), _name(name), _host(h), _status(stRunning),
   _isMaster(false)
{}

void Task::DispatchEvent(EventMsg const & msg) {
	Event * e = _events.Find(msg.GetEventId(),(InstrPlace)msg.GetPlace());
	if (e != 0) {
		EventHandler * eh = e->GetEventHandler();
		if (eh != 0) {
			EventRecord r(*e, *this, msg);
			eh->HandleEvent(r);
		}
	}
	// else ignore event		
}

ACProxy * Task::GetACProxy() {
	return new ACProxy(_host.GetName (), 9900+GetMpiRank()); // default port
}

void Task::AddEvent (Event const & e) {
	
	Syslog::Debug ("[ACProxy] host: %s, mpirank %d,", _host.GetName().c_str(), GetMpiRank());
	ACProxy * proxy = GetACProxy();

	if(e.GetNumPapiMetrics() == 0)
	{
		proxy->AddInstr(_pid, e.GetId (), e.GetFunctionName(), e.GetInstrPlace(),
						 e.GetNumAttributes(), e.GetAttributes());
	}
	else
	{
		proxy->AddInstr(_pid, e.GetId (), e.GetFunctionName(), e.GetInstrPlace(),
						 e.GetNumAttributes(), e.GetAttributes(), e.GetNumPapiMetrics(), e.GetMetrics());
	}
	_events.Add (e);
	
}

			
bool Task::RemoveEvent(int eventId, InstrPlace place) {
	ACProxy * proxy = GetACProxy();
	proxy->RemoveInstr(_pid, eventId, place);
	
	return _events.Remove(eventId, place);
}

void Task::LoadLibrary(std::string const & libPath) {
	ACProxy * proxy = GetACProxy();
	proxy->LoadLibrary(_pid, libPath);
}
							
void Task::SetVariableValue(std::string const & varName,
				AttributeValue const & varValue, Breakpoint * brkpt) {
	ACProxy * proxy = GetACProxy();
	proxy->SetVariableValue(_pid, varName, varValue, brkpt);
}
				
void Task::ReplaceFunction(std::string const & oldFunc,
				std::string const & newFunc, Breakpoint * brkpt) {
	ACProxy * proxy = GetACProxy();
	proxy->ReplaceFunction(_pid, oldFunc, newFunc, brkpt);
}

void Task::InsertFunctionCall(std::string const & funcName, int nAttrs,
								Attribute * attrs, std::string const & destFunc,
								InstrPlace destPlace, Breakpoint * brkpt) {
	ACProxy * proxy = GetACProxy();
	proxy->InsertFunctionCall(_pid, funcName, nAttrs, attrs, destFunc,
								destPlace, brkpt);
}
				
void Task::OneTimeFuncCall(std::string const & funcName, int nAttrs,
								Attribute * attrs, Breakpoint * brkpt) {
	ACProxy * proxy = GetACProxy();
	proxy->OneTimeFuncCall(_pid, funcName, nAttrs, attrs, brkpt);
}

void Task::RemoveFuncCall(std::string const & funcName,
						   std::string const & callerFunc, Breakpoint * brkpt) {
	ACProxy * proxy = GetACProxy();
	proxy->RemoveFuncCall(_pid, funcName, callerFunc, brkpt);
}
				
void Task::FuncParamChange(std::string const & funcName, int paramIdx,
							int newValue, int * requiredOldValue,
							Breakpoint * brkpt) {
	ACProxy * proxy = GetACProxy();
	proxy->FuncParamChange(_pid, funcName, paramIdx, newValue,
							requiredOldValue, brkpt);
}
				
void Task::SetTaskExitHandler(TaskHandler & h) {
	// ...
}
