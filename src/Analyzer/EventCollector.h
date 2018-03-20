
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
// EventCollector.h
// 
// EventCollector - active object that collects incoming ECP events
//
// Anna Sikora, UAB, 2004-2010
//
//----------------------------------------------------------------------

#ifndef EVENTCOLLECTOR_H
#define EVENTCOLLECTOR_H

#include "ActiveObject.h"
#include "Reactor.h"
#include "Syslog.h"
#include "ECPMsg.h"
#include "Socket.h"
#include "EventHandler.h"
#include "Service.h"
#include "ECPHandler.h"
#include <strstream>

/**
 * @brief Provides an interface for event listeners, which consist in
 * methods to respond to events and errors.
 */
class EventListener {
	public:
		/**
		 * @brief Function which is triggered when an event happens.
		 *
		 * @param msg	Message that contains the event data.
		 */

		virtual void OnEvent(ECPMessage * msg) = 0;

		/**
		 * @brief Function which is triggered when a fatal error happens.
		 */
		virtual void OnFatalError() = 0;
};

class EventCollector;//Will be defined later

/**
 * @class ECPAcceptor
 * @brief Event Acceptor class that collects incoming ECP events and prepares their correspondent handler.
 */
class ECPAcceptor : public EventHandler {
	public:
		/**
		 * @brief Constructor, starts listening to the socket and registers
		 * itself in the reactor.
		 * @param reactor	** Reactor of the application??? **
		 * @param port		Socket port.
		 */
		ECPAcceptor(Reactor & reactor, int port);

		/**
		 * @brief Destructor, unregister the object from the reactor.
		 */
		~ECPAcceptor()
		{
			_reactor.UnRegister (*this);
		}

		/**
		 * @brief When a new connection is accepted, prepares a handler for
		 * it. It is registered in the reactor and added to the service.
		 */
		void HandleInput();
	
		/**
		 * @return Reference to the handler object
		 */
		int GetHandle()
		{
			return _socket.GetHandle ();
		}

		/**
		 * @brief Setter for the event collector.
		 * @param collector	Event collector to be set.
		 */
		void SetEventCollector(EventCollector * collector);

	private:
		ServerSocket 	_socket;
		Reactor	    &	_reactor;
		Service 	 	_service;
		EventCollector * _collector;
};

/**
 * @class EventCollector
 * @brief Processes the incoming event records from the DMLibs.
 * It is based on an active object (thread) that collects incoming ECP events
 * It stores a moving window of events incoming from different processes using
 * a pool of buffers. The maximum size of this event window can be configured
 * by the tunlets.
 */
class EventCollector : public ActiveObject {
	public:
		enum { DefaultPort};

		/**
		 * @brief Constructor, starts an execution thread.
		 * @param port	Acceptor port.
		 */
		EventCollector(int port = DefaultPort);

		/**
		 * @brief Destructor, stops the execution thread.
		 */
		~EventCollector();

		/**
		 * @brief Setter for the listener.
		 * @param listener	Listener to be set.
		 */
		void SetListener(EventListener * listener);

		/**
		 * @brief Getter for the listener.
		 * @return Listener of the event collector.
		 */
		EventListener * GetListener() { return _listener; };

		/**
		 * @brief Determines if the collector is aborted.
		 * @return The status of the collector.
		 */
		bool IsAborted() const { return _aborted; }
	protected:
		/**
		 * @brief Not implemented (here for compatibility reasons).
		 */
		void InitThread();

		/**
		 * @brief Runner of the execution thread, handles events until it dies.
		 */
		void Run();

		/**
		 * @brief Not implemented (here for compatibility reasons).
		 */
		void FlushThread();
	
		/**
		 * @brief Called when an exception is caught in the execution thread.
		 *
		 */
		void Fatal ();

	protected:
		EventListener * _listener;
		Reactor 	    _reactor;
		ECPAcceptor		_acceptor;
		bool			_aborted;
};
#endif
