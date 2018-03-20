
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
// ACProxy.h
// 
// ACProxy - class that connects to the AC (monitor and tuner) host 
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#ifndef ACPROXY_H
#define ACPROXY_H

#include <string>
#include <stdarg.h>
#include "Socket.h"
#include "Utils.h"
#include "PTPProtocol.h"

/**
 * @class 	ACProxy
 * @brief	Creates a connection with AC and acts as an interface with it.
 * This class has a socket object to represent the connection and a PTPProtocol
 * object for making requests in a common language.
 *
 * Its methods encapsulate the requests in the adequate kind of request object
 * and use the protocol to serialize and write them in the socket.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */
class ACProxy {
	public:
		/**
		 * @brief Constructor, creates a connection with the given host:port.
		 * 
		 * @param host	host were the target AC is running
		 * @param port  port for the AC process in the host
		 */
		//ACProxy (std::string const & host, int port = 9900)
		ACProxy (std::string const & host, int const port)
		 : _host (host), _port (port), _socket (host, port)
		{
			Syslog::Debug ("[ACProxy] host: %s, port %d", _host.c_str(), port);
		}
	
		/**
		 * @brief	Starts the execution of the application in the AC host
		 *
		 * @param appPath		path to the application executable
		 * @param argc			number of arguments to the application main
		 * @param argv			argument vector to the application main
		 * @param analyzerHost	node where the analyzer is executed
		 */
		void StartApplication (char const * appPath, int argc, char const ** argv,
				char const * analyzerHost);

		/**
         * @brief	Requests for adding an instruction in the target application.
		 *
		 * @param tid		identifier of the thread in which we will add the
		 *					instruction
		 * @param eventId	identifier of the event
		 * @param fName		name of the function in which we will add
		 *					the instruction
		 * @param place 	place in the function where we will add the 
		 * 					instruction
		 * 		  values are: instrUnknown, ipFuncEntry & ipFuncExit
		 * @param nAttrs	number of Attributes
		 * @param attrs		Attributes array
		 */
		void AddInstr (int tid, int eventId, std::string const & fName, 
					   InstrPlace place, int nAttrs, Attribute * attrs);

		/**
         * @brief	Requests for adding an instruction in the target application.
		 *
		 * @param tid		identifier of the thread in which we will add the
		 *					instruction
		 * @param eventId	identifier of the event
		 * @param fName		name of the function in which we will add
		 *					the instruction
		 * @param place 	place in the function where we will add the 
		 * 					instruction
		 * 		  values are: instrUnknown, ipFuncEntry & ipFuncExit
		 * @param nAttrs	number of Attributes
		 * @param attrs		Attributes array
		 * @param nPapi		number of Papi Metrics
		 * @param PapiMetrics		array of Papi Metrics
		 */
		void AddInstr (int tid, int eventId, std::string const & fName, 
					   InstrPlace place, int nAttrs, Attribute * attrs,
					   int nPapi, std::string * PapiMetrics);

		/**
		 * @brief	Requests for removing an instruction from the target 
 		 * 			application.
		 *
		 * @param tid 		identifier of the thread in which we will remove
		 *					the instruction
		 * @param enventId 	identifier of the associated event
		 * @param place		place in the function where the instruction will 
		 *					be removed
		 */
		void RemoveInstr (int tid, int eventId, InstrPlace place);

		/**
		 * @brief	Requests for loading a library in the target application.
		 *
		 * @param tid identifier of the thread in which we will load the library
		 * @libPath library path
		 */
		void LoadLibrary (int tid, std::string const & libPath);

		/**
		 * @brief	Requests for changing the value of a variable in the target
		 *			application.
		 *
		 * @param tid 		identifier of the thread in which the variable is
		 * 					placed
		 * @param varName	name of the variable to change
		 * @param varValue	new value for the variable
		 * @param brkpt		---
		 */
		void SetVariableValue (int tid, std::string const & varName,
							AttributeValue const & varValue, Breakpoint * brkpt);

		/**
		 * @brief	Requests for changing all the instances of a function from
	     *			the target application.
   		 *
		 * @param tid		identifier of the thread in which the function is
		 *					placed
		 * @param oldFunc	name of the function to be changed
		 * @param newFunc	name of the new function
		 * @param brkpt		---
		 */
		void ReplaceFunction (int tid, std::string const & oldFunc,
							std::string const & newFunc, Breakpoint * brkpt);

		/**
		 * @brief	Requests for the insertion of a function call in a point of
		 *			the target application.
		 *
		 * @param tid		identifier of the thread in which the call will be
		 *					placed
		 * @param funcName	name of the function
		 * @param nAttrs	number of attributes
		 * @param attrs		attributes vector
		 * @param destFunc	name of the destination function
		 * @param destPlace	point where the call will be placed
		 * @param brkpt		---
		 */
		void InsertFunctionCall (int tid, std::string const & funcName,
								 int nAttrs, Attribute * attrs,
								 std::string const & destFunc, 
								 InstrPlace destPlace, Breakpoint * brkpt);

		/**
		 * @brief	Requests for the call of a function in this point of
		 *			the target application execution.
		 *
		 * @param tid		identifier of the thread in which the call will be 
		 *					placed
		 * @param funcName 	name of the function
		 * @param nAttrs	number of attributes
		 * @param attrs		attributes vector
		 * @param brkpt		---
		 */
		void OneTimeFuncCall (int tid, std::string const & funcName,
							int nAttrs, Attribute * attrs, Breakpoint * brkpt);

		/**
	     * @brief	Requests for removing all the calls to a function in
		 *			the target application.
		 *
		 * @param tid			identifier of the thread in which the call
		 * 						will be removed
		 * @param funcName		name of the function to be removed
		 * @param callerFunc	function which makes the call
		 * @param brkpt			---
		 */
		void RemoveFuncCall (int tid, std::string const & funcName,
							std::string const & callerFunc, Breakpoint * brkpt);

		/**
		 * @brief	Requests for the changing of the value of a certain parameter
		 * 			in one function of the target application.
		 *
		 * @param tid			identifier of the thread in which the function 
		 * 						is placed
		 * @param funcName		name of the function
		 * @param paramIdx		position of the parameter in the parameter list
		 * @param newValue		new value for the argument
		 * @param requiredOldValue	old value required to change for the new one
		 * @param brkpt			---
		 */
		void FuncParamChange (int tid, std::string const & funcName,
							  int paramIdx, int newValue, int * requiredOldValue, 
							  Breakpoint * brkpt);

	private:
		std::string const & _host;
		int    				_port;
		Socket 				_socket;
		PTPProtocol 		_protocol;
};

#endif

