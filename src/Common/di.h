
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
// di.h
//
// Encapsulation of DynInst classes
//
// UAB, 2000
//
//----------------------------------------------------------------------

#if !defined DI_H
#define DI_H

//PAOLA:08.06.04
#include <dlfcn.h>
#include <BPatch.h>
#include <BPatch_statement.h>
#include <BPatch_process.h>
#include <BPatch_function.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <assert.h>

#include <BPatch_point.h>

using namespace std;

/**
 * @class 	DiEx
 * @brief	Implements the Dyninst's Exceptions
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */
class DiEx
{
public:

	/**
	 * @brief Constructor
	 *
	 * @param msg Exception message to display
	 * @param objName Object Name
	 */
	DiEx (string const & msg, string const & objName = string ())
			: _msg (msg), _objName (objName)
	{}

	/**
	 * @brief Exception message getter
	 * @return _msg
	 */
	string const & GetMessage () const { return _msg; }

	/**
     * @brief Object Name getter
     * @return _objName
     */
	string const & GetObjectName () const { return _objName; }

private:
	string _msg;
	string _objName;
};

typedef BPatch_Vector<BPatch_point*> PointVector;

/**
 * @class  DynInst
 * @brief	Assigns an instance of the class BPatch from Dyninst.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */
class DynInst
{
public:
	static BPatch & Instance ()
    {
    	if (!_registered)
    	{
    	  	_bp.registerErrorCallback (&OnError);
    	  	_registered = true;
    	}
	    return _bp;
	}

    static BPatch & Instance (BPatch in_bp)
    {
    	if (!_registered)
    	{
    		_bp=in_bp;
    	  	_bp.registerErrorCallback (&OnError);
    	  	_registered = true;
    	}
	    return _bp;
	}

protected:
	// prevent from creating an instance
	DynInst ()
	{}

	static void OnError (BPatchErrorLevel severity,
				    	 int number,
				    	 const char* const* params);

private:
	static BPatch _bp;
	static bool	  _registered;
};

//---------------------------------------------------------------------------
/*
class DiThread
{
public:
	DiThread () {}
	// Attach to the program (to a running thread)
	DiThread (char * mutateeName, int pid);

	// Create the program with given arguments
	// argv cannot be null
	DiThread (char * mutateeName, char * argv[], char * envp[] = 0);

	// Create the program
	DiThread (char * mutateeName);

	~DiThread ();

	operator BPatch_thread & () 
	{
		return *_bpThread;
	}

	int GetPid ()
	{
	   return _bpThread->getPid ();
	}  

	bool IsStopped () 
	{
	   assert (_bpThread != 0); 
	   return _bpThread->isStopped (); 
	}
 	BPatchSnippetHandle * InsertSnippet (BPatch_snippet const & expr, BPatch_point & point);
 	
 	BPatchSnippetHandle * InsertSnippet (BPatch_snippet const & expr, BPatch_point & point, 
 			BPatch_callWhen when, BPatch_snippetOrder order);
  	BPatchSnippetHandle * InsertSnippetBefore (BPatch_snippet const & expr, BPatch_point & point);
  	BPatchSnippetHandle * InsertSnippetAfter (BPatch_snippet const & expr, BPatch_point & point);


 	BPatchSnippetHandle * InsertSnippet (BPatch_snippet const & expr, PointVector & points);
 	
 	BPatchSnippetHandle * InsertSnippet (BPatch_snippet const & expr, PointVector & points, 
 			BPatch_callWhen when, BPatch_snippetOrder order);
  	BPatchSnippetHandle * InsertSnippetBefore (BPatch_snippet const & expr, PointVector & points);
  	BPatchSnippetHandle * InsertSnippetAfter (BPatch_snippet const & expr, PointVector & points);

  	
  	void DeleteSnippet (BPatchSnippetHandle * handle);
	// Execute given snippet once
	void OneTimeCode (BPatch_snippet const & expr);
	void ReplaceFunction (BPatch_function & oldFunc, BPatch_function & newFunc);

	void ContinueExecution ();
	bool StopExecution () { return _bpThread->stopExecution (); }
	void WaitFor ();
	void Test ();
	bool Terminate () { return _bpThread->terminateExecution (); }
	bool IsTerminated () { return _bpThread->isTerminated (); }

	// Wait for thread to stop
	void WaitForStop ();
	void loadLibrary (char * libName);
	void GetLineNumber (unsigned long addr, 
						unsigned short &line, 
						char * fileName, 
						int length);

	BPatch_variableExpr * Malloc (BPatch_type & type)
	{
		BPatch_variableExpr * var = _bpThread->malloc (type);
		if (var == NULL)
			throw "Cannot allocate a new variable";
	}

private:
	BPatch_thread* _bpThread;	// The BPatch_thread class operates on code in execution
			     		// This class can be used to manipulate the thread
};*/

//---------------------------------------------------------------------------

/**
 * @class 	DiProcess
 * @brief   Operates on code in execution.
 * This class can be used to manipulate the process
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */
class DiProcess
{
public:

	/**
	 * @deprecated
	 */
    DiProcess () {}

    /**
	 * @brief Attaches the program to a running process
     *
	 * @param mutateeName Name of the mutatee
	 * @param pid PID to give to the process
	 */
	DiProcess (char * mutateeName, int pid);

	/**
	 * @brief Creates the program with the given arguments
	 *
	 * @param mutateeName Name of the mutatee
	 * @param argv Arguments to give to the program. This cannot be null
	 * @param envp Environment list
	 */
	DiProcess (char * mutateeName, char * argv[], char * envp[] = 0);

	/**
	 * @brief Creates the program (mutatee)
	 * @param mutateeName Name of the mutatee
	 */
    DiProcess (char * mutateeName);

    /**
     * @brief Destructor
     */
	~DiProcess ();

	operator BPatch_process & ()
	{
		return *_bpProcess;
	}

    /**
     * @brief Getter of the Pid
     * @return Pid
     */
	int GetPid ()
	{
	   return _bpProcess->getPid ();
	}  

    /**
     * @brief Asserts that _bpProcess has been created and returns a boolean
     * @return bool
     */
	bool IsStopped () 
	{
	   assert (_bpProcess != 0); 
	   return _bpProcess->isStopped (); 
	}

    /**
     * @brief Inserts a given snippet into the given point
     *
     * @param expr Snippet
     * @param point Point where the Snippet will be inserted
     *
     * @return Handle
     */
 	BPatchSnippetHandle * InsertSnippet (BPatch_snippet const & expr, BPatch_point & point);

    /**
     * @brief Inserts the snippet into the point in a given order
     *
     * @param expr Snippet
     * @param point Point where the Snippet will be inserted
     * @param when
     * @param order
     *
     * @return Handle
     */
 	BPatchSnippetHandle * InsertSnippet (BPatch_snippet const & expr, BPatch_point & point, 
 			BPatch_callWhen when, BPatch_snippetOrder order);
  	/**
  	 * @brief Inserts a given snippet before the given point
  	 *
  	 * @param expr Snippet
  	 * @param point Point where the Snippet will be inserted
  	 *
  	 * @return Handle
  	 */
    BPatchSnippetHandle * InsertSnippetBefore (BPatch_snippet const & expr, BPatch_point & point);

    /**
     * @brief Inserts a given snippet after the given point
     *
     * @param expr Snippet
     * @param point Point where the Snippet will be inserted
     *
     * @return Handle
     */
    BPatchSnippetHandle * InsertSnippetAfter (BPatch_snippet const & expr, BPatch_point & point);

    /**
     * @brief Inserts the given snippet in the given points
     *
     * @param expr Snippet
     * @param points Vector of points
     *
     * @return Handle
     */
 	BPatchSnippetHandle * InsertSnippet (BPatch_snippet const & expr, PointVector & points);

    /**
     * @brief Inserts the given snippet in the series of points with the given order and moment
     *
     * @param expr Snippet
     * @param points Vector of points
     * @param when
     * @param order
     *
     * @return Handle
     */
 	BPatchSnippetHandle * InsertSnippet (BPatch_snippet const & expr, PointVector & points, 
 			BPatch_callWhen when, BPatch_snippetOrder order);

    /**
     * @brief Inserts the snippet before the given points
     *
     * @param expr Snippet
     * @param points Vector of points
     *
     * @return Handle
     */
    BPatchSnippetHandle * InsertSnippetBefore (BPatch_snippet const & expr, PointVector & points);

    /**
     * @brief Inserts the snippet after the given points
     *
     * @param expr Snippet
     * @param points Vector of points
     *
     * @return Handle
     */
    BPatchSnippetHandle * InsertSnippetAfter (BPatch_snippet const & expr, PointVector & points);

	/**
	 * @brief Deletes the snippet in the given handle
	 * @param handle
	 */
  	void DeleteSnippet (BPatchSnippetHandle * handle);

	/**
	 * @brief Executes the given snippet once
	 * @param expr Snippet
	 */
	void OneTimeCode (BPatch_snippet const & expr);

	/**
	 * @brief Replaces a function call with a call to another function
	 *
	 * @param oldFunc
	 * @param newFunc
	 */
	void ReplaceFunction (BPatch_function & oldFunc, BPatch_function & newFunc);

	/**
	 * @brief Resumes the execution of mutatee process
	 */
	void ContinueExecution ();

	/**
	 * @brief Stops the execution of the mutatee process
	 * @return bool
	 */
	bool StopExecution () { return _bpProcess->stopExecution (); }

	/**
	 * @brief Waits for the termination of the process
	 */
	void WaitFor ();

	/**
	 * @brief Tests the current mutatee process by waiting for a status change
	 */
	void Test ();

	/**
	 * @brief Terminates the mutatee process
	 * @return bool
	 */
	bool Terminate () { return _bpProcess->terminateExecution (); }

	/**
	 * @brief Returns true if the mutatee process is terminated
	 * @return bool
	 */
	bool IsTerminated () { return _bpProcess->isTerminated (); }

	// Wait for thread to stop
	/**
	 * @brief Waits for the mutatee process to stop
	 */
	void WaitForStop ();

	/**
	 * @brief Loads a shared library into the mutatee's address space. Returns true if successful
	 * @param libName Library name
	 */
	void loadLibrary (char * libName);

	/**
	 * @brief Gets information about the given line number from the mutatee process
	 *
	 * @param addr
	 * @param line
	 * @param fileName
	 * @param length
	 */
	void GetLineNumber (unsigned long addr, unsigned short &line, char * fileName,
						int length);


	/**
	 * @brief Allocates a new variable of the given type
	 * @param type
	 */
	BPatch_variableExpr * Malloc (BPatch_type & type)
	{
		BPatch_variableExpr * var = _bpProcess->malloc (type);
		if (var == NULL)
			throw "Cannot allocate a new variable";
	}

private:
	BPatch_process* _bpProcess;	// The BPatch_process class operates on code in execution
			     		// This class can be used to manipulate the process
};

//------------------------------------------------------------------------------
/**
 * @class 	DiImage
 * @brief   Reads the program's image and gets an associated image object (the executable associated with a thread).
 * It can also find a variable in the image and return it.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */
class DiImage
{
public:
	// Read the program's image and get an associated image object
	DiImage () {}
	DiImage (BPatch_process & bpProcess);

    /**
     * @brief Finds the variable from _bpImage via name and returns it
     * @param name Name of the variable
     * @return Global variable matching <name> in the image.  NULL if not found.
     */
	BPatch_variableExpr * FindVariable (const char * name);

	operator BPatch_image & ()
	{
		return *_bpImage;
	}

private:
	BPatch_image* _bpImage;	// This class defines a program image 
							// (the executable associated with a thread)
};


//------------------------------------------------------------------------------

/**
 * @class DiPoint
 * @brief Dyninst's point class. An object of this class represents a location
 * in an application's code at which DynInst can insert instrumentation.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 1.0b
 */
class DiPoint
{	
public:
	// Find the given location in the procedure/function with a given name
	/**
	 * @brief Finds the given location in the function with a given name
	 *
	 * @param bpImage BPatch Image of the program
	 * @param procName Process Name
	 * @param loc Location
	 */
	DiPoint (BPatch_image & bpImage, string const & procName,
			     BPatch_procedureLocation loc = BPatch_entry);

    /**
     * @brief Getter of the current function name
     *
     * @param buf Returned name
     * @param size Max length of the buffer
     */
	void GetCalledFuncName (char * buf, int size);

    /**
     * @brief Getter of the current address
     * @return address
     */
    unsigned long GetAddress ();

	operator PointVector & ()
	{
		return *_bpPoints;
	}

    /**
     * @brief _bpPoints getter
     * @return _bpPoints
     */
	PointVector &getPoints() { return *_bpPoints; }

private:
	PointVector *_bpPoints;	// An object of this class represents a location 
							// in an application's code at which DynInst 
							// can insert instrumentation
};

//------------------------------------------------------------------------------

/**
 * @class DiFunction
 * @brief Dyninst's function class. It represents a function in the application
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */
class DiFunction
{
	typedef BPatch_Vector<BPatch_function*> FuncVector;
	typedef BPatch_Vector<BPatch_localVar *> LocalVarVector;
	typedef BPatch_Vector<BPatch_point *> PointVector;

public:
	// Find the given function with a given name
	DiFunction (BPatch_image & bpImage, string const & funcName);
	
	/*void GetLineNumber (unsigned short &start, 
						unsigned short &end, 
						char * fileName, 
						unsigned int &max);*/

	/**
	 * @brief Gets the current line number and the file name
	 *
	 * @param start Address
	 * @param end Line
	 * @param fileName String where the file name will be saved
	 * @param max Length
	 */
    void GetLineNumber (unsigned int &start, unsigned int &end, char * fileName,
                        unsigned int &max);

    /**
     * @brief Reads the address of _bpVar
     * @return Address of _bpVar
     */
	unsigned long GetAddress ();

    //unsigned int GetSize ();

    /**
     * @brief Gets the parameters of _bpFunc
     * @return Parameters of the current function
     */
	char const * GetParams ();

    /**
     * @brief Finds the procedure point for the given location
     * @param loc Location of the point to look for
     * @return Point Vector of the specified location
     */
    PointVector * FindPoint (BPatch_procedureLocation loc = BPatch_subroutine);

    /**
     * @brief Getter of the file name
     *
     * @param fileName Parameter where the file name will be stored
     * @param len max length of the file name
     */
    void GetName (char * fileName, int len);

 	operator BPatch_function & ()
	{
		return *_bpFunc;
	}

    /**
     * @brief Prints all functions from the FuncVector _fv_
     * @param fv FuncVector
     */
	static void Dump (FuncVector & fv);

private:
	BPatch_function* _bpFunc;	// An object of this class represents a function
								// in the application
};

//------------------------------------------------------------------------------

/**
 * @class DiType
 * @brief Dyninst's Type class.
 * It represents a variable or area of memory in a thread's address space.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 1.0b
 */
class DiType
{
public:
    /**
     * @brief Constructor
     * @param bpImage Image of the program
     * @param typeName Name of the type
     */
	DiType (BPatch_image & bpImage, char const * typeName) 
	{
		_bpType = bpImage.findType (typeName);
		if (_bpType == NULL)
			 throw DiEx ("Type not found", typeName);
	}

	operator BPatch_type & ()
	{
		return *_bpType;
	}

private:
	BPatch_type* _bpType;	// It represents a variable or area of memory 
							// in a thread's address space
};

//------------------------------------------------------------------------------

/**
 * @class DiIntType
 * @brief Dyninst Int type class
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */
class DiIntType: public DiType
{
public:
	DiIntType (BPatch_image & bpImage)
		: DiType (bpImage, "int")
	{}
};

// -----------------------------------------------------------------------------


/**
 * @class DiSnippetHandle
 * @brief Dyninst's snippet handler class
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 1.0b
 */
class DiSnippetHandle
{
public:
    /**
     * @brief Constructor
     * @param bpProcess BPatch process
     * @param snippet Snipped of code
     * @param point Point in the snippet
     * @param needDelete Flag that states if the snipped has to be deleted. False by default
     */
    DiSnippetHandle (BPatch_process & bpProcess, 
					 BPatch_snippet & snippet, 
					 BPatch_point & point,
					 bool needDelete = false)
	  : _needDelete (needDelete), _bpProcess (bpProcess)
    {
	   _handle = _bpProcess.insertSnippet (snippet, point);
	   if (_handle == NULL)
		   throw DiEx ("Insert snippet failed");
    }

    /**
     * @brief Destructor
     */
    ~DiSnippetHandle ()
    {
	  if (_needDelete)
      {
		 _bpProcess.deleteSnippet (_handle);
      }
    }

private:
	BPatchSnippetHandle* _handle;
	bool                 _needDelete;
	BPatch_process      & _bpProcess;
};


// -------------- TEMPORARY ------------

/**
 * @class DiVariable
 * @brief Deals with Dyninst's Variable class.
 * This can create, read and delete a variable of a given type or size in memory.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 1.0b
 */
class DiVariable
{
public:
	// variable expression snippet, variable is of a given type
	/**
	 * @brief DiVariable constructor of a given type
	 *
	 * @param bpProcess Process
	 * @param type Type of the variable
	 */
	DiVariable (BPatch_process& bpProcess, BPatch_type const & type)
		: _bpProcess (bpProcess)
	{
		_bpVar = _bpProcess.malloc (type);
		if (_bpVar == NULL)
			throw "Cannot allocate a new variable";
	}

	// variable expression snippet, variable is of a given type name
	/**
	 * @brief DiVariable constructor of a given type name
	 *
	 * @param bpProcess Process
	 * @param typeName Type name
	 */
	DiVariable (BPatch_process& bpProcess, char const * typeName)
		: _bpProcess (bpProcess)
	{
		DiImage image (bpProcess);
		DiType t (image, typeName);
		_bpVar = _bpProcess.malloc (t);
		if (_bpVar == NULL)
			throw "Cannot allocate a new variable";
	}

	// variable expression snippet, allocate size bytes
	/**
	 * @brief DiVariable constructor of a given size
	 * @param bpProcess Process
	 * @param size Size in bytes
	 */
	DiVariable (BPatch_process& bpProcess, int size)
		: _bpProcess (bpProcess)
	{
		_bpVar = _bpProcess.malloc (size);
		if (_bpVar == NULL)
			throw "Cannot allocate a new variable";
	}

	/**
	 * @brief Destructor
	 */
	~DiVariable ()
	{
		if (_bpVar != NULL)
			_bpProcess.free (*_bpVar);
	}

	operator BPatch_variableExpr& ()
	{
		return *_bpVar;
	}

	operator BPatch_variableExpr* ()
	{
		return _bpVar;
	}

    // reads the value of a variable

    /**
     * @brief Reads the value of a variable in a thread's address space.
     * <dst> is assumed to be the same size as the variable.
     * @param dst Reads value from here
     */
    void GetValue (void *dst) const
	{
		_bpVar->readValue (dst);
	}

	// read an address of variable
	/**
	 * @brief Returns base address of this variable in the target's address space
	 * @return Base address of the variable
	 */
    long int GetAddress () const
	{ 
		return (long int)_bpVar->getBaseAddr (); 
	}

private:
	BPatch_process		  & _bpProcess;
	BPatch_variableExpr * _bpVar;
};


//-----------------------------------------------------------------------------------

/**
 * @class DiIntVariable
 * @brief Dyninst's int variable class.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 1.0b
 */
class DiIntVariable: public DiVariable
{
public:
	// variable of the int type
	DiIntVariable (BPatch_process& bpProcess)
		: DiVariable (bpProcess, "int")
	{}
};

//-----------------------------------------------------------------------------------

/**
 * @class ProcedureList
 * @brief Implements and handles a vector of BPatch_functions.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 1.0b
 */
class ProcedureList
{
	typedef BPatch_Vector<BPatch_function*> FuncVector;

public:

    /**
     * @brief Constructor
     * @param bpImage Image of the program
     */
	ProcedureList (BPatch_image& bpImage)
	{
		_vector = bpImage.getProcedures();
		if (_vector == NULL)
			throw "Cannot get procedure list from program image";
	}

    /**
     * @brief Getter of the vector's size
     * @return Vector's size
     */
	int GetSize	() const
	{
		return _vector->size ();
	}

	BPatch_function & operator[] (int i) const
	{
		return *(*_vector)[i];		 
	}

private:
	FuncVector* _vector;
};

//-----------------------------------------------------------------------------------

/**
 * @class ModuleList
 * @brief Class that stores and handles a vector of BPatch_modules.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */
class ModuleList
{
	typedef BPatch_Vector<BPatch_module*> ModuleVector;

public:

	/**
	 * @brief Constructor
	 * @param bpImage Program image
	 */
	ModuleList (BPatch_image& bpImage)
	{
		_vector = bpImage.getModules();
		if (_vector == NULL)
			throw "Cannot get module list from program image";
	}

	/**
	 * @brief Getter of the vector size
	 * @return _vector->size
	 */
	int GetSize	() const
	{
		return _vector->size ();
	}

	BPatch_module & operator[] (int i) const
	{
		return *(*_vector)[i];		 
	}

private:
	ModuleVector* _vector;
};

//-----------------------------------------------------------------------------------

/**
 * @class PointList
 * @brief Class that stores a vector of BPatch_points and handles it.
 * Can also get the address and function names of a given point.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */
class PointList
{
	typedef BPatch_Vector<BPatch_point*> PointVector;

public:

    /**
     * @brief Constructor
     * @param func
     */
	PointList (DiFunction & func)
	{
		_vector = func.FindPoint ();
	}

	/**
	 * @brief Getter of the vector size
	 * @return Vector's size
	 */
	int GetSize	() const
	{
		return _vector->size ();
	}

	/**
	 * @brief Gets the name of the function called at the given point
	 *
	 * @param point Point where the function is
	 * @param name Name of the function
	 * @param length Length of the name
	 */
	void GetCalledFuncName (BPatch_point & point, char * name, int length)
	{
		BPatch_function * func = point.getCalledFunction ();
		if (func == NULL)
			throw "The called point is not a function";
		func->getName (name, length);
	}
	
	/**
	 * @brief Gets the address of the given point
	 * @param point Given point to look get address from
	 * @return Address of the given point
	 */
	unsigned long GetAddress (BPatch_point & point)
	{
		unsigned long tmp = (unsigned long) point.getAddress ();
		return tmp;
	}

	BPatch_point & operator[] (int i) const
	{
		return *(*_vector)[i];		 
	}

private:
	PointVector* _vector;
};

#endif
