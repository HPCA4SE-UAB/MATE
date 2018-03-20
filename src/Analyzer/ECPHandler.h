
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
// EventCollHandler.h
// 
// EventCollHandler - handles coming events
// 					one handler for one process (client)
//
// Anna Sikora, UAB, 2002-2010
//
//----------------------------------------------------------------------

#ifndef ECPHANDLER_H
#define ECPHANDLER_H

#include <string>
#include "ECPProtocol.h"
//#include "ECPAcceptor.h"
#include "Service.h"
#include "EventCollector.h"

class EventCollector;//Defined in EventCollector.h
class Service;//Defined in Service.h

/**
 * @brief Encapsulates data structures and methods to handle incoming event
 * collector inputs.
 */
class ECPHandler : public EventHandler {
	public:
		/**
		 * @brief Constructor.
		 */
		ECPHandler(SocketPtr & socket, EventCollector * collector)
			: _socket (socket), _registered (false),
			 _collector (collector), _service (0), _tid (0)
		{}

		/**
		 * @brief Not implemented (here for compatibility reasons)
		 */
		void Remove();

		/**
		 * @brief Reads an incoming message and handles it depending on its type.
		 * First reads a message from the socket, then creates the proper type of
		 * message and the calls the onEvent method of the listener of the events
		 * (if any).
		 */
		void HandleInput();

		/**
		 * @brief Handler getter.
		 * @return Reference to the handler object
		 */
		int GetHandle() {
			return _socket->GetHandle ();
		}

		/**
		 * @brief Service setter.
		 * @param service	Reference to the service.
		 */
		void SetService(Service * service){
			_service = service;
		}

	private:
		/**
		 * @brief Not implemented (here for compatibility reasons)
		 */
		void PrepareEvent(EventMsg * em);

	private:
		SocketPtr		_socket; // this handler owns this sockets !
		bool			_registered;
		EventCollector * _collector;
		Service 	 *	_service;
		int 			_tid;
		std::string		_host;
};
#endif

