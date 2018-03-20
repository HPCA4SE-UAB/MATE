
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
// Tuner.cpp
// 
// Tuner - manages task tuning
//
// Anna Sikora, UAB, 2000-2004
//
//----------------------------------------------------------------------

#include "Tuner.h"
#include "Syslog.h"
#include <assert.h>

// note: must delete req.
void Tuner::Process (TuningRequest * request)
{
	assert (request != 0);
	
	// find associated task
	int pid = request->GetPid ();
   	Task & task = _tasks.GetByPid (pid); // throws Exception
   	assert (pid == task.GetPid ());
   	
   	// has breakpoint ?
   	Breakpoint const * brkpt = request->GetBreakpoint ();
   	if (brkpt != 0)
   	{
   		Syslog::Debug ("[Tuner] Delaying tuning due to breakpoint");
   		// delay execution of this tuning request to breakpoint occurrence
   		task.AddDelayedTuning (request);
   		InsertBreakpoint (task, *brkpt);   		
   		return; 
   	}
   	else 
   		Syslog::Debug ("[Tuner] Breakpoint: none");
   	
   	// else process the request
   	switch (request->GetType ())
   	{
		case PTPLoadLibrary:
		{
			LoadLibraryRequest * r = (LoadLibraryRequest*)request;
			LoadLibrary (*r);
			break;
		}
		case PTPSetVariableValue:
		{
			SetVariableValueRequest * r = (SetVariableValueRequest*)request;
			SetVariableValue (*r);
			break;
		}
		case PTPReplaceFunction:
		{
			ReplaceFunctionRequest * r = (ReplaceFunctionRequest*)request;
			ReplaceFunction (*r);
			break;
		}
		case PTPInsertFuncCall:
		{
			InsertFunctionCallRequest * r = (InsertFunctionCallRequest*)request;
			InsertFunctionCall (*r);
			break;
		}
		case PTPOneTimeFuncCall:
		{
			OneTimeFunctionCallRequest * r = (OneTimeFunctionCallRequest*)request;
			OneTimeFuncCall (*r);
			break;
		}
		case PTPRemoveFuncCall:
		{
			RemoveFunctionCallRequest * r = (RemoveFunctionCallRequest*)request;
			RemoveFuncCall (*r);
			break;
		}
		case PTPFuncParamChange:
		{
			FunctionParamChangeRequest * r = (FunctionParamChangeRequest*)request;
			FuncParamChange (*r);
			break;
		}
	}
	delete request;
}
	
// LoadLibrary (int tid, char const * libPath);
void Tuner::LoadLibrary (LoadLibraryRequest & tuningReq)
{
	int pid = tuningReq.GetPid ();
	char path [256];
	strcpy (path, tuningReq.GetLibraryPath ().c_str ());
	Syslog::Debug ("[Tuner] Loading library");

   	Task & task = _tasks.GetByPid (pid); // throws Exception
   	assert (pid == task.GetPid ());
	task.GetProcess ().loadLibrary (path);
}

// SetVariableValue (int tid, char const * varName, char const * varValue, Breakpoint brkpt);
void Tuner::SetVariableValue (SetVariableValueRequest & tuningReq)
{
	int pid = tuningReq.GetPid ();
	Task & task = _tasks.GetByPid (pid); // throws Exception
	assert (pid == task.GetPid ());
		
	string variableName = tuningReq.GetVariableName ();
	string value = tuningReq.GetValueString ();
	
	Syslog::Debug ("[Tuner] SetVariableValue pid %d, variable %s = %s", 
				pid, variableName.c_str (), value.c_str ());
	task.GetProcess ().StopExecution ();
	Syslog::Debug ("[Tuner] SetVariableValue: Searching for the variable..."); 
	BPatch_variableExpr * variable = task.GetImage ().FindVariable (variableName.c_str ());
	if (variable != 0)
	{
		double * d = (double*)tuningReq.GetValueBuffer ();
		float f = (float)(*d);
		Syslog::Debug ("[Tuner] Writing variable value... %f with size %d",f, sizeof (f));
		variable->writeValue ((void*)&f, (int)sizeof(f));//tuningReq.GetValueBuffer (), tuningReq.GetValueSize ());
		Syslog::Debug ("[Tuner] Variable set");
		float readVar;
		variable->readValue ((void *)(&readVar));
		Syslog::Debug ("[Tuner] Read variable value is: %f", readVar);
		
	// this is temporal!!!!!!!!!!!
	// for now it is a patch, it should be changed to handle each type of coming variable
	// in this case this is for handle int variable changes
	// the code written before is for float type
	/*	void * d = (void*)tuningReq.GetValueBuffer ();
		variable->writeValue ((void*)d, (int)sizeof(int));//tuningReq.GetValueBuffer (), tuningReq.GetValueSize ());
		Syslog::Debug ("[Tuner] Variable set");
		int readVar;
		variable->readValue ((void *)(&readVar));
		Syslog::Debug ("[Tuner] Read variable value is: %d", readVar);
	*/
	}
	else
		Syslog::Warn ("[Tuner] SetVariableValue failed. Variable %s not found !", variableName.c_str ());
	task.GetProcess ().ContinueExecution ();
	Syslog::Debug ("[Tuner] SetVariableValue done");
}

// ReplaceFunction (int tid, char const * oldFunc, char const * newFunc, Breakpoint brkpt);
void Tuner::ReplaceFunction (ReplaceFunctionRequest & tuningReq)
{
	int pid = tuningReq.GetPid ();
	string const & oldFuncName = tuningReq.GetOldFunction ();
	string const & newFuncName = tuningReq.GetNewFunction ();
	Breakpoint * brkpt = tuningReq.GetBreakpoint ();
	
	Syslog::Debug ("[Tuner] ReplaceFunction pid %d, oldFunc %s, newFunc %s", 
				pid, oldFuncName.c_str (), newFuncName.c_str ());

	Task & task = _tasks.GetByPid (pid); // throws Exception
	assert (pid == task.GetPid ());
	
	DiFunction oldFunc (task.GetImage (), oldFuncName);
	DiFunction newFunc (task.GetImage (), newFuncName);
	task.GetProcess ().ReplaceFunction (oldFunc, newFunc);
	Syslog::Info ("[Tuner] ReplaceFunction done");
	task.Continue ();
}

// void InsertFunctionCall (int tid, char const * funcName, int nAttrs, Attribute * attrs
// 							char const * destFunc, InstrPlace destPlace, Breakpoint brkpt);
void Tuner::InsertFunctionCall (InsertFunctionCallRequest & tuningReq)
{
	int pid = tuningReq.GetPid ();
	string const & funcName = tuningReq.GetFuncName ();
	int nAttrs = tuningReq.GetAttrCount ();
	Attribute * attrs = tuningReq.GetAttributes ();
	string const & destFuncName = tuningReq.GetDestFunc ();
	InstrPlace place = tuningReq.GetInstrPlace ();
	Breakpoint * brkpt = tuningReq.GetBreakpoint ();
	
	Syslog::Debug ("[Tuner] InsertFunctionCall pid %d, func %s into destFunc %s", 
				pid, funcName.c_str (), destFuncName.c_str ());
	Syslog::Debug ("[Tuner] Attributes: %d", nAttrs);
	if (attrs) 
		attrs->Dump ();

	Task & task = _tasks.GetByPid (pid); // throws Exception
	assert (pid == task.GetPid ());
	DiImage & image = task.GetImage ();
	
	DiFunction func (image, funcName);
	BPatch_Vector<BPatch_snippet*> args;
	for (int i=0; i<nAttrs; i++)		
		args.push_back (new BPatch_constExpr (attrs[i].id.c_str()));

    BPatch_funcCallExpr funcCall (func, args); 
    
    DiPoint point (image, destFuncName.c_str (), BPatch_entry);
    switch (place)
	{
		case ipFuncEntry:
		{
			DiPoint point (image, destFuncName.c_str (), BPatch_entry);
			task.GetProcess ().InsertSnippet (funcCall, point);	
			break;
		}
		case ipFuncExit:
		{
			DiPoint point (image, destFuncName.c_str (), BPatch_exit);
			task.GetProcess ().InsertSnippet (funcCall, point);	
			break;
		}
	}
	Syslog::Debug ("[Tuner] InsertFunctionCall done"); 
}

// void OneTimeFuncCall (int tid, char const * funcName, int nAttrs, Attribute * attrs, Breakpoint brkpt);
void Tuner::OneTimeFuncCall (OneTimeFunctionCallRequest & tuningReq)
{
	int pid = tuningReq.GetPid ();
	string const & funcName = tuningReq.GetFuncName ();
	int nAttrs = tuningReq.GetAttrCount ();
	Attribute * attrs = tuningReq.GetAttributes ();
	
	Syslog::Debug ("[Tuner] OneTimeFuncCall pid %d, func %s", pid, funcName.c_str ());
	Syslog::Debug ("[Tuner] Attributes: %d", nAttrs);
	if (attrs) 
		attrs->Dump ();
		
	Task & task = _tasks.GetByPid (pid); // throws Exception
	assert (pid == task.GetPid ());
		
	DiFunction func (task.GetImage (), funcName);
	BPatch_Vector<BPatch_snippet*> args;
	for (int i=0; i<nAttrs; i++)		
		args.push_back (new BPatch_constExpr (attrs[i].id.c_str()));
	
    BPatch_funcCallExpr funcCall (func, args); 
    
    if (!task.GetProcess ().IsStopped ())
    {
    	Syslog::Debug ("[Tuner] WARNING Stopping running task...");
    	bool b = task.GetProcess ().StopExecution ();
    	if (!b || !task.GetProcess ().IsStopped ())
    		Syslog::Debug ("[Tuner] WARNING The task is still NOT stopped...");	
    }
	task.GetProcess ().OneTimeCode (funcCall);
	task.GetProcess ().ContinueExecution ();
	
	Syslog::Debug ("[Tuner] OneTimeCode executed"); 
}

// void RemoveFuncCall (int tid, char const * funcName, char const * callerFunc, Breakpoint brkpt);
void Tuner::RemoveFuncCall (RemoveFunctionCallRequest & tuningReq)
{
	int pid = tuningReq.GetPid ();
	string const & funcName = tuningReq.GetFuncName ();
	Breakpoint * brkpt = tuningReq.GetBreakpoint ();
	
	Syslog::Debug ("[Tuner] RemoveFuncCall pid %d, func %s NOT IMPLEMENTED", 
				pid, funcName.c_str ());

	Task & task = _tasks.GetByPid (pid); // throws Exception
	assert (pid == task.GetPid ());
	
/*	DiFunction func (task.GetImage (), funcName);
	BPatch_Vector<BPatch_snippet*> args;
	BPatch_constExpr attrs.??? (eventId);
    args.push_back (&eventIdExpr);  
    BPatch_funcCallExpr funcCall (func, args); 
	task.GetThread ().OneTimeCode (funcCall);	

	delete [] attrs;*/
}

// void FuncParamChange (int tid, char const * funcName, 
//						int paramIdx, int newValue, int * requiredOldValue, Breakpoint brkpt);
void Tuner::FuncParamChange (FunctionParamChangeRequest & tuningReq)
{
	int pid = tuningReq.GetPid ();
	string const & funcName = tuningReq.GetFuncName ();
	int paramIdx = tuningReq.GetParamIdx ();
	int newValue = tuningReq.GetNewValue ();
	int const * requiredOldValue = tuningReq.GetReqOldValue (); // 
	Breakpoint * brkpt = tuningReq.GetBreakpoint ();
	
	Syslog::Debug ("[Tuner] FuncParamChange pid %d, func %s, param %d, value %d", 
				pid, funcName.c_str (), paramIdx, newValue);

	Task & task = _tasks.GetByPid (pid); // throws Exception
	assert (pid == task.GetPid ());
	
	DiImage & image = task.GetImage ();
	
	DiFunction func (image, funcName);
	BPatch_paramExpr param (paramIdx);
	BPatch_constExpr newValueExpr (newValue);
	
/*	if (requiredOldValue)
	{
		// if param = oldValue
		BPatch_boolExpr 
	}
	else
	{
		
	}
	
	BPatch_ifExpr changeParamExpr (func, args); 
	DiPoint point (image, funcName.c_str (), BPatch_entry);
	task.GetProcess ().InsertSnippet (changeParamExpr, point);	*/
}

void Tuner::InsertBreakpoint (Task & task, Breakpoint const & brkpt)
{
	
    if (!task.GetProcess ().IsStopped ())
    {
    	Syslog::Debug ("[Tuner] WARNING Stopping running task...");
    	bool b = task.GetProcess ().StopExecution ();
    	if (!b || !task.GetProcess ().IsStopped ())
    		Syslog::Debug ("[Tuner] WARNING The task is still NOT stopped...");	
    }
    	
	BPatch_procedureLocation loc = (brkpt.place==ipFuncEntry) ? BPatch_entry : BPatch_exit;
	
	DiPoint point (task.GetImage (), brkpt.funcName, loc);	
	BPatch_breakPointExpr brkptExpr;
	BPatchSnippetHandle * h = task.GetProcess ().InsertSnippet (brkptExpr, point);
	task.GetInstr ().SetBreakpoint (h);
	
	Syslog::Debug ("[Tuner] Inserted breakpoint at: funcName: %s, place: %d", 
		brkpt.funcName.c_str(), brkpt.place);

	task.GetProcess ().ContinueExecution ();
}




void Tuner::RemoveLastBreakpoint (Task & task)
{
	BPatchSnippetHandle * h = task.GetInstr ().GetBreakpoint ();
	assert (h != 0);
	task.GetProcess ().DeleteSnippet (h);
	Syslog::Debug ("[Tuner] Removed breakpoint from task"); 
}

