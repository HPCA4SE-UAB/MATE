
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
// Tuner.h
// 
// Tuner - manages task tuning
//
// Anna Sikora, UAB, 2000-2004
//
//----------------------------------------------------------------------

#ifndef TUNER_H
#define TUNER_H

#include "Tasks.h"
#include "PTPMsg.h"

/**
 * @class Tuner
 *
 * @brief Contains the tools necessary to handle the requests from the Analyzer.
 * Performs the different tuning jobs and handles breakpoints by delaying
 * the tuning until the target point is reached.
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */



class Tuner
{
public:

    /**
     * @brief Constructor
     * @param tasks
     */
	Tuner (TaskCollection & tasks) 
		: _tasks (tasks)
	{}

	/**
	 * @brief Handles the tuning requests received from the Analyzer.
	 *
	 * Applies the required changes in the target process. If there's a
	 * breakpoint for the request instead of processing the request it is
	 * pushed back to the pending requests list and a breakpoint is inserted
	 * and its handler is passed to the task.
	 *
	 * When the breakpoint in the process is reached its removed and the
	 * tuning is performed.
	 *
	 * After applying changes the request is deleted.
	 *
	 * @param req Tuning request to be applied on the target process.
	 */
	void Process (Common::TuningRequest * req);

	/**
	 * @brief Removes the breakpoint snippet inserted in the process.
	 *
	 * To be called when a breakpoint is processed and therefore has
	 * not further purpose.
	 *
	 * @param task Task to which executes the process.
	 */
	void RemoveLastBreakpoint (Task & task);
	
private:
	//Specific requests

	/**
	 * @brief Loads a dynamic library into a running process.
	 *
	 * @param tuningReq Object that contains the necessary info,
	 * like the path to the library.
	 */
	void LoadLibrary (LoadLibraryRequest & tuningReq);

	/**
	 * @brief Finds a variable in the running process and sets it
	 * to the desired value.
	 *
	 * note(8-2011): only implemented for variables of type float.
	 *
	 * @param tuningReq Object that contains the necessary info,
	 * like the name of the variable and the new value.
	 */
	void SetVariableValue (SetVariableValueRequest & tuningReq);

	/**
	 * @brief Replaces all calls to a function with calls to another one.
	 * @param tuningReq Object that contains the necessary info.
	 */
	void ReplaceFunction (ReplaceFunctionRequest & tuningReq);

	/**
	 * @brief Inserts a function call in a specified point in the process.
	 * @param tuningReq Object that contains the necessary info.
	 */
	void InsertFunctionCall (InsertFunctionCallRequest & tuningReq);

	/**
	 * @brief Inserts a function call to be executed immediately into a process.
	 * @param tuningReq Object that contains the necessary info.
	 */
	void OneTimeFuncCall (OneTimeFunctionCallRequest & tuningReq);

	/**
	 * @brief Not implemented.
	 */
	void RemoveFuncCall (RemoveFunctionCallRequest & tuningReq);

	/**
	 * @brief Not implemented (unfinished).
	 */
	void FuncParamChange (FunctionParamChangeRequest & tuningReq);

	/**
	 * @brief Generates a breakpoint into a specified location of a process
	 * and passes a handle to the task it belongs to.
	 *
	 * @param task Task in which the bp will be inserted.
	 * @param brkpt Breakpoint location info (i.e function and place of the bp).
	 */
	void InsertBreakpoint (Task & task, Breakpoint const & brkpt);

private:
	TaskCollection & _tasks;
};

#endif


