
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
// Analyzer.cpp
// 
// Analyzer - module that performs analysis of the events
// 
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#include "Analyzer.h"
#include "ACProxy.h"
#include "Syslog.h"
#include <unistd.h>

static bool done = true;

std::string const & Analyzer::GetFuncDef (std::string const & name,
											int & paramCount, int & funcId) {
	FuncDef const & fd = _funcDefs.Find(name);
	paramCount = fd.GetParamCount();
	funcId = fd.GetFuncId();
	return fd.GetParamFormat();
}

int Analyzer::GetFuncId (std::string const & name) {
	FuncDef const & fd = _funcDefs.Find (name);
	return fd.GetFuncId ();
}

void Analyzer::Initialize (Config & config)
{
	// get functions - from the configuration - to be traced
	/*Config::KeyIterator kIter = config.GetKeys ("Functions");
	infoLog.LogEvent (0, "Analyzer Init. Adding functions ids to the map...");
	while (!kIter.AtEnd ())
	{
		_eventMap.Add (kIter.GetKey (), kIter.GetIntValue ());
		infoLog.LogEvent (1, "Function Name is: %s", kIter.GetKey ().c_str ());
		infoLog.LogEvent (1, "Function id is: %d", kIter.GetIntValue ());
		kIter.Next ();
	}*/
}
//=============================================================================

void Analyzer::AnalyzeEvent ()
{
	//if (_list.GetSize () == 10) // TEMPORAL
	Analyze();
	if (ProblemFound()){ //TEMPORAL
		Syslog::Debug ("PROBLEM FOUND!!!!!!!!!!!!!!!!!!!!!!!!");
		RemoveInstr ();
		sleep (5);
		Tune ();
	}
}

void Analyzer::Instrument ()
{
	//TEMPORAL
	std::string machine = "aows7";
	int tid = 262148;

	AddInstrReq (machine, tid, 1, "pvm_recv", ipFuncEntry, 0, 0);
	AddInstrReq (machine, tid, 1, "pvm_recv", ipFuncExit, 0, 0);
	
	Attribute attrs [2]; 
	//attrs[0].source = asVarValue;
	attrs[0].source = asFuncParamValue;
	//attrs[0].source = asFuncReturnValue;
	//attrs[0].source = asConstValue;
	
	//attrs[0].type = avtInteger;
	//attrs[0].type = avtFloat;
	attrs[0].type = avtChar;
	//attrs[0].type = avtString;
	
	//attrs[0].id = "_var";
	//attrs[0].id = "Temp2";
	attrs[0].id = "0";
	
	/*attrs[1].source = asFuncParamValue;
	attrs[1].type = avtFloat;
	attrs[1].id = "1";*/

	attrs[1].source = asFuncParamValue;
	attrs[1].type = avtFloat;
	attrs[1].id = "1";
	
	//AddInstrReq (machine, tid, 0, "pvm_send", ipFuncEntry, 2, attrs);
	AddInstrReq (machine, tid, 2, "Temp", ipFuncEntry, 2, attrs);
}

void Analyzer::Tune ()
{
	Syslog::Debug ("PERFORMANCE PROBLEM FOUND!!!!!!!!!!!!!!!!!!!!!!!!");
	RemoveInstr ();
	sleep (5);
	
	// temporal - these should be set after the analysis
	int tid = 262148;
	std::string machine = "aows7";
	Breakpoint b;
	b.funcName = "pvm_send";
	b.place = ipFuncEntry;
	
	/*
	ACProxy ac (machine);
	string oldFunc = "oldFunc";
	string newFunc = "newFunc";
	ac.ReplaceFunction (tid, oldFunc, newFunc, &b);
	Syslog::Debug ("[Analyzer] ReplaceFunction - tid: %d, oldFunc: %s, newFunc: %s\n", tid, oldFunc.c_str (), newFunc.c_str ());
	*/

	ACProxy ac (machine);
	string funcName = "oneTimeFunc";
	ac.OneTimeFuncCall (tid, funcName, 0, 0, 0);
	Syslog::Debug ("[Analyzer] OneTimeFuncCall - tid: %d, funcName: %s\n", 
			tid, funcName.c_str ());

	/*sleep (10);
	ACProxy ac2 (machine);
	string variable = "_var";
	int value = 5;
	ac2.SetVariableValue (tid, variable, value, &b);*/
}

void Analyzer::RemoveInstr ()
{
	//temporal!!!!!!!!!
	std::string machine = "aows7";
	std::string const functionName = "pvm_recv";
	int tid = 262148;
	int eventId = 1; //GetFuncId (functionName);
		
	RemoveInstrReq (machine, tid, eventId, ipFuncEntry);
}

void Analyzer::AddInstrReq (std::string & machine, int tid, int eventId, 
							std::string const & fName, InstrPlace place,
							int nAttrs, Attribute * attrs)
{
	Syslog::Debug ("Analyzer::AddInstrReq");
	ACProxy ac (machine);
	ac.AddInstr (tid, eventId, fName, place, nAttrs, attrs);
	Syslog::Debug ("Adding Instrumentation Message sent");
}

void Analyzer::RemoveInstrReq (std::string & machine, int tid, int eventId, InstrPlace place)
{
	Syslog::Debug ("Analyzer::RemoveInstrReq");
	ACProxy ac (machine);
	ac.RemoveInstr (tid, eventId, place);
	Syslog::Debug ("Removing Instrumentation Message sent");
}

void Analyzer::TuningReq (std::string & machine, int tid, std::string const & variable, 
					int value, Breakpoint * brkpt)
{
	ACProxy ac (machine);
	ac.SetVariableValue (tid, variable, value, brkpt);
	Syslog::Debug ("[Analyzer] SetVariableValue - tid: %d, variable: %s, value: %d\n", tid, variable.c_str (), value);
}

void Analyzer::Analyze ()
{
	Syslog::Debug ("Analyzing event...");
	int eventNum = _list.GetSize () - 1;
	Syslog::Debug ("Event num: %d", eventNum);
	EventMsg * event = _list.GetEvent (eventNum);
	InstrPlace place = ipFuncEntry;
	int eventId = 1; 		
	int fatherTid = 262148; // father tid
	
	Syslog::Debug ("Event is: %d, id: %d, place: %u", event->GetEventId (), event->GetPlace ());
	if ((event->GetEventId () == eventId))
	{
		Syslog::Debug ("Father has received event %d from a child, %d times", eventId, _receiveCount);
		_receiveCount++;
	}
}

int Analyzer::ProblemFound ()
{
	return _receiveCount == 5;
}

