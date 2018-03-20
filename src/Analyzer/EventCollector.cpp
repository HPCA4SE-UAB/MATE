
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
// EventCollector.cpp
// 
// EventCollector - active object that collects incoming ECP events
//
// Anna Sikora, UAB, 2004-2010
//
//----------------------------------------------------------------------

#include "EventCollector.h"
#include "Syslog.h"
#include <stdlib.h>

ECPAcceptor::ECPAcceptor(Reactor & reactor, int port)
	: _socket(port), _reactor(reactor), _service(reactor), _collector(0)
{
	Syslog::Debug("[ECPAcceptor] Port: %d", port);
	_socket.Listen();
	_reactor.Register(*this); //Register with reactor
}

void ECPAcceptor::HandleInput() {
	//SocketPtr sPtr = _socket.Accept (5);
	Syslog::Debug("[Acceptor] Handling input");
	SocketPtr clientSocket = _socket.Accept();
	Syslog::Debug("[ECP] New connection");

	ECPHandler * handler = new ECPHandler(clientSocket, _collector);
	if (handler == NULL)
		throw "FAILED: create handler to handle this connection";

	Syslog::Debug("[Acceptor] Register this handler with reactor");
	_reactor.Register(*handler);
	_service.Add (handler); //Add handler to service
}

void ECPAcceptor::SetEventCollector(EventCollector * collector) {
	_collector = collector;
}

EventCollector::EventCollector(int port)
  : _listener(0), _acceptor(_reactor, port),  _aborted(false)
{
	//Start the thread
	_acceptor.SetEventCollector (this);
	Syslog::Info ("Starting event collector thread on port %d", port);
	Resume();
}

EventCollector::~EventCollector() {
	Syslog::Info ("Stopping event collector thread");
	Kill (); //Stop thread
}

void EventCollector::InitThread() {
	// NO_OP
}
void EventCollector::Run() {
	try {
		Syslog::Info ("EventCollector thread running...");	
		TimeValue time(1);
		while (!_isDying) {
			//Syslog::Info ("_reactor.HandleEvents");
			_reactor.HandleEvents(&time);
		}
		Syslog::Info("EventCollector thread exited");
	} catch (Exception & e) {
		Syslog::Fatal("Event Collector thread exception: %s, object: %s",
			e.GetErrorMessage().c_str(), e.GetObjectName().c_str());
		Fatal();
	}	
	catch (char const* s) {
		Syslog::Fatal("Event Collector thread exception: %s", s);
		Fatal();
	}
	catch (...) {
		Syslog::Fatal("Event Collector thread unexpected exception");
		Fatal();
	}
}

void EventCollector::FlushThread() {
	// NO_OP
}

void EventCollector::SetListener(EventListener * listener) {
	_listener = listener;
}

void EventCollector::Fatal() {
	_aborted = true;
	if (_listener != 0)
		_listener->OnFatalError();
}

