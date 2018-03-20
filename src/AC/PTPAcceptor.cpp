
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
// PTPAcceptor.cpp
// 
// PTPAcceptor - waits for and accepts comming tuning events
// Communicator module
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#include "PTPAcceptor.h"
#include "PTPHandler.h"
#include "Syslog.h"

PTPAcceptor::PTPAcceptor (Reactor & reactor, TaskManager & tm, int port)
	: _socket (port), _reactor (reactor), _tm (tm)
{	
	_socket.Listen ();
	// register with reactor
	Syslog::Debug ("PTP accepted registering in reactor");
	_reactor.Register (*this);
}

void PTPAcceptor::HandleInput ()
{
	Syslog::Debug ("Accepting PTP connection");
	SocketPtr clientSocket = _socket.Accept ();
	// create handler to handle this connection
	PTPHandler handler (clientSocket, _tm);
	handler.HandleInput ();
}
