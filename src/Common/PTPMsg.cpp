
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
// PTPMsg.h
// 
// PTPMsg - represent messages that are interchanged between 
//			analyzer and tuner/tracer
//
// Anna Sikora, UAB, 2003
//
//----------------------------------------------------------------------

#include "PTPMsg.h"
#include <stdio.h>

using namespace std;
using namespace Common;

int PTPMessage::GetDataSize () const
{
	CountingSerializer ser;
	Serialize (ser);
	return ser.GetSize ();
}

void TuningRequest::ClearBreakpoint ()
{
	delete _brkpt;
	_brkpt = 0;
}

static void PutBrkpt (Serializer & out, Breakpoint * b)
{
	if (b != 0) 
	{
		out.PutByte (1);
		b->Serialize (out);
	}
	else 
		out.PutByte (0);
}

static void GetBrkpt (DeSerializer & in, Breakpoint ** b)
{
	
	delete *b;
	if (in.GetByte () == 1)
	{
		*b = new Breakpoint ();
		(*b)->DeSerialize (in);
	}
	else
		*b = 0;
}

static void PutAttrs (Serializer & out, Attribute * attrs, int nAttrs)
{
	out.PutInt (nAttrs);
	for (int i=0; i<nAttrs; i++)
		attrs[i].Serialize (out); 
}

static void GetAttrs (DeSerializer & in, Attribute * & attrs, int & nAttrs)
{
	delete [] attrs;
	nAttrs = in.GetInt ();
	if (nAttrs == 0)
		attrs = 0;
	else 
	{
		attrs = new Attribute [nAttrs];
		for (int i=0; i<nAttrs; i++)
			attrs[i].DeSerialize (in);
	}
}

static void PutMetrics (Serializer & out, std::string * PapiMetrics, int nMetrics)
{
	out.PutInt (nMetrics);
	for (int i=0; i<nMetrics; i++)
		//PutString
		out.PutString(PapiMetrics[i]);

		//PapiMetrics[i].Serialize (out); 
}

static void GetMetrics (DeSerializer & in, std::string * & PapiMetrics, int & nMetrics)
{
	delete [] PapiMetrics;
	nMetrics = in.GetInt ();
	if (nMetrics == 0)
		PapiMetrics = 0;
	else 
	{
		PapiMetrics = new std::string [nMetrics];
		for (int i=0; i<nMetrics; i++)
			//GetString
			PapiMetrics[i] = in.GetString();
			//PapiMetrics[i].DeSerialize (in);
	}
}

void TuningRequest::Serialize (Serializer & out) const
{
	out.PutInt (_pid);
	::PutBrkpt (out, _brkpt);
//	Syslog::Debug ("[TuningRequest] Breakpoint: serialized");
}

void TuningRequest::DeSerialize (DeSerializer & in)
{
	_pid = in.GetInt ();
	::GetBrkpt (in, &_brkpt);
}

//------------------
void LoadLibraryRequest::Serialize (Serializer & out) const
{
	TuningRequest::Serialize (out);
	out.PutString (_libPath);
}

void LoadLibraryRequest::DeSerialize (DeSerializer & in)
{
	TuningRequest::DeSerialize (in);
	_libPath = in.GetString ();
}

//------------------

void SetVariableValueRequest::Serialize (Serializer & out) const
{
	TuningRequest::Serialize (out);
	out.PutString (_varName);
	_varValue.Serialize (out);
}

void SetVariableValueRequest::DeSerialize (DeSerializer & in)
{
	TuningRequest::DeSerialize (in);
	_varName = in.GetString ();	
	_varValue.DeSerialize (in);
}

//------------------
void ReplaceFunctionRequest::Serialize (Serializer & out) const
{
	TuningRequest::Serialize (out);
	out.PutString (_oldFunc);
	out.PutString (_newFunc);
}

void ReplaceFunctionRequest::DeSerialize (DeSerializer & in)
{
	TuningRequest::DeSerialize (in);
	_oldFunc = in.GetString ();
	_newFunc = in.GetString ();
}

//------------------
void InsertFunctionCallRequest::Serialize (Serializer & out) const
{
	TuningRequest::Serialize (out);

	out.PutString (_funcName);
	::PutAttrs (out, _attrs, _nAttrs);
	out.PutString (_destFunc);
	out.PutInt ((int)_place);
}

void InsertFunctionCallRequest::DeSerialize (DeSerializer & in)
{
	TuningRequest::DeSerialize (in);
	_funcName = in.GetString ();
	::GetAttrs (in, _attrs, _nAttrs);
	_destFunc = in.GetString ();
	_place = (InstrPlace)in.GetInt ();
}

//------------------
void OneTimeFunctionCallRequest::Serialize (Serializer & out) const
{
	TuningRequest::Serialize (out);
	out.PutString (_funcName);
	::PutAttrs (out, _attrs, _nAttrs);	
}

void OneTimeFunctionCallRequest::DeSerialize (DeSerializer & in)
{
	TuningRequest::DeSerialize (in);
	_funcName = in.GetString ();
	::GetAttrs (in, _attrs, _nAttrs);	
}

//------------------
void RemoveFunctionCallRequest::Serialize (Serializer & out) const
{
	TuningRequest::Serialize (out);
	out.PutString (_funcName);
	out.PutString (_callerFunc);
}

void RemoveFunctionCallRequest::DeSerialize (DeSerializer & in)
{
	TuningRequest::DeSerialize (in);

	_funcName = in.GetString ();
	_callerFunc = in.GetString ();
}

//------------------
void FunctionParamChangeRequest::Serialize (Serializer & out) const
{
	TuningRequest::Serialize (out);
	out.PutString (_funcName);
	out.PutInt (_paramIdx);
	out.PutInt (_newValue);
	out.PutByte (_hasOldValue);
	out.PutInt (_reqOldValue);
}

void FunctionParamChangeRequest::DeSerialize (DeSerializer & in)
{
	TuningRequest::DeSerialize (in);

	_funcName = in.GetString ();
	_paramIdx = in.GetInt ();
	_newValue = in.GetInt ();
	_hasOldValue = in.GetByte ();
	_reqOldValue = in.GetInt ();
}
//------------------
void AddInstrRequest::Serialize (Serializer & out) const
{
	out.PutInt (_pid);
	out.PutString (_funcName);
	out.PutInt (_eventId);
	out.PutInt ((int)_instrPlace);
	::PutAttrs (out, _attrs, _nAttrs);
	::PutMetrics (out, _PapiMetrics, _nPapi);
}

void AddInstrRequest::DeSerialize (DeSerializer & in)
{
	_pid = in.GetInt ();
	_funcName = in.GetString ();
	_eventId = in.GetInt ();
	_instrPlace = (InstrPlace)in.GetInt ();
	::GetAttrs (in, _attrs, _nAttrs);
	::GetMetrics (in, _PapiMetrics, _nPapi);	
}

//------------------
void RemoveInstrRequest::Serialize (Serializer & out) const
{
	out.PutInt (_pid);
	out.PutInt (_eventId);
	out.PutInt ((int)_instrPlace);
}

void RemoveInstrRequest::DeSerialize (DeSerializer & in)
{
	_pid = in.GetInt ();
	_eventId = in.GetInt ();
	_instrPlace = (InstrPlace)in.GetInt ();
}

StartAppRequest::StartAppRequest (string const & appPath, int argc, char const ** argv,
			string const & analyzerHost)
	: _argc (argc), _argv (0)
{
	if (appPath.size() != 0)
		_appPath = appPath;
	if (analyzerHost.size() != 0)
		_analyzerHost = analyzerHost;
	if (argv != 0)
	{
		_argv = new char * [_argc+1];
		for (int i=0; i<argc; ++i)
		{
			_argv [i] = strdup (argv [i]);
		}
		_argv [_argc] = 0;
	}
	//Syslog::Debug ("[StartAppRequest::StartAppRequest] done");
}	
StartAppRequest::~StartAppRequest ()
{
	if (_argv != 0)
	{
		for (int i=0; i<_argc; ++i)
			free (_argv [i]);
		delete [] _argv;
	}
}

void StartAppRequest::Serialize (Serializer & out) const
{
	out.PutString (_appPath);	
	out.PutInt (_argc);
	for (int i=0; i<_argc; ++i)
		out.PutString (_argv [i]);
	out.PutString (_analyzerHost);
	//Syslog::Debug ("[StartAppRequest] serialized");
}

void StartAppRequest::DeSerialize (DeSerializer & in)
{
	_appPath = in.GetString ();
	// clean
	if (_argv != 0)
	{
		for (int i=0; i<_argc; ++i)
			free (_argv [i]);
		delete [] _argv;
	}
	_argc = in.GetInt ();
	_argv = new char * [_argc+1];
	for (int i=0; i<_argc; ++i)
	{
		string s = in.GetString ();
		_argv [i] = strdup (s.c_str ());
		//Syslog::Debug ("[StartAppRequest] args %d -> %s", i, _argv [i]);
	}
	_argv [_argc] = 0;
	_analyzerHost = in.GetString ();
	//Syslog::Debug ("[StartAppRequest] deserialized");
}
