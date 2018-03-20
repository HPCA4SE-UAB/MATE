
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

#ifndef TASKINSTR_H
#define TASKINSTR_H

//----------------------------------------------------------------------
//
// TaskInstr.h
// 
// TaskInstr - manages task instrumentation (add/remove)
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#include "InstrSet.h"
#include "Utils.h"
//#include <BPatch_thread.h>
#include <string>
#include <map>


// Instrumentation of a task
/**
 * @class TaskInstr
 *
 * @brief Adds and remove instrumentation from the process in execution.
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class TaskInstr
{
	typedef std::map<int, InstrGroup *> Map;
	typedef std::pair<Map::iterator, bool> Pair;
	
public:
    /**
	 * @brief Constructor
	 */
	TaskInstr () : _brkptHandle (0) {}

	/**
	 * @brief Destructor
	 */
	~TaskInstr ();

	/**
	 * @brief Getter of the size of the variable _map.
	 *
	 * @return size of the map _map.
	 */
	int GetSize () const 
	{ 
		return _map.size (); 
	}

	/**
	 * @brief Adds a new snippet (handler) to the group in the map under the eventId key.
	 *
	 * If a group doesn't exist with that key one is created and added to the map.
	 *
	 * @param eventId Identifier of the event used as key to store the
	 * instrumentation groups in the map.
	 * @param functionName Name of the function to be modified.
	 * @param instrPlace Place of the function where the snippet will be inserted.
	 * @param handler Handle of the snippet to be inserted.
	 */
	void Add (int eventId, string const & functionName, InstrPlace instrPlace, 
				BPatchSnippetHandle * handler);

	/**
	 * @brief Eliminates all the snippets to be inserted on the same event and place.
	 *
	 * @param eventId Identifier of the event used as key to find and remove the
	 * instrumentation groups in the map.
	 * @param instrPlace Place of the function where the snippet would have been.
	 *
	 * @throws Remove cannot find instrumentation group.
	 */
	void Remove (int eventId, InstrPlace instrPlace);

	/**
	 * @brief Finds an instrumentation group for a given eventId.
	 *
	 * @param eventId Key to find the instrumentation group in the map.
	 *
	 * @return IntrGroup object that contains all the snippets to be added on a given event.
	 */
	InstrGroup * FindGroup (int eventId);

	/**
	 * @brief Setter of the variable _brkptHandle which represents a place in the process
	 * where it should be stopped.
	 */
	void SetBreakpoint (BPatchSnippetHandle * h) { _brkptHandle = h; }

	/**
	 * @brief Getter of the variable _brkptHandle which represents a place in the process
	 * where it should be stopped.
	 *
	 * @return Handle of the breakpoint.
	 */
	BPatchSnippetHandle * GetBreakpoint () { return _brkptHandle; }

private:
	Map	_map; // maps eventid to instr groups
	BPatchSnippetHandle * _brkptHandle;
};

#endif
