
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
// Monitor.cpp
// 
// Monitor - manages task instrumentation
//
// Anna Sikora, UAB, 2000-2004
//
//----------------------------------------------------------------------

#include "Monitor.h"
#include "SnippetMaker.h"
#include "Syslog.h"
#include <semaphore.h>
#include <fcntl.h>
#include <string>
#include <sstream>
#include <iostream>
#include <time.h>

// JORDI
void sendSignal(int pid)
{

	std::stringstream ss;
	ss << pid;
	std::string sem_name = ss.str();
	sem_t *sem = sem_open(sem_name.c_str(), O_CREAT, S_IRWXU, 0);
	if(sem == SEM_FAILED)
	{
		printf("Error open\n");
		return;
	}
	sem_post(sem);
	Syslog::Debug (" SIGNAL SENT TO %d!!!!-----------]\n", pid);
	sem_close(sem);
	sem_unlink(sem_name.c_str());
}

void Monitor::AddInstr (AddInstrRequest & instrReq)
{
	Syslog::Debug ("[Monitor] Add instr...");
	int pid = instrReq.GetPid ();
	int eventId = instrReq.GetEventId ();
	string const fName = instrReq.GetFunctionName ();
	InstrPlace instrPlace = instrReq.GetInstrPlace ();
	int nAttrs = instrReq.GetAttrsCount ();
	Attribute * attrs = instrReq.GetAttributes ();
	int nPapi = instrReq.GetMetricsCount();
	std::string * PapiMetrics = instrReq.GetMetrics();
	if(fName.compare("MonitorSignal") == 0)
	{
		sendSignal(pid);
		Syslog::Debug ("[Monitor] Sending signal...");
		return ;
	}

	Syslog::Debug ("[Monitor] Add instr: pid: %d, eventID: %d, function: %s, place: %d, nAttrs: %d, nPapi: %d",
			pid, eventId, fName.c_str (), instrPlace, nAttrs, nPapi);
	for (int i=0; i<nAttrs; i++)
		attrs[i].Dump ();
	for (int i=0; i<nPapi; i++)
		Syslog::Debug ("Metric: %s", PapiMetrics[i].c_str());
   	Task & task = _tasks.GetByPid (pid); // throws Exception
   	assert (pid == task.GetPid ());
   	
   	// prepare a snippet
	SnippetMaker maker (task.GetProcess (), task.GetImage ());
	BPatchSnippetHandle * handle = maker.MakeEventSnippet (eventId, fName, instrPlace,
		nAttrs, attrs, nPapi, PapiMetrics);


	if(instrPlace == ipFuncEntry)
	{//initialize PAPI EventSet

		Syslog::Debug ("nPapi: %d", nPapi);
		DiFunction createFunc (task.GetImage (), "DMLib_ECreate");
	    BPatch_Vector<BPatch_snippet*> createArgs;
	    BPatch_Vector<BPatch_snippet*> statements;
	    BPatch_constExpr func_name (fName.c_str() );
	    createArgs.push_back (&func_name);
	    BPatch_funcCallExpr  callCreate(createFunc, createArgs);
		statements.push_back (&callCreate);
		BPatch_sequence seq (statements);
		
		
		
		if(nPapi>0)
		{
			Syslog::Debug ("Call DMLib_ECreate");
			task.GetProcess().OneTimeCode(seq);
			Syslog::Debug ("Call DMLib_ECreate prepared");
		}
		//add PAPI metrics to EventSet
		Syslog::Debug ("For each metric....");
		for (int i=0; i<nPapi; ++i)
		{
			DiFunction AddMFunc (task.GetImage (), "DMLib_PAPIAdd");
		    BPatch_Vector<BPatch_snippet*> metricsArgs;
		    BPatch_Vector<BPatch_snippet*> statementsP;
		    BPatch_constExpr metricExpr (PapiMetrics[i].c_str());
		    metricsArgs.push_back (&metricExpr);
		    metricsArgs.push_back(&func_name);
		    BPatch_funcCallExpr  callAddM(AddMFunc, metricsArgs);
			statementsP.push_back (&callAddM);
			BPatch_sequence seqP (statementsP);
			task.GetProcess().OneTimeCode(seqP);
			Syslog::Debug ("Param %s prepared", PapiMetrics[i].c_str());
		}
	}




	// add a handle to the inserted snippet
	task.GetInstr ().Add (eventId, fName, instrPlace, handle);
	Syslog::Debug ("[Monitor] Add instr done");
	task.GetProcess ().ContinueExecution ();
}

void Monitor::RemoveInstr (RemoveInstrRequest & instrReq)
{
	Syslog::Debug ("Monitor:: remove instr ...");	
	int pid = instrReq.GetPid ();
	int eventId = instrReq.GetEventId ();
	InstrPlace instrPlace = instrReq.GetInstrPlace ();
	
	Syslog::Debug ("[Monitor] Remove instr: pid: %d, eventID: %d, place: %d",
				pid, eventId, instrPlace);

	Task & task = _tasks.GetByPid (pid); // throws Exception
   	assert (pid == task.GetPid ());
   	
   	InstrGroup * group = task.GetInstr ().FindGroup (eventId);
	assert (group != 0);
	InstrGroup::Iterator it = group->begin ();
	while (it != group->end ())
	{
		SnippetHandler * handler = (*it);
		assert (handler != 0);
		if (handler->GetInstrPlace () == instrPlace)
		{
			//Syslog::Debug ("Removing snippet...");
			task.GetProcess ().DeleteSnippet (handler->GetHandle ());	
			//Syslog::Debug ("Snippet removed");
		}
		it++;
	}
	task.GetInstr ().Remove (eventId, instrPlace);
	Syslog::Debug ("Monitor:: remove instr done");	
}

