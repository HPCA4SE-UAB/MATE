
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

// ------------------------------------------------------------------
// Socket.cpp
//
// ------------------------------------------------------------------

#include "Socket.h"
#include "TimeValue.h"
#include "Reactor.h"
#include <unistd.h>
#include <stdio.h> // debug

SocketBase::SocketBase (int family, int type, int protocol)
	: _localPort (0)
{
	_hSocket = ::socket (family, type, protocol);
	if (_hSocket < 0)
		throw SysException ("Socket creation failed");
}
		
SocketBase::SocketBase (int hSocket, Address & addr)
	: _hSocket (hSocket), _addr (addr), _localPort (0)
{
}

SocketBase::~SocketBase ()
{
	::close (_hSocket);
}

SocketPtr SocketBase::Accept ()
{
	Address addr;
	socklen_t size = addr.GetSize ();
	int hSock = ::accept (_hSocket, addr, &size);
	if (hSock < 0)
		throw SysException ("Socket accept failed");
	return SocketPtr (new Socket (hSock, addr));
}

void SocketBase::Bind (int port)
{
	Address address (port);
	int reuse = 1;
    if (setsockopt(_hSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        throw SysException ("setsockopt(SO_REUSEADDR) failed");
	int status = ::bind (_hSocket, address, sizeof (address));
	if (status < 0)
	{
		throw SysException ("Bind socket failed");
	}
	_localPort = port;
	_addr = address;
}

void SocketBase::Connect (Address & address)
{
	int status = ::connect (_hSocket, address, sizeof (address));

	if (status < 0)
	{
		throw SysException ("Connect socket failed");
	}
	_addr = address;
}

void SocketBase::Connect (std::string const & host, int port)
{

	Address addr (host, port);
	Connect (addr);
}

void SocketBase::Listen (int backLog)
{
	int status = ::listen (_hSocket, backLog);
	if (status < 0)
		throw SysException ("Socket listen failed");
}

// data must be already converted to network byte order
int SocketBase::DoSend (char const * buf, int bufSize, int flags)
{		
    //printf ("DoSending0 %d bytes...\n", bufSize);



	int bytesWritten = ::send (_hSocket, buf, bufSize, flags);


	//printf ("DoSending1 %d bytes...\n", bufSize);
	//close(_hSocket);
	//printf ("DoSending2 sent %d bytes...\n", bytesWritten);
	if (bytesWritten < 0)
	{
		//printf ("DoSending3 sent %d bytes...\n", bytesWritten);
		throw SysException ("Socket send failed");
	}
	//printf ("DoSending4 sent %d bytes...\n", bytesWritten);
	return bytesWritten;
}

// function blocks until all bytes are sent
void SocketBase::Send (char const * buf, int bufSize, int flags)
{
//printf ("Sending %d bytes...", bufSize);	
  	size_t bytesWritten;
  	ssize_t n;

  	for (bytesWritten = 0; bytesWritten < bufSize; bytesWritten += n)
    {
		n = DoSend (buf + bytesWritten, bufSize - bytesWritten);
      	if (n == 0)
			break;
    }
}
		
void SocketBase::Send (std::string const & str, int flags)
{
	return Send (str.c_str (), str.length (), flags);
}

void SocketBase::Send (ByteStream & stream, int flags)
{
	return Send (stream.GetData(), stream.GetDataSize(), flags);
}

// function returns number of bytes received
int SocketBase::Receive (char * buf, int bufSize, int flags)
{
	int bytesReceived = ::recv (_hSocket, buf, bufSize, flags);
	if (bytesReceived == -1)
		throw SysException ("Socket receive failed");
	return bytesReceived;
}

// if no incoming data is available at the socket, 
// the recv call blocks and waits for data to arrive 
// function returns number of bytes received
int SocketBase::ReceiveN (char * buf, int bufSize, int flags)
{
  	size_t bytesRead;
  	ssize_t n;

  	for (bytesRead = 0; bytesRead < bufSize; bytesRead += n)
    {
      n = Receive ((char *) buf + bytesRead, bufSize - bytesRead);
      //if (n == 0)
        //break;
    }
  	return bytesRead;
}


void SocketBase::SetOption (int level, int option, char const * value, int valueSize)
{
	//SOL_SOCKET, TCP_NODELAY, (const char *)&value, sizeof(int)
	int status = ::setsockopt (_hSocket, level, option, value, valueSize);
	if (status < 0)
	{
		std::cerr << "SetSockOpt status = " << status << std::endl;
		throw SysException ("Set socket option failed");
	}
}

int SocketBase::GetOption (int level, int option, char * value, int & valueSize)
{	
	int status = ::getsockopt (_hSocket, level, option, value, (socklen_t*)&valueSize);
	if (status < 0)
		throw SysException ("Get socket option failed");
}

void SocketBase::SetTCPNoDelay (bool value)
{
	SetOption (IPPROTO_TCP, TCP_NODELAY, (const char *)&value, sizeof(int));
}

void SocketBase::SetReuseAddress (bool value)
{
	SetOption (SOL_SOCKET, SO_REUSEADDR, (const char *)&value, sizeof(int));
}

void SocketBase::SetKeepAlive (bool value)
{
	SetOption (SOL_SOCKET, SO_KEEPALIVE, (const char *)&value, sizeof(int));
}

void SocketBase::SetReceiveTimeout (int timeoutMs)
{
	SetOption (SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeoutMs, sizeof(int));
}

void SocketBase::SetSendTimeout (int timeoutMs)
{
	SetOption (SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeoutMs, sizeof(int));
}

int SocketBase::GetReceiveTimeout ()
{
	int val = 0;
	int len = sizeof(int);
	return GetOption (SOL_SOCKET, SO_RCVTIMEO, (char *)&val, len);
}

int SocketBase::GetSendTimeout ()
{
	int val = 0;
	int len = sizeof(int);
	return GetOption (SOL_SOCKET, SO_SNDTIMEO, (char *)&val, len);
}

// TCP/IP server socket -----------------------------------------------------

ServerSocket::ServerSocket (int port, int backLog)
	: _backLog (backLog)
{
	Syslog::Debug ("[ServerSocket::ServerSocket] port: %d", port);
	_sock.SetReuseAddress (true);
	_sock.Bind (port);
	Syslog::Debug ("[ServerSocket::ServerSocket] binded");
}

SocketPtr ServerSocket::Accept ()
{
//	_sock.Listen (_backLog);
	return _sock.Accept ();
}

void ServerSocket::Listen ()
{
	_sock.Listen (_backLog);
}

// waits for connection given timeout (in millis)
// if connection is accepted it is returned through
// socket auto pointer. otherwise accept timeout
// occurs and returned socket autopointer is empty.
SocketPtr ServerSocket::Accept (int timeoutMs)
{
	_sock.Listen (_backLog);
	// use demulitiplexer to provide
	// accept with timeouts
	TimeValue timeout (timeoutMs / 1000, (timeoutMs % 1000) * 1000);
	EventDemultiplexer demux;
	demux.AddHandle (_sock.GetHandle ());
	// wait for connection
	int status = demux.Select (&timeout);
	if (status == 0)
	{
		 // timed-out,
		 // return empty auto pointer
		return SocketPtr ();
	}
	else
	{
		// accept connection
		return _sock.Accept ();
	}
}

void Socket::Send (ByteStream & stream, int flags)
{
	_sock.Send (stream.GetData(), stream.GetDataSize(), flags);
}
