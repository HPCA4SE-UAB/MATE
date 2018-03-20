
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

#ifndef __SOCKET_H__
#define __SOCKET_H__

// Local includes
#include "Address.h"
#include "Syslog.h"
#include "ByteStream.h"

// C++ includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <memory>
#include <strstream>
#include <stdio.h>

namespace Common {

	//version Common
	class Socket;

	// Socket Smart Pointer
	typedef std::auto_ptr<Socket> SocketPtr;

	/**
	 * @class SocketBase
	 * @brief Represents an endpoint for communication between two machines.
	 *
	 * This class works as an adapter for the socket functions included on the
	 * sys/socket library.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class SocketBase
	{
	public:
		/**
		 * @brief Constructor.
		 *
		 * @param family Socket family, default AF_INET.
		 * @param type Socket type, default SOCK_STREAM.
		 * @param protocol Socket protocol, default 0.
		 *
		 * @throws SysException
		 */
		SocketBase (int family = AF_INET, int type = SOCK_STREAM, int protocol = 0);

		/**
		 * @brief Constructor.
		 *
		 * Sets the socket to the given handle with the given address.
		 *
		 * @param hSocket Socket handle.
		 * @param addr Socket address.
		 */
		SocketBase (int hSocket, Address & addr);

		/**
		 * @brief Destructor.
		 */
		virtual ~SocketBase ( );

		/**
		 * @brief Accepts a connection and creates a socket.
		 */
		SocketPtr Accept ();

		/**
		 * @brief Associates the socket with a local endpoint.
		 * @param port Port where the socket will perform the connection.
		 */
		void Bind (int port);

		/**
		 * @brief Sets the socket to a listening state.
		 * @param backLog Maximum length of the queue for pending connections.
		 */
		void Listen (int backLog);

		/**
		 * @brief Connects a socket on the given address.
		 */
		void Connect (Address & address);

		/**
		 * @brief Connects a socket on the given host and port.
		 */
		void Connect (std::string const & host, int port);

		// data must be converted to network byte order
		/**
		 * @brief Sends the data through the socket converted
		 * to network byte order.
		 *
		 * @throws SysException
		 */
		void Send (char const * buf, int bufSize, int flags = 0);

		/**
		 * @brief Sends the data through the socket converted
		 * to network byte order.
		 *
		 * @throws SysException
		 */
		void Send (std::string const & str, int flags = 0);

		/**
		 * @brief Sends the data through the socket converted
		 * to network byte order.
		 *
		 * @throws SysException
		 */
		void Send (ByteStream & stream, int flags = 0);

		// if no incoming data is available at the socket,
		// the recv call blocks and waits for data to arrive
		/**
		 * @brief Gets data from the socket.
		 *
		 * If no incoming data is available at the socket, the call blocks
		 * and waits for data to arrive.
		 *
		 * @return Number of bytes received.
		 *
		 * @throws SysException
		 */
		int Receive (char * buf, int bufSize, int flags = 0);

		/**
		 * @brief Gets data from the socket, performs multiple Receive() calls
		 * until the buffer is full.
		 *
		 * @return Number of bytes received.
		 *
		 * @throws SysException
		 */
		int ReceiveN (char * buf, int bufSize, int flags = 0);

		/**
		 * @brief Cast to int returns the socket handle.
		 */
		operator int () { return _hSocket; }

		/**
		 * @brief Returns socket handle.
		 */
		int GetHandle () const { return _hSocket; }

		/**
		 * @brief Sets the TCPNoDelay property.
		 * @throws SysException
		 */
		void SetTCPNoDelay (bool value);

		/**
		 * @brief Sets the KeepAlive property.
		 * @throws SysException
		 */
		void SetKeepAlive (bool value);

		/**
		 * @brief Sets the ReuseAddress property.
		 * @throws SysException
		 */
		void SetReuseAddress (bool value);

		/**
		 * @brief Sets the ReceiveTimeout property.
		 * @throws SysException
		 */
		void SetReceiveTimeout (int timeoutMs);

		/**
		 * @brief Sets the SendTimeout property.
		 * @throws SysException
		 */
		void SetSendTimeout (int timeoutMs);

		/**
		 * @brief Gets the ReceiveTimeout property.
		 * @throws SysException
		 */
		int GetReceiveTimeout ();

		/**
		 * @brief Gets the SendTimeout property.
		 * @throws SysException
		 */
		int GetSendTimeout ();

		/**
		 * @brief Returns the address where the socket is connected.
		 */
		Address const & GetAddress () const { return _addr; }

		/**
		 * @brief Returns the port the socket is listening.
		 */
		int GetLocalPort () const { return _localPort; }

	protected:

		/**
		 * @brief Sets a socket option.
		 *
		 * @param level Protocol level at which the option resides.
		 * @param option Option name.
		 * @param value Option value to set.
		 * @param valueSize Size of the value.
		 *
		 * @throws SysException
		 */
		void SetOption (int level, int option, char const * value, int valueSize);

		/**
		 * @brief Gets the value of a socket option.
		 *
		 * @param level Protocol level at which the option resides.
		 * @param option Option name.
		 * @param value Buffer to save the value.
		 * @param valueSize Integer to save the size of the value.
		 *
		 * @throws SysException
		 */
		int GetOption (int level, int option, char * value, int & valueSize);

		/**
		 * @brief Sends the data through the socket.
		 *
		 * @throws SysException
		 */
		int DoSend (char const * buf, int bufSize, int flags = 0);

	protected:
		int		_hSocket;		// socket handle
		Address _addr;			// remote address
		int		_localPort;		// local port
	};

	/**
	 * @class Socket
	 * @brief Holds a SocketBase object and represents a client socket.
	 *
	 * All the functions in this class are present on the SocketBase class and have
	 * the same functionality.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class Socket
	{
		friend class SocketBase;
	public:

		/**
		 * @brief Constructor.
		 * @throws SysException
		 */
		Socket (Address & address);

		/**
		 * @brief Constructor.
		 * @throws SysException
		 */
		Socket (std::string const & host, int port);

		/**
		 * @brief Returns the address to which the socket is connected.
		 */
		Address const & GetRemoteAddress () const { return _sock.GetAddress (); }

		/**
		 * @brief Returns the local address the socket is listening to.
		 */
		Address GetLocalAddress () const { return Address (_sock.GetLocalPort ()); }

		/**
		 * @brief Returns the local port the socket is listening.
		 */
		int GetLocalPort () const { return _sock.GetLocalPort (); }

		/**
		 * @brief Sends the data through the socket converted
		 * to network byte order.
		 *
		 * @throws SysException
		 */
		void Send (char const * buf, int bufSize, int flags = 0);

		/**
		 * @brief Sends the data through the socket converted
		 * to network byte order.
		 *
		 * @throws SysException
		 */
		void Send (std::string const & str, int flags = 0);

		/**
		 * @brief Sends the data through the socket converted
		 * to network byte order.
		 *
		 * @throws SysException
		 */
		void Send (ByteStream & stream, int flags = 0);

		/**
		 * @brief Gets data from the socket.
		 *
		 * If no incoming data is available at the socket, the call blocks
		 * and waits for data to arrive.
		 *
		 * @return Number of bytes received.
		 *
		 * @throws SysException
		 */
		int Receive (char * buf, int bufSize, int flags = 0);

		/**
		 * @brief Gets data from the socket, performs multiple Receive() calls
		 * until the buffer is full.
		 *
		 * @return Number of bytes received.
		 *
		 * @throws SysException
		 */
		int ReceiveN (char * buf, int bufSize, int flags = 0);

		/**
		 * @brief Cast to int returns the socket handle.
		 */
		operator int () { return _sock.GetHandle (); }

		/**
		 * @brief Returns socket handle.
		 */
		int GetHandle () const { return _sock.GetHandle (); }

		/**
		 * @brief Sets the TCPNoDelay property.
		 * @throws SysException
		 */
		void SetTCPNoDelay (bool value);

		/**
		 * @brief Sets the ReuseAddress property.
		 * @throws SysException
		 */
		void SetReuseAddress (bool value);

		/**
		 * @brief Sets the KeepAlive property.
		 * @throws SysException
		 */
		void SetKeepAlive (bool value);

		/**
		 * @brief Sets the ReceiveTimeout property.
		 * @throws SysException
		 */
		void SetReceiveTimeout (int timeoutMs);

		/**
		 * @brief Sets the SendTimeout property.
		 * @throws SysException
		 */
		void SetSendTimeout (int timeoutMs);

		/**
		 * @brief Gets the ReceiveTimeout property.
		 * @throws SysException
		 */
		int GetReceiveTimeout ();

		/**
		 * @brief Gets the SendTimeout property.
		 * @throws SysException
		 */
		int GetSendTimeout ();

	protected:
		/**
		 * @brief Constructor.
		 */
		Socket (int hSocket, Address & addr) : _sock (hSocket, addr) {}
	private:
		SocketBase _sock;
	};

	/**
	 * @class ServerSocket
	 * @brief Holds a SocketBase object and represents a TCP/IP server socket.
	 *
	 * All the functions on this class are present on the SocketBase class and have
	 * the same functionality.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class ServerSocket
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param backLog Maximum length of the queue for pending connections.
		 *
		 * @throws SysException.
		 */
		ServerSocket (int port, int backLog = 5);

		/**
		 * @brief Sets the socket to a listening state.
		 */
		void Listen ();

		/**
		 * @brief Accepts a connection and creates a socket.
		 */
		SocketPtr Accept ();

		/**
		 * @brief Accepts a connection and creates a socket, waits the
		 * given timeout.
		 */
		SocketPtr Accept (int timeoutMs);

		/**
		 * @brief Returns local server address.
		 */
		Address const & GetAddress () const { return _sock.GetAddress (); }

		/**
		 * @brief Returns the port the socket is listening to.
		 */
		int GetLocalPort () const { return _sock.GetLocalPort (); }

		/**
		 * @brief Returns socket handle.
		 */
		int GetHandle () const { return _sock.GetHandle (); }
	
	private:
		SocketBase _sock;
		int		   _backLog;
	};
}

using namespace Common;

inline void Socket::Send (char const * buf, int bufSize, int flags)
{
	_sock.Send (buf, bufSize, flags);
}

inline void Socket::Send (std::string const & str, int flags)
{
	_sock.Send (str, flags);
}

inline int Socket::Receive (char * buf, int bufSize, int flags)
{
	return _sock.Receive (buf, bufSize, flags);
}

inline int Socket::ReceiveN (char * buf, int bufSize, int flags)
{
	return _sock.ReceiveN (buf, bufSize, flags);
}

inline void Socket::SetTCPNoDelay (bool value)
{
	_sock.SetTCPNoDelay (value);
}

inline void Socket::SetKeepAlive (bool value)
{
	_sock.SetKeepAlive (value);
}

inline void Socket::SetReceiveTimeout (int timeoutMs)
{
	_sock.SetReceiveTimeout (timeoutMs);
}

inline void Socket::SetSendTimeout (int timeoutMs)
{
	_sock.SetSendTimeout (timeoutMs);
}

inline void Socket::SetReuseAddress (bool value)
{
	_sock.SetReuseAddress (value);
}

inline int Socket::GetReceiveTimeout ()
{
	return _sock.GetReceiveTimeout ();
}

inline int Socket::GetSendTimeout ()
{
	return _sock.GetSendTimeout ();
}

inline Socket::Socket (Address & address)
{
	_sock.Connect (address);
}

inline Socket::Socket (std::string const & host, int port)
{
	_sock.Connect (host, port);
}
#endif
