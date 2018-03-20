
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

//-----------------------------------------
//
// process.cpp
//
// version with pipes
//
// Anna Sikora, UAB, January, 2001
//
//-----------------------------------------

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <strings.h>
#include <iostream>
#include "Process.h"
#include "Reactor.h"

using namespace std;
using namespace Common;

void Process::Start ()
{
	_pid = fork ();
	if (_pid == -1)	// error 
		throw SysException ("Fork failed");
	if (_pid == 0) // child process
	{
		try
		{
			int status = Run ();
			// exit child process
			exit (status);
		}
		catch (SysException & e)
		{
			e.Display ();
			exit (-1);
		}
	}
	// else do nothing - we are main process
}

//----------------------------------------------------------------------

void ExecProcess::Start ()
{
	_pid = fork ();
	if (_pid == -1)	// error: fork failed
	{
		throw SysException ("Fork failed");
	}
	if (_pid == 0) 
	{
		// we are child process
		// redirect - stdin to read from pipe _in 
		// 			- stdout to write to pipe _out
		// 			- stderr to write to pipe _err		
		::dup2 (_in.GetRead (), 0); 		
		::dup2 (_out.GetWrite (), 1); 
		::dup2 (_err.GetWrite (), 2); 
		// close all other descriptors, leave only stdin, stdout and stderr
		int fdTableSize = ::getdtablesize ();
		for (int i = 3; i<fdTableSize; ++i)
		{
			::close (i);
		}
		try
		{
			int status = Run ();
			// exit child process?????????
			//exit (status);
		}
		catch (SysException & e)
		{
			e.Display (); // must print to stderr
			exit (-1);
		}
	}
	else
	{
		// we are main process
		// main process can write to stdin of forked process
		// so, it closes read from in
		_in.CloseRead ();
		// main process can read from stdout of forked process
		// so, it closes write from out
		_out.CloseWrite ();
		_err.CloseWrite ();
		
	}
}

int ExecProcess::Run ()
{
	// run a program
	int status = execvp (_programPath.c_str(), _argv);
	if (status == -1) 
	{
		// error
		throw SysException ("Execv failed", _programPath);
	}
	// on success exec function does not return
	//return 0;
}

ExecProcess::Status ExecProcess::WaitForEvent 
	(char * buffer, int bufSize, int & bytesRead, TimeValue * timeout)
{
	assert (_out.IsReadOpen () || _err.IsReadOpen ());
	EventDemultiplexer demux;
	// register open handler
	if (_out.IsReadOpen ())
		demux.AddHandle (_out.GetRead ());
	if (_err.IsReadOpen ())
		demux.AddHandle (_err.GetRead ());
	while (true)
	{
		int numEvents = demux.Select (timeout);
		if (numEvents < 1)
		{
			// timeout
			buffer [0] = 0;
			bytesRead = 0;
			_err.CloseRead ();
			_out.CloseRead ();
			return stTimeout;
		}
		// check out pipe
		if (_out.IsReadOpen () && demux.IsHandleActivated (_out.GetRead ()))
		{
			// read data from _out pipe
			bytesRead = _out.Read (buffer, bufSize);
			if (bytesRead > 0)
			{
				return stOutReady;
			}
			else //if (bytesRead == 0)
			{
				// eof
				_out.CloseRead ();
				return stOutEof;
			}
		}
		if (_err.IsReadOpen () && demux.IsHandleActivated (_err.GetRead ()))
		{
			// read data from _err pipe
			bytesRead = _err.Read (buffer, bufSize);
			if (bytesRead > 0)
			{
				return stErrReady;
			}
			else //if (bytesRead == 0)
			{
				// eof
				_err.CloseRead ();
				return stErrEof;
			}
		}
	}
}								  
	
//----------------------------------------------------------------------
int RemoteProcess::Run ()
{
	char * rshPath = (char *) _rshPath.c_str();
	
	// {rshPath, _hostName, _command, 0 }
	// {rshPath, "-l", _userName, _hostName, _command, 0 }
	// {rshPath, "-n", _hostName, _command, 0 }
	// {rshPath, "-n", "-l", _userName, _hostName, _command, 0 }
		
	char * argv [] = {rshPath, 0, 0, 0, 0, 0, 0};
	int idx = 1;
	// build array of arguments
	if (_nullOutput)
	{
		argv [idx++] = "-n";
	}
	if (_userName.size() != 0)
	{
		argv [idx++] = "-l";
		char userName[_userName.size()];
		strcpy(userName, _userName.c_str());
		argv [idx++] = userName;
	}
	char hostName[_hostName.size()];
	char command[_command.size()];
	strcpy(hostName, _hostName.c_str());
	strcpy(command, _command.c_str());

	argv [idx++] = hostName;
	argv [idx] = command;
	
	// run a remote program using rsh
	int status = execv (rshPath, argv);
	if (status == -1) // error
	{
		// error
		throw SysException ("Execv failed", rshPath);
		// catch it in Start () and display the error
	}
	// success
	return 0;
}
