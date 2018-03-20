
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

#ifndef TASK_H
#define TASK_H

//----------------------------------------------------------------------
//
// Task.h
// 
// Task - represents an MPI task
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#include "di.h"
#include "TimeValue.h"
#include "TaskInstr.h"
#include "PTPMsg.h"
#include "cmdline.h"
#include <iostream>
#include <string>
#include <strstream>
#include <queue>

class Tuner;
//class TuningRequest;

/**
 * @class Task
 *
 * @brief Represents each of the processes that we can modify using Dyninst.
 *
 * Provides the necessary function to manage the process to be modified and
 * to control its execution during the modifications.
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class Task
{
	typedef std::deque<Common::TuningRequest *> TuningRequests;
	
public:
	/**
	 * @brief Constructor
	 *
	 * @param path Path to the executable of the application.
	 * @param args[] Array that contains the arguments with which the
	 * application should be executed.
	 * @param clockDiff Correction of the clock difference.
	 * @param anayzerHost Address of the host in which the analyzer is running.
	 * @param analyzerPort Port through which the analyzer communicates.
	 * @param debugLevel Selected level of debugging.
	 * @param debugStdErr Debug messages output.
	 * @param DMLibName Name of the dynamic library to be loaded into the process.
	 */
	Task (const std::string& path, char * args[], TimeValue const & clockDiff,
			string const & analyzerHost, int analyzerPort,
			int debugLevel, int debugStdErr, string const & DMLibName);

	/**
	 * @brief Destructor
	 */
	~Task ();
	
	/**
	 * @brief Getter of the identifier of the process being modified.
	 * @return Pid of the process being modified.
	 */
	int GetPid () { return _process.GetPid (); } 	// process identifier

	/**
	 * @brief Getter of the MPIRank attribute.
	 */
	int GetMpiRank ();

	/**
	 * @brief Restarts the execution of the process after breakpoint.
	 */
	void Continue () { _process.ContinueExecution (); }

	/**
	 * @brief Waits for the process to be terminated.
	 */
	void WaitFor () { _process.WaitFor (); }

	/**
	 * @brief Getter of the variable _process.
	 * @return Process that is being modified.
	 */
	DiProcess & GetProcess ()  { return _process; }

	/**
	 * @brief Getter of the variable _image.
	 * @return Image of the process that is being modified.
	 */
	DiImage & GetImage ()  { return _image; }

	/**
	 * @brief Getter of the variable _instr.
	 * @return Instrumentation to be inserted.
	 */
	TaskInstr & GetInstr ()  { return _instr; }

	/**
	 * @brief Checks if the process is running.
	 * @return False if its running, true if stopped.
	 */
	bool IsStopped () { return _process.IsStopped (); }

	/**
	 * @brief Terminates a running process and invokes the callback function if exists.
	 * @return True for success, false for failure.
	 */
	bool Terminate () { return _process.Terminate (); }

	/**
	 * @brief Checks if the process is terminated.
	 * @return True if the process has exited.
	 */
	bool IsTerminated () { return _process.IsTerminated (); }

	/**
	 * @brief Adds a tuning request to the pending list.
	 * @param req Request for tuning procedure.
	 */
	void AddDelayedTuning (Common::TuningRequest * req);

	/**
	 * @brief Checks if the process is stopped in a breakpoint.
	 * @return True if process is currently stopped and situated in a breakpoint .
	 */
	bool IsStoppedOnBreakpoint ()  { return _process.IsStopped () && _pendingTuningReqs.size () > 0; }

	/**
	 * @brief It is called when the process hits a breakpoint and needs to be handled.
	 * @param t Tuner that will apply the changes specified in the request for each task.
	 */
	void ProcessBreakpoint (Tuner & t);

	/**
	 * @brief Unload the DMLib from a process which has terminated.
	 */
	void UnloadLibrary ();
private:

	/**
	 * @brief Creates and inserts on the process a snippet to load the dynamic
	 * library into it and sets the clockdiff variable.
	 *
	 * @param clockDiff Correction of the clock difference.
	 * @param anayzerHost Address of the host in which the analyzer is running.
	 * @param analyzerPort Port through which the analyzer communicates.
	 * @param debugLevel Selected level of debugging.
	 * @param debugStdErr Debug messages output.
	 */
	void InitLibrary (string const & analyzerHost, int analyzerPort, TimeValue const & clockDiff, 
		int, int);

	
private:
	char		  * _path;		// task path
	DiProcess  		_process;	// represents mutatee process
	DiImage	 		_image;		// represents mutatee program image
	TaskInstr		_instr;		// task instrumentation
	TuningRequests	_pendingTuningReqs; // pending tuning requests (deferred by breakpoints)
};

#endif

