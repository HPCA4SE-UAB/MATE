
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

// MATE
// Copyright (C) 2002-2008 Ania Sikora, UAB.

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef __PROCESS_H__
#define __PROCESS_H__

// Local includes
#include "SysException.h"
#include "Pipe.h"
#include "Reactor.h"

namespace Common {
	/**
	 * @class Process
	 * @brief Abstract class, creates a new process to perform different
	 * operations on the overrided method Run().
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2001
	 */

	// ------------------------------------------------------------
	class Process
	{
	public:

		/**
		 * @brief Executes the process.
		 */
		virtual void Start ();

		/**
		 * @brief Returns process id.
		 */
		int GetPid () const
		{
			return _pid;
		}
	
	protected:
		/**
		 * @brief Constructor.
		 */
		Process ()
			: _pid (-1)
		{}
		
		virtual int Run () = 0;
	
		int _pid; // process id
	};
	
	/**
	 * @class ExecProcess
	 * @brief Executes a program as a child of the current process.
	 *
	 * @extends Process
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2001
	 */

	// ------------------------------------------------------------
	class ExecProcess : public Process
	{
	public:
		enum Status
		{
			stOutReady,
			stOutEof,
			stErrReady,
			stErrEof,
			stTimeout
		};

	public:

		/**
		 * @brief Constructor.
		 *
		 * Example usage:
		 *  \code
		      char * argv [] = { "/usr/bin/vi", "param1", "param2", 0 };
		      ExecProcess p ("/usr/bin/vi", argv);
		    \endcode
		 * Notes:
		 *	 - first element of argv must be program path
		 *	 - last element of argv table must be 0
		 *
		 * @param programPath Path of the program to execute.
		 * @param argv Arguments to pass to the execution of the program.
		 *
		 */
		ExecProcess (std::string const & programPath, char * const argv [])
			: _programPath (programPath), _argv (argv)
		{}

		/**
		 * @brief Executes the program.
		 *
		 * The standard outputs and inputs of the program will be redirected to
		 * internal pipes, to be handled on the WaitForEvent() method.
		 */
		void Start ();

		/**
		 * @brief Waits until an event is placed on any of the outputs
		 * of the process or the time limit is reached.
		 *
		 * If any event was sent by the process, it is placed on the buffer.
		 *
		 * @return Information about how the function ended and what was placed
		 * on the buffer.
		 */
		Status WaitForEvent (char * buffer, int bufSize, int & bytesRead,
				TimeValue * timeout = 0);

	protected:
		/**
		 * @brief Constructor.
		 */
		ExecProcess ()
			: _programPath (0), _argv (0)
		{}

		/**
		 * @brief Executes de process.
		 */
		int Run ();

	private:
		std::string const &   _programPath;	// program path
		char * const * _argv;		// program arguments

		Pipe 	_in;	// pipe from which forked process will read (its input)
		Pipe 	_out;	// pipe to which forked process will write (its output)
		Pipe 	_err;	// pipe to which forked process will write errors
	};

	//----------------------------------------------------------------------
	// revisit: class name as we are using rsh
	// revisit: description
	// rsh [-n] [-l username] hostname command

	/**
	 * @class RemoteProcess
	 * @brief Remotely executes a command in another machine.
	 *
	 * Uses the rsh program to perform the execution of the process.
	 *
	 * @extends ExecProcess
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2001
	 */
	
	// ------------------------------------------------------------
	class RemoteProcess : public ExecProcess
	{
	public:
	
		/**
		 * @brief Constructor.
		 *
		 * @param hostName Host where the command will be executed remotely.
		 * @param command Command to execute.
		 * @param rshPath Local path of the rsh program, default = "/usr/bin/rsh".
		 */
		RemoteProcess (std::string hostName, std::string command,
				std::string rshPath = std::string("/usr/bin/rsh"))
			: _hostName (hostName), _userName (0), _command (command),
			  _nullOutput (false), _rshPath(rshPath)
		{}
	
		/**
		 * @brief Constructor.
		 *
		 * @param hostName Host where the command will be executed remotely.
		 * @param command Command to execute.
		 * @param userName Name of the user on the host to execute the command.
		 * @param rshPath Local path of the rsh program, default = "/usr/bin/rsh".
		 */
		RemoteProcess (std::string hostName,
				std::string userName,
				std::string command,
				std::string rshPath = std::string("/usr/bin/rsh"))
			: _hostName (hostName), _userName (userName), _command (command),
			  _nullOutput (false), _rshPath(rshPath)
		{}

		/**
		 * @brief Enables output to null property.
		 */
		void SetOutputToNull ()
		{
			_nullOutput = true;
		}

	protected:
		int Run ();

		std::string GetHostName () const
		{
			return _hostName;
		}

	private:
		std::string _hostName;
		std::string _userName;
		std::string _command;
		std::string _rshPath;
		bool   _nullOutput;
	};
}

#endif
