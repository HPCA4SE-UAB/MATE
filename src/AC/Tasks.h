
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
// Tasks.h
// 
// Tasks - task collection
//
// Anna Sikora, UAB, 2009
//
//----------------------------------------------------------------------

#if !defined TASKS_H
#define TASKS_H

#include <memory>
#include <cassert>
#include <cstdio>
#include "Exception.h"
#include "Task.h"
#include "auto_vector.h"
#include "di.h"

using std::auto_ptr;

/**
 * @class TaskCollection
 *
 * @brief Groups task in a single, easy to handle, collection.
 *
 * Provides collection methods to a group of tasks to facilitate the
 * handling of many tasks at once.
 */
class TaskCollection
{
public:
	enum { NotFound = -1 };

	/**
	 * @brief Constructor
	 */
	TaskCollection () : _count (0) {}

	/**
	 * @brief Adds a new task to the collection.
	 * @param task Pointer to the task to be added.
	 */
	void Add (auto_ptr<Task> & task)
	{
		_arr.push_back (task);
		++_count;
	}

	/**
	 * @brief Removes a task from the collection.
	 * @param index Position in the array of the task to be removed.
	 */
	void Delete (int index)
	{
		assert (_count > 0);
	   _arr.remove_direct (index, 0);
	   --_count;
   	}
	
	/**
	 * @brief Erases all the elements of the array.
	 */
	void Clear ()
	{
		_arr.clear ();
	}
	
	/**
	 * @brief Enables the use of the [] to select an element from the collection.
	 *
	 * @param index Position in the array.
	 *
	 * @return Constant value of a pointer to the selected task.
	 */
	Task const * operator[] (int index) const
	{
		return _arr [index];
	}
	
	/**
	 * @brief Enables the use of the [] to select an element from the collection.
	 *
	 * @param index Position in the array.
	 *
	 * @return Pointer to the selected task.
	 */
	Task * operator[] (int index)
	{
		return _arr [index];
	}

	/**
	 * @brief Finds task by its PID and returns its index.
	 *
	 * Checks if a task with the given PID is being executed by the AC.
	 * @param pid ID of the process which is executing the task.
	 *
	 * @return NotFound if a task with given PID is not stored in the collection
	 */
	int FindByPid (int pid)
	{
		auto_vector<Task>::iterator iter = _arr.begin ();
		int idx = 0;
		while (iter != _arr.end ())
		{
	    	Task * t = (*iter);
	    	if (t != 0)
	    	{  
	      		int taskPid = t->GetPid ();
	      		if (taskPid == pid)
					return idx;
	    	}  
	    	iter++;
	    	idx++;
	 	}
		return NotFound;
	}  
	
	/**
	 * @brief Getter of the number of tasks contained.
	 * @return number of stored tasks
	 */
	int GetCount () const
	{
		return _count;
	}

	/**
	 * @brief Looks for a task among the ones stored and returns its
	 * reference if found.
	 *
	 * @param pid ID of the process executing the desired task.
	 *
	 * @return The task if it's found.
	 *
	 * @throws "Task not found" exception if there is no task with the
	 * required PID.
	 */
	Task & GetByPid (int pid)
	{
		int taskIdx = FindByPid (pid);
		if (taskIdx == NotFound)
		{
			// task doesn't exist or its already finished
			char buf [16];
			sprintf (buf, "Pid=%d", pid);
			throw Exception ("Task not found", buf);
		}
		Task * t = _arr [taskIdx];
		assert (t != 0);
		return *t;
	}	

private:
	auto_vector<Task> 	_arr; 
	int					_count;
};

#endif
