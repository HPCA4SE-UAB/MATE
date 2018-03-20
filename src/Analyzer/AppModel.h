
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
// AppModel.h
// 
// Abstract Application Model Classes
//
// Anna Sikora, UAB, 2004
//
//----------------------------------------------------------------------

#if !defined APPMODEL_H
#define APPMODEL_H

#include "auto_vector.h"
#include "Utils.h"
#include "sync.h"
#include "EventCollector.h"
#include "Syslog.h"
#include "Queue.h"
#include "ECPMsg.h"
#include "AppEvent.h"
#include "AppTask.h"
#include "ACProxy.h"
#include <vector>
#include <map>
#include <string>

namespace Model
{
	/**
	 * @class Application
	 * @brief Represents tuned application in the analyzer.
	 * Holds identificative information of the application, the tasks that form
	 * it (and which one is the master), the host where they are running, the
	 * places from where we are getting events and handlers to both: tasks and
	 * hosts.
	 * Provides methods to:
	 * <ul>
	 * <li> Retrieve application information</li>
	 * <li>Monitoring: add/remove events to trace.</li>
	 * <li>Tuning: loading libraries, changing variables & parameter values,
	 * 	   adding/removing function calls and calling them explicitly. </li>
	 * </ul>
	 * Basically the monitoring a tuning methods call to the corresponding
	 * methods in AppTask for all the tasks that conform the application.
	 */
	class Application : public EventListener
	{
		typedef Queue<ECPMessage *> EventQueue;
		enum { MaxQueueSize = 1000 };
		public:
			/**
			 * @brief Constructor.
			 *
			 * @param appPath	Path to the executable of the application.
			 * @param argc		Number of arguments of the application.
			 * @param argv		Arguments of the application.
			 */
			Application (char const * appPath, int argc, char const ** argv);

			/**
			 * @brief Name getter.
			 * @return Name of the running program
			 */
			string GetName () const { return _name; }

			/**
			 * @brief Number of tasks getter.
			 * @return Number of tasks actually running
			 */
			int NumActiveTasks () const { return _tasks.Size (); }

			/**
			 * @brief Tasks getter.
			 * @return A collection of Task objects
			 */
			Tasks & GetTasks () { return _tasks; }

			/**
			 * @brief Hosts getter.
			 * @return A collection of Host objects that form the virtual machines
			 */
			Hosts & GetHosts () { return _hosts; }

			/**
			 * @brief Master task getter.
			 * @return A reference to the master task of the application
			 */
			Task * GetMasterTask () { return _masterTask; }

			/**
			 * @return The application status information
			 */
			Status GetStatus () const { return _status; }

			/**
			 * @brief Starts the application.
			 * @deprecated
			 */
			void Start ();

			// MONITORING ========================================================
			/**
			 * @brief Adds a definition of a new event to be traced in all running
			 * tasks of the application.
			 *
			 * @param e	Event to be traced.
			 * @return Number of tasks where the event tracing was added.
			 */
			int AddEvent (Event const & e);

			/**
			 * @brief Removes previously added event from all running tasks.
			 *
			 * @param eventId	Id of the event
			 * @param place		Location of the function where the event is recorded
			 *
			 * @return  number of tasks where the event was removed.
			 */
			int RemoveEvent (int eventId, InstrPlace place);

			// TUNING ==========================================================
			/**
			 * @brief Loads a shared library to all running tasks.
			 * This enables the Analyzer to load any additional code
			 * required for the tuning.
			 *
			 * @param libPath Path to the library.
			 * @return Number of tasks where the library is loaded.
			 */
			int LoadLibrary (string const & libPath);

			/**
			 * @brief Modifies a value of a specified variable in a given
			 * set of tasks.
			 *
			 * application process.
			 * @param varName	Name of the variable.
			 * @param varValue	New value for the variable.
			 * @param brkpt		---
			 * @return Number of tasks where the values were changed.
			 */
			int SetVariableValue (string const & varName,
							AttributeValue const & varValue, Breakpoint * brkpt);

			/**
			 * @brief Replaces all calls to a function with calls to another one
			 * in a given set of tasks.
			 * @param oldFunc	Name of the function to replace.
			 * @param newFunc	Name of the new function.
			 * @param brkpt		---
			 * @return Number of tasks where the function calls were changed.
			 */
			int ReplaceFunction (string const & oldFunc,
								string const & newFunc, Breakpoint * brkpt);

			/**
			 * @brief Inserts a new function invocation code at a given location
			 * in a given set of tasks
			 *
			 * @param funcName	Name of the function to call.
			 * @param nAttrs 	Number of parameters of the function.
			 * @param attrs 	Values for each parameter.
			 * @param destFunc	Function where the calls will be placed.
			 * @param destPlace	Point of the function where the calls will be placed.
			 * @param brkpt		---
			 * @return Number of tasks where the function calls were added.
			 */
			int InsertFunctionCall (string const & funcName, int nAttrs,
									Attribute * attrs, string const & destFunc,
									InstrPlace destPlace, Breakpoint * brkpt);

			/**
			 * @brief Inserts a new function invocation code in a given
			 * set of tasks and calls it once
			 *
			 * @param funcName	Name of the function to call
			 * @param nAttrs	Number of arguments of the function
			 * @param attrs		Values for each argument of the function
			 * @param brkpt		---
			 * @return Number of tasks where the function was called.
			 */
			int OneTimeFuncCall (string const & funcName,
								int nAttrs, Attribute * attrs, Breakpoint * brkpt);

			/**
			 * @brief Removes all calls to a given function from the given
			 * caller function in a given set of tasks.
			 * For example this method can be used to remove all flush() function
			 * calls from a debug() function.
			 *
			 * @param funcName		Name of the function
			 * @param callerFunc	Function that calls the function that will
			 * 						be removed
			 * @param brkpt			---
			 * @return Number of tasks where the function call is removed.
			 */
			int RemoveFuncCall (string const & funcName,
								string const & callerFunc, Breakpoint * brkpt);

			/**
			 * @brief Sets the value of an input parameter of a given
			 * function in a given set of tasks.
			 * This parameter value is modified before the function body is
			 * invoked. There exists the possibility to change the parameter value under condition,
			 * namely if the parameter has a value equal to requiredOldValue,
			 * only then its value is changed to new one.
			 * If the requiredOldValue is zero, then the value of the parameter is changed
			 * unconditionally.
			 *
			 * @param funcName 			Name of the function
			 * @param paramIdx			Id of the parameter to change
			 * @param newValue			New value for the parameter
			 * @param requiredOldValue	Required old value of the parameter to change it
			 * @param brkpt				---
			 * @return Number of tasks where the parameter was changed.
			 */
			int FuncParamChange (string const & funcName, int paramIdx,
					int newValue, int * requiredOldValue, Breakpoint * brkpt);

			/**
			 * @brief Installs a callback function that is called
			 * when a new task is started or an existing one is terminated.
			 *
			 * @param h	Handler for the new tasks.
			 */
			void SetTaskHandler (TaskHandler & h);

			/**
			 * @brief 	Installs a callback function that is called when a new
			 * host is added to the virtual machine or an existing one is
			 * removed.
			 *
			 * @param h	Handler for the new hosts.
			 */
			void SetHostHandler (HostHandler & h);

			/**
			 * @brief	Processes application events (ECP).
			 *
			 * @param 	Block indicates if the function blocks and waits for next
			 * 			event.
			 * @return	Number of processed events
			 */
			int ProcessEvents (bool block = true);

			/**
			 * @brief 	This method is called in the context of Event Collector
			 * 			thread.
			 *
			 * @param msg	Pointer to a message object that must be deleted by
			 * 				a receiver.
			 */
			void OnEvent (ECPMessage * msg);

			/**
			 * @brief 	This method is called when fatal EventCollector error
			 * 		  	occurs.
			 * Application changes its status to stAborted.
			 */
			void OnFatalError ();

		protected:
			/**
			 * @brief	Takes the proper actions depending on the kind of
			 * message received.
			 * <ul>
			 * <li> Register: adds the host where the new task was created and
			 * 				  creates a task object to represent it. </li>
			 * <li> Unregister: removes the task from the list of task.</li>
			 * <li> Event: calls DispatchEvent to handle it.</li>
			 * </ul>
			 *
			 * @param msg	Message that contains a request from an AC.
			 */
			void ProcessEvent (ECPMessage * msg);

			/**
			 * @brief	Finds the sender-task corresponding object and
			 * 			dispatches its event (see task).
			 * @param msg Message that contains an event request from an AC.
			 */
			void DispatchEvent (EventMsg const & msg);

			/**
			 * @brief Creates & adds a new host to the host list of the
			 * application.
			 *
			 * @param name	Name of the host
			 * @return Reference to the created host
			 */
			Host & AddHost (string const & name);

			/**
			 * @brief	Adds a task to the application list.
			 *
			 * @param	pid		Process identifier of the task.
			 * @param	mpiRank	MPI identifier of the task
			 * @param	name	Process name
			 * @param	host	Host where the task is running
			 */
			void AddTask (int pid, int mpiRank, string const & name, Host & h);

			/**
			 * @brief	Removes a task when an unregistered message is received
			 * 			(see processEvent).
			 * @param	tid		process (thread) identifier of the task.
			 */
			void RemoveTask (int tid);

		private:
			string 		_name;
			int 		_argc;
			char const ** _argv;
			Tasks		_tasks;
			Task 	 *  _masterTask;
			Hosts		_hosts;
			Status		_status;
			EventQueue 	_queue;
			TaskHandlers _taskHandlers;
			HostHandlers _hostHandlers;
	};

}; // of namespace Model

#endif



