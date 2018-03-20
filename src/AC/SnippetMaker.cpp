
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
// SnippetMaker.cpp
// 
// SnippetMaker - class that creates snippets
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#include "SnippetMaker.h"
#include "Syslog.h"

// DMLib_OpenEvent (eventId, instrPlace, paramCount);
// for each param:
//	  DMLib_AddxxParam (value);
// DMLib_CloseEvent ();

// np. void foo (int a, float f):
//
// DMLib_OpenEvent (1, instrEntry, 2);
// DMLib_AddIntParam (attr [0]);
// DMLib_AddFloatParam (attr [1]);
// DMLib_CloseEvent ();
BPatchSnippetHandle * SnippetMaker::MakeEventSnippet (int eventId, 
								std::string const & funcName,
								InstrPlace instrPlace, 
								int nAttrs, 
								Attribute * attrs,
								int nPapi,
								std::string * PapiMetrics)
{
	typedef BPatch_Vector<BPatch_snippet*> SnippetVector;
	Syslog::Debug ("SnippetMaker::MakeEventSnippet");
	SnippetVector statements;
	// 1. Call DMLib_OpenEvent (eventId, instrPlace, paramCount);	
	Syslog::Debug ("Call DMLib_OpenEvent");
    DiFunction openFunc (_image, "DMLib_OpenEvent");
    SnippetVector openArgs;
    BPatch_constExpr eventIdExpr (eventId);
    BPatch_constExpr placeExpr (instrPlace);


    int nAttrs_Papi = nAttrs;
    if(instrPlace == ipFuncExit)
    	nAttrs_Papi = nAttrs + nPapi;


    BPatch_constExpr attrsCountExpr (nAttrs_Papi);
    openArgs.push_back (&eventIdExpr);    
    openArgs.push_back (&placeExpr);    
    openArgs.push_back (&attrsCountExpr);
    BPatch_funcCallExpr callOpen (openFunc, openArgs);
    statements.push_back (&callOpen);
    Syslog::Debug ("Call DMLib_OpenEvent prepared");

	// 2. For each param: DMLib_AddxxParam (value);
    Syslog::Debug ("For each param....");
	for (int i=0; i<nAttrs; ++i)
	{
		std::string name = GetAddParamName (attrs [i]);
		DiFunction addFunc (_image, name.c_str ());
		SnippetVector * addArgs = new SnippetVector ();
		addArgs->push_back (MakeSourceSnippet (attrs[i]));
		BPatch_funcCallExpr * callAddParam = new BPatch_funcCallExpr (addFunc, *addArgs);  
		statements.push_back (callAddParam);
		Syslog::Debug ("Param %d prepared", i);
	}	


	//IF startfunc, start..., start PAPI counter
	//IF endfunc, end...., return PAPI values
	// 2.1 initialize Papi counter if ipFuncEntry
	BPatch_Vector<BPatch_snippet*> PapiArgs;
	BPatch_constExpr func_name (funcName.c_str() );
	PapiArgs.push_back(&func_name);
	std::string PapiFuncName;
	if(instrPlace == ipFuncEntry)
	{
		PapiFuncName = "DMLib_PAPI_Start";
	}	
	else if(instrPlace == ipFuncExit)
	{
		PapiFuncName = "DMLib_PAPI_Stop";
	}
	DiFunction PapiFunc (_image, PapiFuncName.c_str());
	BPatch_funcCallExpr callPapi (PapiFunc, PapiArgs);
	if(nPapi>0)
		statements.push_back(&callPapi);

	Syslog::Debug ("Call DMLib_Close");
	DiFunction closeFunc (_image, "DMLib_CloseEvent");
	SnippetVector closeArgs;
	BPatch_funcCallExpr callClose (closeFunc, closeArgs); 
	statements.push_back (&callClose);
		
	// ok, snippet is ready. insert it.
	BPatch_sequence seq (statements);
	Syslog::Debug ("Snippet prepared");
	BPatchSnippetHandle * handle = InsertSnippet (funcName, instrPlace, seq);
	Syslog::Debug ("Snippet inserted...");


	return handle;
}

BPatchSnippetHandle * SnippetMaker::InsertSnippet (std::string const & funcName,
	InstrPlace instrPlace, BPatch_snippet & snippet)
{
	switch (instrPlace)
	{
		case ipFuncEntry:
		{
			Syslog::Debug ("SnippetMaker::InsertSnippet instrEntry");
			DiPoint point (_image, funcName.c_str (), BPatch_entry);
			return _process.InsertSnippet (snippet, point);	
		}
		case ipFuncExit:
		{
			Syslog::Debug ("SnippetMaker::InsertSnippet instrExit");
			DiPoint point (_image, funcName.c_str (), BPatch_exit);
			return _process.InsertSnippet (snippet, point);	
		}
		default:
		{
			throw "InsertSnippet failed. Unknown instrumentation place";
			return 0;
		}
	}	
}

std::string SnippetMaker::GetAddParamName (Attribute const & attr)
{
	switch (attr.type)
	{
	case avtInteger:
		return "DMLib_AddIntParam";
	case avtFloat:
		return "DMLib_AddFloatParam";
	case avtDouble:
		return "DMLib_AddDoubleParam";
	case avtChar:
		return "DMLib_AddCharParam";
	case avtString:
		return "DMLib_AddStringParam";
	default:
		throw "Unsupported parameter type";
		break;			
	}
}

BPatch_snippet * SnippetMaker::MakeSourceSnippet (Attribute const & attr)
{
	switch (attr.source)
	{
	case asFuncParamValue: // id represents function param index
	{
		int attrNum = atoi(attr.id.c_str());
		Syslog::Debug ("SnippetMaker::MakeSourceSnippet for asFuncParamValue with idx: %d", attrNum);
		return new BPatch_paramExpr (attrNum);
	}
	case asVarValue: // id represents variable name
	{
		Syslog::Debug ("SnippetMaker::MakeSourceSnippet for asVarValue for variable: %s", attr.id.c_str());
		return _image.FindVariable (attr.id.c_str());
	}
	case asFuncParamPointerValue: // id represents function param index
	{
		int attrNum = atoi(attr.id.c_str());
		Syslog::Debug ("SnippetMaker::MakeSourceSnippet for asFuncParamPointerValue with idx: %d", attrNum);
		BPatch_paramExpr * e = new BPatch_paramExpr (attrNum);
		return new BPatch_arithExpr (BPatch_deref, *e);
	}
	case asFuncReturnValue:
	{
		Syslog::Debug ("SnippetMaker::MakeSourceSnippet for asFuncReturnValue = %s", attr.id.c_str());
		DiFunction foo (_image, attr.id.c_str());
		BPatch_Vector<BPatch_snippet*> args;
		BPatch_funcCallExpr * funcCallExpr = new BPatch_funcCallExpr (foo, args);
		return funcCallExpr;
	}
	case asConstValue:
	{
		Syslog::Debug ("SnippetMaker::MakeSourceSnippet for asConstValue = %s", attr.id.c_str());
		// note: DynInst does not support all datatypes. Supported types: char*, int.
		switch (attr.type)
		{
		case avtInteger:
			return new BPatch_constExpr (atoi(attr.id.c_str()));
		case avtString:
			return new BPatch_constExpr (attr.id.c_str());
		default:
			throw "DynInst does not support this type as BPatch_constExpr";
		}
	}
	/*case asSentMessageSize:
	{
		// call for special function from DMLib that generates size of sent message
		Syslog::Debug ("MakeSourceSnippet for asSentMessageSize for function = %s\n", attr.id.c_str());
		DiFunction sentMsgSizeInfo (_image, "DMLib_SentMsgSizeInfo");
		BPatch_Vector<BPatch_snippet*> args;
		BPatch_funcCallExpr * funcCallExpr = new BPatch_funcCallExpr (sentMsgSizeInfo, args);
		return funcCallExpr;
	}
	case asRecvMessageSize:
	{
		// call for special function from DMLib that generates size of received message
		Syslog::Debug ("MakeSourceSnippet for asRecvMessageSize for function = %s\n", attr.id.c_str());
		DiFunction recvMsgSizeInfo (_image, "DMLib_RecvMsgSizeInfo");
		BPatch_Vector<BPatch_snippet*> args;
		BPatch_funcCallExpr * funcCallExpr = new BPatch_funcCallExpr (recvMsgSizeInfo, args);
		return funcCallExpr;
	}*/
	}
}


