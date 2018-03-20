
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

#ifndef INSTRSET_H
#define INSTRSET_H

//----------------------------------------------------------------------
//
// InstrSet.h
// 
// Instrumentation set - class that represents a filter of program
//						instrumentation
//		   (current set of functions that are instrumented in a program)
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

// note: get rid of warning C4786: identifier was truncated 
// to '255' characters...
#pragma warning (disable: 4786)

#include <vector>
#include <string>
#include <assert.h>
#include "Utils.h"
#include "di.h"

/**
 * @class SnippetHandler
 * @brief Contains he necessary fields to manage snippets.
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class SnippetHandler
{
public:

	/**
	 * @brief Constructor.
	 *
	 * @param eventId Unique identifier for the event.
	 * @param funcName Name of the function in which the snippet will be
	 * inserted.
	 * @param place Position in which the instrumentation will be added.
	 * @param handle Handle for the Dyninst snippet.
	 */
	SnippetHandler (int eventId, std::string const & funcName, InstrPlace place, 
						BPatchSnippetHandle * handle)
		: _eventId (eventId), _funcName (funcName), _place (place), _handle (handle)
	{}
	
	/**
	 * @brief Getter for the variable _eventId.
	 * @return Id of the event.
	 */
	int GetEventId () const
	{
		return _eventId;
	}

	/**
	 * @brief Getter for the variable _funcName.
	 * @return Name of the function in which the snippet will be inserted.
	 */
	std::string const & GetFuncName () const
	{
		return _funcName;
	}

	/**
	 * @brief Getter for the variable _place.
	 * @return Position of the function in which the snippet will be inserted.
	 */
	InstrPlace GetInstrPlace () const
	{
		return _place;
	}

	/**
	 * @brief Getter for the variable _handle.
	 * @return Handle of the snippet to be inserted.
	 */
	BPatchSnippetHandle * GetHandle () const
	{
		return _handle;
	}
	
private:
	int 					_eventId; // function id
	std::string const		_funcName;
	InstrPlace				_place;
	BPatchSnippetHandle * 	_handle;
};

//----------------------------------------------------------------------
// group of instrumentation snippets inserted into one function (the same funcId)

/**
 * @class InstrGroup
 * @brief Contains a group of snippets to be inserted in a function.
 *
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class InstrGroup
{
	typedef vector<SnippetHandler *> SnippetHandlers;
public:	
	typedef vector<SnippetHandler *>::iterator Iterator;
	
	/**
	 * @brief Constructor
	 */
	InstrGroup (int eventId, std::string const & funcName) 
		: _eventId (eventId), _funcName (funcName) 
	{}	

	/**
	 * @brief Destructor
	 */
	~InstrGroup ();

	/**
	 * @brief Getter for the variable _eventId.
	 * @return Id of the event.
	 */
	int GetEventId () const { return _eventId; }

	/**
	 * @brief Getter of the size of _vector.
	 * @return Size of the vector _vector.
	 */
	int GetSize () const { return _vector.size (); }

	/**
	 * @brief Checks if _vector is empty.
	 * @return 0 if not empty, 1 if empty.
	 */
	bool IsEmpty () const { return _vector.size () == 0; }

	/**
	 * @brief Getter of the name of the function.
	 * @return String that contains the name of the function.
	 */
	std::string const & GetFuncName () const { return _funcName; }
	
	/**
	 * @brief Add the handler passed as a parameter to the _vector.
	 *
	 * @param place Object that represents the place in the program in which
	 * the snippet will be inserted.
	 *
	 * @param handle Object of the class BPatchSnippetHandle that handles
	 * a Dyninst snippet.
	 */
	void AddHandler (InstrPlace place, BPatchSnippetHandle * handle);

	/**
	 * @brief Eliminates the handlers from the vector to be inserted in the
	 * place passed as a parameter.
	 *
	 * @param place Object that represents the place in the program in which
	 * the snippet will be inserted.
	 */
	void RemoveHandler (InstrPlace place);

	/**
	 * @brief Getter for an iterator pointing to the first element in the InstrGroup.
	 * @return Iterator for the variable _vector that points to its beginning.
	 */
	Iterator begin () { return _vector.begin (); }

	/**
	 * @brief Getter for an iterator pointing to the last instruction (handler) on the group.
	 * @return Iterator for the variable _vector that points to its final element.
	 */
	Iterator end () { return _vector.end (); }
	
private:
	int const			_eventId;
	std::string const 	_funcName;
	SnippetHandlers 	_vector;
};

#endif


