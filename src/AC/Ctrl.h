
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
// ctrl.h
//
// AC (Tracer/Tuner Controller)
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#ifndef CTRL_H
#define CTRL_H

#include "TimeValue.h"
#include "Config.h"
#include "cmdline.h"
#include "TaskManager.h"
#include "StringArray.h"

#include <unistd.h>
//#include <sys/param.h>
#include <netdb.h>


/**
 * @class Controller
 * @brief Contains the main functionality of the AC, including its main loop
 * which runs until all tuning operations have been finished.
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class Controller
{	
public:
	/**
	 * @brief Constructor.
	 * @param cmdLine Class that provides commandline communications with the user.
	 */
	Controller (CommandLine & cmdLine);

	/**
	 * @brief Destructor.
	 */
	~Controller ();

	/**
	 * @brief Initializes all the necessary fields and starts the main loop of the AC.
	 *
	 * Creates a TaskManager objects and a Reactor and PTPAcceptor which will provide event
	 * handling and tuning capabilities.
	 */
	void Run (); 

	/**
	 * @brief Sets the _fInterrupted variable to 1.
	 */
	void Interrupt ();

private:
	/**
	* @brief Sets the clock difference for the task manager and starts the MPI task linked
	* to the AC.
	*
	* @param taskMngr Task manager object that manages the assigned task.
	*/
	void InitTracer (TaskManager & taskMngr);

	/**
	* @brief Reads the configuration file.
	*
	* Checks if the cmdLine recovered a configure file path and reads it. If not it tries
	* to read the default configuration file.
	*/
	void Configure ();

	/**
	* @brief Not implemented.
	* @param taskMngr Task manager object that manages the assigned task.
	*/
	void GetInstrumentationFunctions (TaskManager & taskMngr);

	/**
	* @brief Checks if the path passed is valid.
	*
	* Tries to open the file located in the passed path and throws an exception if it cant.
	*
	* @param taskMngr Task manager object that manages the assigned task.
	*
	* @throws SysException
	*/
	void CheckFile (char const * fileName);

	/**
	 * @brief Sets up the behavior of the program for the signal SIGINT.
	 */
	void SetupSignals ();

	/**
	 * @brief ---------
	 *
	 * @param taskMngr Task manager object that manages the assigned task.
	 *
	 * @return undefined
	 */
	bool ContinueExecution (TaskManager & taskMngr);

	/**
	 * @brief Implements the reaction to the signal SIGINT.
	 * @param signo Received signal.
	 */
	static void SignalHandler (int signo);
	

	static Controller * TheInstance;
	
private:
	CommandLine &	_cmdLine;
	char const * 	_configFile;
	Config 			_cfg;
	int				_fInterrupted;
};

#endif

