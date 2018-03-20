
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
// AppTask.h
// 
// Abstract Application Model Classes
//
// Anna Sikora, UAB, 2004
//
//----------------------------------------------------------------------

#if !defined APPTASK_H
#define APPTASK_H

#include "auto_vector.h"
#include "Utils.h"
#include "sync.h"
#include "EventCollector.h"
#include "Syslog.h"
#include "Queue.h"
#include "ECPMsg.h"
#include "AppEvent.h"
#include "ACProxy.h"
#include "Host.h"
#include <vector>
#include <map>
#include <string>


namespace Model {

	// task/app status
	enum Status { stNotStarted, stStarting, stRunning, stFinished, stAborted };
	
	class Task;//Will be defined later
	
	/**
	 * @class 	TaskHandler
	 * @brief 	Abstract class that provides methods to determine
	 * 			if a task is started or terminated.
	 */
	class TaskHandler {
	public:
		/**
		 * @brief Called when a new task is started.
		 * @param t	Started task object.
		 */
		virtual void TaskStarted(Task & t) = 0;
		/**
		 * @brief Called when a task is terminated.
		 * @param t	Terminated task object.
		 */
		virtual void TaskTerminated(Task & t) = 0;
	};

	typedef std::vector <TaskHandler *> TaskHandlers;

	class Application;//Defined in AppModel

	/**
	 * @class	Task
	 * @brief	Encapsulates information to define the tasks that form the
	 * 			application.
	 * The data structure of a task consists of identification data
	 * (pid, mpiRank, name), status data, where it is running (host), which
	 * events are being collected from it and if it is either a master task or
	 * not.
	 * Provides methods to:
	 * <ul>
	 * <li>Retrieve application information</li>
	 * <li>Monitoring: add/remove events to trace.</li>
	 * <li>Tuning: loading libraries, changing variables & parameter values,
	 * 	   adding/removing function calls and calling them explicitly. </li>
	 * </ul>
	 */
	class Task {
		friend class Application;
		public:
			/**
			 * @brief	Pid getter.
			 * @return 	Globally unique process id
			 */
			int GetPid() const { return _pid; }
			/**
			 * @brief	MPI Rank getter.
			 * @return 	MPI Rank of the task.
			 */
			int GetMpiRank() const { return _mpiRank; }

			/**
			 * @brief 	Name getter.
			 * @return 	Process name
			 */
			string GetName() const { return _name; }

			/**
			 * @brief	Host getter.
			 * @return 	Reference to the host object this task is running on.
			 */
			Host & GetHost() const { return _host; }

			/**
			 * @brief Indicates if the task is still running.
			 * @return True if still running false otherwise.
			 */
			bool IsRunning() const { return _status == stRunning; }

			/**
			 * @brief Indicates if the task is the master task.
			 * @return True if master false otherwise.
			 */
			bool IsMaster() const { return _isMaster; }

			/**
			 * @brief Status getter.
			 *
			 * @return Task Status information
			 */
			Status GetStatus() const { return _status; }

			//	MonitoredEvents - collection of events being monitored in this tasks
			//	TuningActions - a collection of tuning actions performed in this task

			// MONITORING =====================================================
			/**
			 * @brief Adds a definition of new event to be traced in this task.
			 * @param e Event to be traced.
			 * @return Number of tasks where the event tracing was added.
			 */
			void AddEvent(Event const & e);

			/**
			 * @brief Removes previously added event from this task.
			 * @param eventId	Id of the event
			 * @param place		Place of the function where the event is recorded
			 *
			 * @return  Number of tasks where the event was removed.
			 */
			bool RemoveEvent(int eventId, InstrPlace place);

			// TUNING =========================================================
			/**
			 * @brief Loads a shared library to this task.
			 * This enables the Analyzer to load any additional code required
			 * for the tuning.
			 *
			 * @param libPath Path to the library.
			 * @return Number of tasks where the library is loaded.
			 */
			void LoadLibrary(string const & libPath);

			/**
			 * @brief Modifies a value of a specified variable in the running
			 * task application process.
			 * @param varName	Name of the variable.
			 * @param varValue	New value for the variable.
			 * @param brkpt		---
			 * @return Number of tasks where the values were changed.
			 */
			void SetVariableValue(string const & varName,
							AttributeValue const & varValue, Breakpoint * brkpt);

			/**
			 * @brief Replaces all calls to a function with calls to another
			 * one in this task.
			 * @param oldFunc	Name of the function to replace.
			 * @param newFunc	Name of the new function.
			 * @param brkpt		---
			 * @return Number of tasks where the function calls were changed.
			 */
			void ReplaceFunction(string const & oldFunc,
							string const & newFunc, Breakpoint * brkpt);
			/**
			 * @brief	Inserts a new function invocation code at a given location
			 * in this task.
			 *
			 * @param funcName	Name of the function to call.
			 * @param nAttrs 	Number of parameters of the function.
			 * @param attrs 	Values for each parameter.
			 * @param destFunc	Function where the calls will be placed.
			 * @param destPlace	Place of the function where the calls will be placed.
			 * @param brkpt		---
			 * @return Number of tasks where the function calls were added.
			 */
			void InsertFunctionCall(string const & funcName, int nAttrs,
									Attribute * attrs, string const & destFunc,
									InstrPlace destPlace, Breakpoint * brkpt);
			/**
			 * @brief Inserts a new function invocation code in this task and
			 * invokes it once.
			 *
			 * @param funcName	Name of the function to call
			 * @param nAttrs	Number of arguments of the function
			 * @param attrs		Values for each argument of the function
			 * @param brkpt		---
			 * @return Number of tasks where the function was called.
			 */
			void OneTimeFuncCall(string const & funcName,
							int nAttrs, Attribute * attrs, Breakpoint * brkpt);
			/**
			 * @brief Removes all calls to a given function from the given caller
			 * function in this task.
			 * For example this method can be used to remove all flush() function
			 * calls from a debug() function.
			 *
			 * @param funcName		Name of the function
			 * @param callerFunc	Function that calls the function that will
			 * 						be removed
			 * @param brkpt			---
			 * @return Number of tasks where the function call is removed.
			 */
			void RemoveFuncCall(string const & funcName,
							string const & callerFunc, Breakpoint * brkpt);

			/**
			 * @brief Sets the value of an input parameter of a given function in
			 * this task.
			 * This parameter value is modified before the function body is
			 * invoked. It is also possible to change the parameter value under
			 * condition, namely if the parameter has a value equal to
			 * requiredOldValue, only then its value is changed to new one.
			 * If the requiredOldValue is zero, then the value of the parameter
			 * is changed unconditionally.
			 *
			 * @param funcName 			Name of the function
			 * @param paramIdx			Id of the parameter to change
			 * @param newValue			New value for the parameter
			 * @param requiredOldValue	Required old value of the parameter
			 * 							to change
			 * @param brkpt				---
			 * @return Number of tasks where the parameter was changed.
			 */
			void FuncParamChange(string const & funcName, int paramIdx,
									int newValue, int * requiredOldValue,
									Breakpoint * brkpt);

			/**
			 * @brief Installs a callback function that is called when this task
			 * terminates.
			 */
			void SetTaskExitHandler(TaskHandler & h);

		protected:
			/**
			 * @brief Constructor.
			 *
			 * @param pid		Globally unique task id.
			 * @param mpiRank	Id associated to MPI
			 * @param name		Process name.
			 * @param h			Reference to the host object this task is running
			 * 					on.
			 */
			Task(int pid, int mpiRank, string const & name, Host & h);

			/**
			 * @brief Sets if this task is Master or not.
			 * @param value	Determines if its Master or not.
			 */
			void SetMaster(bool value) { _isMaster = value; }

			/**
			 * @return ACProxy object of this task.
			 */
			ACProxy * GetACProxy();

			/**
			 * @brief Finds the Event of the given message, gets its handler
			 * and Record and passes it to HandleEvent()
			 * @param msg
			 */
			void DispatchEvent(EventMsg const & msg);
		
		private:
			int		_pid;
			int 	_mpiRank;
			string 	_name;
			Host  &	_host;
			Status	_status;
			Events  _events;
			bool	_isMaster;
	};
	
	/**
	 * @brief Tasks encapsulate methods to work with lists of Task objects.
	 * The data structure to hold the information is an auto_vector.
	 * This class provides methods to add, remove, access Task objects in an
	 * array. It also provides methods to find Tasks and for measuring the array.
	 */
	class Tasks
	{
	public:
		enum { NotFound = -1 };

		/**
		 * @brief Constructor.
		 */
		Tasks () : _count (0) {}	
		/**
		 * @brief Adds a task to the list.
		 * @param task	The task to add.
		 */
		void Add(auto_ptr<Task> & task);

		/**
		 * @brief Deletes a task from the list.
		 * @param index	Position in the list of the task to delete.
		 */
		void Delete(int index);

		/**
		 * @brief Accessor to the array.
		 *
		 * @param index	Position of the array to access.
		 * @return Pointer to the specified position
		 */
		Task const * operator[](int index) const;

		/**
		 * @brief Accessor to the array.
		 * @param index	Position of the array to access.
		 * @return Pointer to the specified position
		 */
		Task * operator[](int index);

		/**
		 * @brief Finds a task by ID and returns its index.
		 * @param id	Identifier of the task.
		 * @returns Index of the task or NotFound if a task with given ID is
		 * not stored in the collection.
		 */
		int FindById(int id);

		/**
		 * @brief Size getter.
		 * @return Number of stored tasks
		 */
		int Size() const;

		/**
		 * @brief Finds a task by ID and returns a reference to it.
		 * @param id	Identifier of the task (pid).
		 * @return A reference to the found task.
		 * @throws Exception if not found.
		 */
		Task & GetById(int id);

		/**
		 * @brief Removes a task by ID from the list.
		 *	@param id	Identifier of the task.
		 *	@return A reference to the removed task or a null pointer
		 *	if it was not present.
		 */
		auto_ptr<Task> Remove(int id);
	private:
		auto_vector<Task> 	_arr; 
		int					_count;
	};		

}; // of namespace Model

#endif

