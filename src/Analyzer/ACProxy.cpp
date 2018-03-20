
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
// TunerProxy.cpp
// 
// TunerProxy - class that connects to the tuner host
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#include "ACProxy.h"
#include "PTPMsg.h"
#include "Syslog.h"
#include <strstream>

void ACProxy::StartApplication(char const * appPath, int argc, 
							   char const ** argv, char const * analyzerHost) {

	StartAppRequest req(appPath, argc, argv, analyzerHost);
	Syslog::Debug("[ACProxy] StartApp: appPath: %s, argc %d", appPath, argc);
	for (int i = 0; i < argc; ++i) {
		Syslog::Debug("\tArg %d = %s", i, argv[i]);
		++i;
	}
	_protocol.WriteMessageEx(req, _socket);
}

void ACProxy::AddInstr(int tid, int eventId, std::string const & fName,
						InstrPlace place, int nAttrs, Attribute * attrs) {
	AddInstrRequest req(tid, eventId, fName, place, nAttrs, attrs);
	Syslog::Debug("[ACProxy] AddInstr: tid: %d, eventId: %d, function: %s, "
					"place: %d, nAttrs: %d", tid, eventId,
					fName.c_str(), place, nAttrs);
	/*for (int i=0; i<nAttrs; i++)
		attrs[i].Dump();*/


	_protocol.WriteMessageEx (req, _socket);
}

void ACProxy::AddInstr (int tid, int eventId, std::string const & fName, 
					   InstrPlace place, int nAttrs, Attribute * attrs,
					   int nPapi, std::string * PapiMetrics) {
	AddInstrRequest req(tid, eventId, fName, place, nAttrs, attrs, nPapi, PapiMetrics);
	Syslog::Debug("[ACProxy] AddInstr: tid: %d, eventId: %d, function: %s, "
					"place: %d, nAttrs: %d, nPapi %d", tid, eventId,
					fName.c_str(), place, nAttrs, nPapi);



	_protocol.WriteMessageEx (req, _socket);
}


void ACProxy::RemoveInstr(int tid, int eventId, InstrPlace place) {
	RemoveInstrRequest req (tid, eventId, place);
	Syslog::Debug ("[ACProxy] RemoveInstr: tid: %d, eventID: %d, place: %d",
					tid, eventId, place);
	_protocol.WriteMessageEx (req, _socket);
}

void ACProxy::LoadLibrary(int tid, string const & libPath) {
	LoadLibraryRequest req (tid, libPath);
	Syslog::Debug ("[ACProxy] LoadLibrary: tid: %d, lib: %s", tid, 
					libPath.c_str ());

	_protocol.WriteMessageEx (req, _socket);
}

void ACProxy::SetVariableValue(int tid, string const & varName,
					AttributeValue const & varValue, Breakpoint * brkpt) {

	SetVariableValueRequest req (tid, varName, varValue, brkpt);
	Syslog::Debug ("[ACProxy] SetVariableValue tid %d, variable %s, value %s", 
				tid, varName.c_str (), varValue.ToString ().c_str ());

	if (brkpt != 0) 
		Syslog::Debug ("[ACProxy] Breakpoint: funcName: %s, place: %d", 
						brkpt->funcName.c_str(), brkpt->place);

	_protocol.WriteMessageEx (req, _socket);
}

void ACProxy::ReplaceFunction(int tid, string const & oldFunc,
					string const & newFunc, Breakpoint * brkpt) {
	ReplaceFunctionRequest req (tid, oldFunc, newFunc, brkpt);
	Syslog::Debug ("[ACProxy] ReplaceFunction tid %d, oldFunc %s, newFunc %s", 
					tid, oldFunc.c_str (), newFunc.c_str ());
	if (brkpt != 0) 
		Syslog::Debug ("[ACProxy] Breakpoint: funcName: %s, place: %d", 
						brkpt->funcName.c_str(), brkpt->place);

	_protocol.WriteMessageEx (req, _socket);	
}

void ACProxy::InsertFunctionCall(int tid, string const & funcName,
					int nAttrs, Attribute * attrs, string const & destFunc,
					InstrPlace destPlace, Breakpoint * brkpt) {

	InsertFunctionCallRequest req (tid, funcName, nAttrs, attrs, destFunc,
									destPlace, brkpt);
	Syslog::Debug ("[ACProxy] InsertFunctionCall tid %d, func %s into destFunc %s", 
				tid, funcName.c_str (), destFunc.c_str ());
	for (int i = 0; i < nAttrs; i++)
		Syslog::Debug ("[ACProxy] Attributes -> %d -> id: %s",
						i, attrs[i].id.c_str ());

	if (brkpt != 0) 
		Syslog::Debug ("[ACProxy] Breakpoint: funcName: %s, place: %d", 
						brkpt->funcName.c_str(), brkpt->place);

	_protocol.WriteMessageEx (req, _socket);
}

void ACProxy::OneTimeFuncCall(int tid, string const & funcName,
					int nAttrs, Attribute * attrs, Breakpoint * brkpt) {

	OneTimeFunctionCallRequest req (tid, funcName, nAttrs, attrs, brkpt);
	Syslog::Debug ("[ACProxy] OneTimeFuncCall tid %d, func %s, nAttrs: %d", 
					tid, funcName.c_str (), nAttrs);
	for (int i = 0; i < nAttrs; i++)
		Syslog::Debug ("[ACProxy] Attributes -> %d -> id: %s",
						i, attrs[i].id.c_str ());

	if (brkpt != 0) 
		Syslog::Debug ("[ACProxy] Breakpoint: funcName: %s, place: %d", 
						brkpt->funcName.c_str(), brkpt->place);
	else
		Syslog::Debug ("[ACProxy] Breakpoint: none");
	
	_protocol.WriteMessageEx (req, _socket);
}

void ACProxy::RemoveFuncCall(int tid, string const & funcName,
					string const & callerFunc, Breakpoint * brkpt) {
	RemoveFunctionCallRequest req (tid, funcName, callerFunc, brkpt);
	Syslog::Debug ("[ACProxy] RemoveFuncCall tid %d, func %s", tid, 
					funcName.c_str ());
	if (brkpt != 0) 
		Syslog::Debug ("[ACProxy] Breakpoint: funcName: %s, place: %d", 
						brkpt->funcName.c_str(), brkpt->place);

	_protocol.WriteMessageEx (req, _socket);
}

void ACProxy::FuncParamChange(int tid, string const & funcName,
					int paramIdx, int newValue, int * requiredOldValue,
					Breakpoint * brkpt) {
	FunctionParamChangeRequest req(tid, funcName, paramIdx, newValue,
									requiredOldValue, brkpt);
	Syslog::Debug ("[ACProxy] FuncParamChange tid %d, func %s, param %d, value %d", 
					tid, funcName.c_str (), paramIdx, newValue);
	if (brkpt != 0) 
		Syslog::Debug ("[ACProxy] Breakpoint: funcName: %s, place: %d", 
						brkpt->funcName.c_str(), brkpt->place);

	_protocol.WriteMessageEx (req, _socket);	
}
