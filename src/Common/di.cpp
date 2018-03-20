
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
// di.cpp
//
// Encapsulation of DynInst classes
//
// UAB, 2000
//
//----------------------------------------------------------------------

#include "di.h"
#include <stdio.h>
#include "Syslog.h"
//#include <iostream>

BPatch DynInst::_bp;

bool DynInst::_registered = false;

void DynInst::OnError (BPatchErrorLevel severity,
				    	 int number,
				    	 const char* const* params)
{
	if (number == 0) 
	{
   		// conditional reporting of warnings and informational messages
        if (severity == BPatchInfo)
        {
        	Syslog::Debug ("DynInst Info: %s", params[0]);
       		printf ("[DynInst]: %s\n", params[0]);
        }
        else
        {
        	Syslog::Debug ("DynInst Warning: %s", params[0]);
        }
    }
	else 
	{
    	// reporting of actual errors
    	char line[256];
    	const char * pattern = _bp.getEnglishErrorString (number);
    	_bp.formatErrorString (line, sizeof(line), pattern, params);
    	Syslog::Debug ("DynInst error: %d, %d, %s", number, severity, line);
	}
	// We consider some errors fatal.
	if (number == 101) 
	{
		Syslog::Fatal ("Exiting due to DynInst fatal error");
   		exit(-1);
   	}
}

//--------------------------------------------------------------------------

// Attach to the program (to a running process)
DiProcess::DiProcess (char * mutateeName, int pid)
{
    assert (mutateeName != NULL);
	BPatch & bp = DynInst::Instance ();
	_bpProcess = bp.processAttach (mutateeName, pid);
	if (_bpProcess == NULL)
	   throw DiEx ("Attach to the program failed", mutateeName);		   
}

// Create the program with given arguments
// argv cannot be null
DiProcess::DiProcess (char * mutateeName, char * argv[], char * envp[])
{
	assert (mutateeName != NULL);
    assert (argv != NULL);
	//BPatch & bp = DynInst::Instance ();

	BPatch *test_bp = new BPatch();
	BPatch & bp = *test_bp;
	_bpProcess = bp.processCreate (mutateeName,(const char **)argv,(const char **)envp);
	//DynInst::Instance (bp);
	//DynInst::Instance ();

	if (_bpProcess == NULL)
		throw DiEx ("Process creation failed", mutateeName);
}

// Create the program
DiProcess::DiProcess (char * mutateeName)
{
    assert (mutateeName != NULL);
    BPatch & bp = DynInst::Instance ();
	char * argv[] = { mutateeName, NULL };
	_bpProcess = bp.processCreate (mutateeName,(const char **)argv);
	if (_bpProcess == NULL)
		throw DiEx ("Process creation failed", mutateeName);
}

DiProcess::~DiProcess ()
{
//  Syslog::Debug ("Destroying DiProcess");
	// Process owns the BPatch_process so it must delete _bpProcess
//	delete _bpProcess;
}

// Insert a given snippet into given points
BPatchSnippetHandle * DiProcess::InsertSnippet (BPatch_snippet const & expr, BPatch_point & point)
{ 
	_bpProcess->stopExecution ();
	BPatchSnippetHandle * handle = _bpProcess->insertSnippet (expr, point);
//	ContinueExecution ();
	if (handle == NULL)
		throw DiEx ("Insert snippet failed");
	return handle;
}

BPatchSnippetHandle * DiProcess::InsertSnippet (BPatch_snippet const & expr, BPatch_point & point, 
 			BPatch_callWhen when, BPatch_snippetOrder order)
{
	BPatchSnippetHandle * handle = _bpProcess->insertSnippet (expr, point, when, order);
	if (handle == NULL)
		throw DiEx ("Insert snippet failed");
	return handle;
}
 			
// Insert a given snippet before or after a given points
BPatchSnippetHandle * DiProcess::InsertSnippetBefore (BPatch_snippet const & expr, BPatch_point & point)
{ 
	BPatchSnippetHandle * handle = _bpProcess->insertSnippet (expr, point, BPatch_callBefore, BPatch_firstSnippet);	
	if (handle == NULL)
		throw DiEx ("Insert snippet before failed");
	return handle;
}

BPatchSnippetHandle * DiProcess::InsertSnippetAfter (BPatch_snippet const & expr, BPatch_point & point)
{
	BPatchSnippetHandle * handle = _bpProcess->insertSnippet (expr, point, BPatch_callAfter, BPatch_firstSnippet);	
	if (handle == NULL)
		throw DiEx ("Insert snippet After failed");
	return handle;
}


// Insert a given snippet into given points
BPatchSnippetHandle * DiProcess::InsertSnippet (BPatch_snippet const & expr, PointVector & points)
{ 
	_bpProcess->stopExecution ();
	BPatchSnippetHandle * handle = _bpProcess->insertSnippet (expr, points);
//	ContinueExecution ();
	if (handle == NULL)
		throw DiEx ("Insert snippet failed");
	return handle;
}

BPatchSnippetHandle * DiProcess::InsertSnippet (BPatch_snippet const & expr, PointVector & points, 
 			BPatch_callWhen when, BPatch_snippetOrder order)
{
	BPatchSnippetHandle * handle = _bpProcess->insertSnippet (expr, points, when, order);
	if (handle == NULL)
		throw DiEx ("Insert snippet failed");
	return handle;
}
 			
// Insert a given snippet before or after a given points
BPatchSnippetHandle * DiProcess::InsertSnippetBefore (BPatch_snippet const & expr, PointVector & points)
{ 
	BPatchSnippetHandle * handle = _bpProcess->insertSnippet (expr, points, BPatch_callBefore, BPatch_firstSnippet);	
	if (handle == NULL)
		throw DiEx ("Insert snippet before failed");
	return handle;
}

BPatchSnippetHandle * DiProcess::InsertSnippetAfter (BPatch_snippet const & expr, PointVector & points)
{
	BPatchSnippetHandle * handle = _bpProcess->insertSnippet (expr, points, BPatch_callAfter, BPatch_firstSnippet);	
	if (handle == NULL)
		throw DiEx ("Insert snippet After failed");
	return handle;
}



void DiProcess::DeleteSnippet (BPatchSnippetHandle * handle)
{
	bool done = _bpProcess->deleteSnippet (handle);
	if (!done)
	{
		throw DiEx ("Deleting snippet failed. Handle is not defined.");
	}
}

// Execute given snippet once
void DiProcess::OneTimeCode (BPatch_snippet const & expr)
{
	if (_bpProcess->isTerminated ())
		Syslog::Debug ("Process already terminated");
	if (_bpProcess->isStopped ())
		Syslog::Debug ("Process is stopped");
	_bpProcess->oneTimeCode (expr);
}

// Replace function call with call to another function
void DiProcess::ReplaceFunction (BPatch_function & oldFunc, BPatch_function & newFunc)
{
	if (!_bpProcess->replaceFunction (oldFunc, newFunc))
		throw DiEx ("Cannot replace function call");
}

// Continue execution of the process
void DiProcess::ContinueExecution ()
{
	Syslog::Debug ("[DiProcess] Continue execution...");
	if (!_bpProcess->continueExecution ())
		throw "Continue execution failed";
}

// Wait for termination of the process
void DiProcess::WaitFor ()
{
	BPatch & bp = DynInst::Instance ();
    while (!_bpProcess->isTerminated ())
		bp.waitForStatusChange ();
}

void DiProcess::Test ()
{
	std::cerr << "Test Process" << std::endl; 
	BPatch & bp = DynInst::Instance ();
	for (int i=0; i<3; i++)
	{
		std::cerr << "POOL " << i << std::endl; 
		if (!_bpProcess->isTerminated ())
			bp.waitForStatusChange ();
	}
}

// Wait for process to stop
void DiProcess::WaitForStop ()
{
	BPatch & bp = DynInst::Instance ();
	while (!_bpProcess->isStopped () && !_bpProcess->isTerminated ())
	{
		bp.waitForStatusChange ();
	}

	if (!_bpProcess->isStopped ())
		throw "Process is terminated";
}

void DiProcess::loadLibrary (char * libName)
{
	Syslog::Debug ("load library");
	if (_bpProcess == NULL)
		throw DiEx ("Process null in loadLibrary");
	if (!_bpProcess->loadLibrary (libName))
	{
		Syslog::Debug ("Cannot load dynamic library...");
		throw "Cannot load dynamic library";
	}
	Syslog::Debug ("library loaded");
}

void DiProcess::GetLineNumber (unsigned long addr, unsigned short &line, char * fileName,
							int length)
{
	BPatch_Vector< BPatch_statement > lines;
	if (_bpProcess->getSourceLines (addr, lines)){
		line = lines[0].lineNumber();
		for (int i = 0; i < length; i++){
			fileName[i] = lines[0].fileName()[i];
		}
	} else {
		throw DiEx ("Could not get information about the line number from process");
	}
}

//---------------------------------------------------------------------------
// Read the program's image and get an associated image object
DiImage::DiImage (BPatch_process & bpProcess)
{
	_bpImage = bpProcess.getImage ();
	if (_bpImage == NULL)
		throw DiEx ("Get program image failed");
}

BPatch_variableExpr * DiImage::FindVariable (const char * name)
{
	BPatch_variableExpr * tmpVar = _bpImage->findVariable (name);
	if (tmpVar == NULL)
	{
		Syslog::Debug ("Finding variable -%s- failed", name);
		throw DiEx ("Finding variable failed");
	}
	return tmpVar;
}

//---------------------------------------------------------------------------
// Find the given location in the procedure/function with a given name
DiPoint::DiPoint (BPatch_image & bpImage, string const & procName, 
		     BPatch_procedureLocation loc)
{
	DiFunction f (bpImage, procName);
	PointVector *points = f.FindPoint (loc);
	if (points == NULL || points->size () < 1) {
		char msg[512];
		sprintf(msg,"Procedure point not found (%d points)", points->size ()); 
		throw DiEx (msg, procName);
	}
	_bpPoints = points;
}

void DiPoint::GetCalledFuncName (char * buf, int size)
{
	BPatch_function * func = (*_bpPoints)[0]->getCalledFunction ();
	if (func == NULL)
		throw DiEx ("Could not get called function.");
	func->getName (buf, size);
}

unsigned long DiPoint::GetAddress ()
{
	return (unsigned long)(*_bpPoints)[0]->getAddress ();
}

//---------------------------------------------------------------------------
// Find the given function with a given name
DiFunction::DiFunction (BPatch_image & bpImage, string const & funcName)
{
	FuncVector fv;
	bpImage.findFunction (funcName.c_str (), fv);
	if (fv.size () == 0)
		throw DiEx ("Function not found", funcName);
	else if (fv.size () != 1)
	{
		char buf [128];
		Dump (fv);		
		sprintf (buf, "%d function implementations found", fv.size ());
		throw DiEx (buf, funcName);
	}
	_bpFunc = fv [0];
}

void DiFunction::GetLineNumber (unsigned int &start, 
							unsigned int &end, 
							char * fileName, 
							unsigned int &max)
{
	if (_bpFunc->getName (fileName, max)){
		max = (long int)_bpFunc->getName (fileName, max);
	} else {
		throw DiEx ("Could not get information about the line number");
	}
}

unsigned long DiFunction::GetAddress ()
{
	char buf [256];
	_bpFunc->getName (buf, 256);
	return (unsigned long)_bpFunc->getBaseAddr ();
}

/*unsigned int DiFunction::GetSize ()
{
	return _bpFunc->getSize ();
}*/

char const * DiFunction::GetParams ()
{
	LocalVarVector * variables = _bpFunc->getParams ();
	int count = variables->size ();
	if (variables == NULL || variables->size () < 1)
		throw DiEx ("There are no parameters");
	char const * name = (*variables) [0]->getName ();
	return name;
}

DiFunction::PointVector * DiFunction::FindPoint (BPatch_procedureLocation loc)
{
	PointVector * points = _bpFunc->findPoint (loc);
	if (points == NULL)
		throw DiEx ("Procedure point for the given function not found");		
	return points;
}

void DiFunction::GetName (char * fileName, int len)
{
	_bpFunc->getName (fileName, len);
}

// Utilities

void DiFunction::Dump (DiFunction::FuncVector & fv)
{
	char buf [256] , buf1[256], buf2 [256];
	
	for (int i=0; i<fv.size (); ++i)
	{
		BPatch_function * f = fv [i];
		Syslog::Debug ("Function %s, %s, module %s", f->getName(buf, sizeof (buf)),
			f->getMangledName(buf1, sizeof (buf1)),
			f->getModuleName (buf2, sizeof (buf2))
			);
	}
}
