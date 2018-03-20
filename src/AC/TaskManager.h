
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
// TaskManager.h
// 
// TaskManager - manager (model) of mpi tasks
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#if !defined TASKMANAGER_H
#define TASKMANAGER_H

#include <iostream>
#include <cassert>
#include "Tasks.h"
#include "Config.h"
#include "TimeValue.h"
#include "Tuner.h"
#include "Monitor.h"
#include "di.h"

using namespace std;

/**
 * @class TaskExitHandler
 *
 * @brief Contains a virtual function to handle the exit of a task.
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class TaskExitHandler
{
public:
	/**
	 * @brief Installs a callback function that is called when the task
	 * terminates.
	 */
   virtual void HandleTaskExit (Task const & task, int exitCode) = 0;
};


// create only one instance of this class
/**
 * @class TaskManager
 *
 * @brief Single class that starts and handles all the tasks.
 *
 * Originally this class started the applications in each node using MPI, now this is done externally.
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class TaskManager
{
public:


	/**
	 * @brief Constructor
	 * @param cfg Configuration handler.
	 */
	TaskManager (Config & cfg);

	/**
	 * @brief Destructor
	 */
	~TaskManager ();

	/**
	 * @brief Executes the application passed as a parameter to the AC.
	 *
	 * Starts the task containing MPI code that will be monitored and tuned.
	 * Once started it'll be added to the tasks collection of running tasks.
	 *
	 * @param path Absolute location of the executable containing the app.
	 * @param args Arguments that should be passed to the application when it
	 * is started.
	 *
	 * @return True if the task has been successfully started, false otherwise.
	 */
	bool StartMPITask (const std::string& path, char ** args);

	/**
	 * @brief Creates a new task.
	 *
	 * Creates a new task that represents the execution of the program passed
	 * as a parameter and adds it to the list of stored tasks.
	 *
	 * @param path Absolute location of the executable containing the app.
	 * @param args Arguments that should be passed the application when it
	 * is started.
	 * @param envc Environment count. (Unused)
	 * @param env Environments. (Unused)
	 *
	 *  @return true if the task has been successfully started, false otherwise.
	 */
	bool StartTask (const std::string& path, char ** args, int envc, char ** env);

	/**
	 * @brief Getter for the variable _tasks.
	 *
	 * @return A collection contains all the stored tasks.
	 */
	TaskCollection & GetTaskCollection () { return _tasks; }

	/**
	 * @brief This function checks for changes in the process status
	 * and handles the checkpoint when they're reached.
	 */
	void HandleEvents ();
	
	/**
	 * @brief Sets the passed object as the handler for when the task exits.
	 *
	 * @param handler Object that represents a callback function to be
	 * executed when the task exits.
	 */
	void Register (TaskExitHandler & handler) { _handler = &handler; }

	/**
	 * @brief Checks if all the tasks have been finished.
	 * @return True if there are not tasks running after they've been started.
	 */
  	bool IsDone () const { return (_started && _tasks.GetCount () == 0); }

  	/**
  	 * @brief Setter of the values that represent the net address of the Analyzer.
  	 *
  	 * @param analyzerHost IP of the node in which the Analyzer is running.
  	 * @param collectorPort Port number being used by the analyzer.
  	 */
  	void SetAnalyzerHost (string const & analyzerHost, int collectorPort);

  	/**
  	 * @brief Sets the time difference between the host and the master.
  	 *
  	 * @param clockDiff Time values of the clock difference between
  	 * host and master.
  	 */
	void SetClockDiff (TimeValue const & clockDiff) { _clockDiff = clockDiff; }

	/**
	 * @brief Sets the values of the flags that control message output of the program.
	 *
	 * These values are extracted from the configuration file.
	 *
	 * @param level Value that determines which messages will be printed.
	 * @param showStdErr Value that determines if the error message should be printed.
	 */
	void SetTaskDebug (int level, int showStdErr)
	{
		_taskDebugLevel = 0;
		_taskDebugStdErr = showStdErr;
	}

	/**
	 * @brief Getter of the _monitor variable.
	 * @return A reference to the monitor object.
	 */
	Monitor & GetMonitor () { return _monitor; }

	/**
	 * @brief Getter of the _tuner variable.
	 * @return A reference to the tuner object.
	 */
	Tuner & GetTuner ()  { return _tuner; }
	
private:

	/**
	 * @brief Checks if any of the tasks stored is stopped on a breakpoint
	 * and if so processes it so the tuner can apply the necessary changes.
	 */
	void CheckBreakpointHit ();
	
	/**
	 * @brief Removes the task containing the process @code proc /endcode from the
	 * stored tasks collection and calls the exit handles of the task.
	 *
	 * @param proc Dyninst object that represents the process that exited.
	 * @param exitCode Code that the process reported on exit.
	 */
	void OnExitCallback (BPatch_thread * proc, int exitCode);

	/**
	 * @brief Handles the error information reported by Dyninst.
	 *
	 * @param severity Indicator of the severity of the problem.
	 * @param number Number code of the error.
	 * @param params Text that describes the error.
	 */
	void OnErrorCallback (BPatchErrorLevel severity, int number, const char* const* params);

	/**
	 * @brief Terminates the remaining processes and removes their tasks from the
	 * collection @code _tasks /endcode.
	 *
	 * When tasks are terminated this way their handleExit callback function is
	 * disabled.
	 */
	void CleanupTasks ();

	/**
	 * @brief Not implemented.
	 */
	void KillTask (int pid);

	/**
	* @brief Prints the information retrieved from the process that exited.
	*
	* This function is registered into the BPatch object as the callback
	* for exiting Threads and so it will be executed whenever one of them exits.
	*
	* @param proc Dyninst object that represents the thread that exited.
	* @param exitCode Code that describes the type of exit the process had.
	*/
	static void ThreadExitCallback (BPatch_thread * proc, BPatch_exitType exitCode);

	/**
	* @brief Adaptor for the function @code OnErrorCallback /endcode.
	*
	* This function is registered into the BPatch object as the callback
	* for Dyninst error and so it will be executed whenever one of them occurs.
	*
	* @param severity Indicator of the severity of the problem.
	* @param number Number code of the error.
	* @param params Text that describes the error.
	*/
	static void DynInstErrorCallback (BPatchErrorLevel severity, int number, const char* const* params);
	
private:
	TaskCollection    	_tasks;
	Monitor 		    _monitor;
	Tuner 				_tuner;
	TaskExitHandler * 	_handler;
	bool				_started;
	static TaskManager *_taskMngr;
	TimeValue 			_clockDiff; // time difference between this host and the master
	string 		 		_analyzerHost;
	int 				_analyzerPort;
	int					_taskDebugLevel;
	int					_taskDebugStdErr;
	bool 			  	_disableExitCallback;
	string				_DMLibName;
};	

#endif
