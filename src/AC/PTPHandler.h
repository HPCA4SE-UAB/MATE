
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
// PTPHandler.h
// 
// PTPHandler - handles coming solutions
// Communicator module
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#ifndef PTPHANDLER_H
#define PTPHANDLER_H

#include "PTPProtocol.h"
#include "PTPAcceptor.h"
#include "TaskManager.h"
/**
 * @class PTPHandler
 * @brief Manages the requests from the PTPAcceptor.
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class PTPHandler : public EventHandler
{
public:
	/**
	 * @brief Constructor
	 * @param socket Pointer to the socket used to get the input (request).
	 * @param tm Task manager that handles the task to which the request affects.
	 */
	PTPHandler (SocketPtr & socket, TaskManager & tm)
		: _socket (socket), _registered (false), _tm (tm)
	{}
	
	//not defined?
	//void Remove ();

	/**
	 * @brief Reads message from socket and handles the different kinds of
	 * requests that are received.
	 */
	void HandleInput ();
	
	/**
	 * @brief Getter of a handler for the variable _socket.
	 * @return Handle of the socket.
	 */
	int GetHandle ()
	{
		return _socket->GetHandle ();
	}
	

private:
	SocketPtr	_socket; // this handler owns this sockets !
	bool		_registered;
	TaskManager & _tm;
};

#endif
