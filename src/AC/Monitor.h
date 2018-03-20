
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

#ifndef MONITOR_H
#define MONITOR_H

//----------------------------------------------------------------------
//
// Monitor.h
// 
// Monitor - manages task instrumentation
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#include "Tasks.h"
#include "PTPMsg.h"
#include "Config.h"
#include "auto_vector.h"
#include <memory>
#include <assert.h>

/**
 * @class Monitor
 * @brief Adds request to add or remove instrumentation in/from the tasks
 * that it is monitoring.
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class Monitor
{
public:
	/**
	 * @brief Constructor
	 * @param tasks Collection of tasks susceptible to be modified.
	 */
	Monitor (TaskCollection & tasks)
		: _tasks (tasks)
	{}

	/**
	 * @brief Adds the instructions requested to the task they belong to.
	 * @param instReq Object that represents the request for instrumentation
	 * to be added to a task.
	 */
	void AddInstr (AddInstrRequest & instrReq);

	/**
	 * @brief Removes the instructions requested from the selected task.
	 * @param instReq Object that represents the request for instrumentation
	 * to be removed from a task.
	 */
	void RemoveInstr (RemoveInstrRequest & instrReq);
private:
	TaskCollection & _tasks;
};

#endif

