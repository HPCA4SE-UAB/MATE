
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
// reactor.cpp
// 
// Reactor implementation
//
//----------------------------------------------------------------------

#include "Reactor.h"
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>

using namespace Common;

void HandlerMap::Add (int handle, EventHandler * handler)
{
	// insert new pair (handle, handler)
	Pair pair = _map.insert (Map::value_type (handle, handler));
	
	// check if key is unique
	//if (!pair.second)
	//	throw "Key is not unique in this section";	
}

EventHandler * HandlerMap::Get (int handle)
{
	Map::const_iterator iter = _map.find (handle);
	if (iter != _map.end ())    // value exists
	{
		return (*iter).second;
	}
	// not found
	return 0;
}

// ---------------------------------------------------------------
void EventDemultiplexer::AddHandle (int handle)
{
	// make sure the handle is not yet registered
	assert (!FD_ISSET (handle, &_readHandles));
	// register new handle
	FD_SET (handle, &_readHandles);
	// track handle with maximum value
	if (handle > _maxHandle)
		_maxHandle = handle;
}

void EventDemultiplexer::RemoveHandle (int handle)
{
	// make sure the handle is registered
	assert (FD_ISSET (handle, &_readHandles));
	// unregister handle
	FD_CLR (handle, &_readHandles);
}

// timeout can be:
// - 0 (NULL) - check and block in forever loop
// - object pointer with value of:
//		 0 	- checks but does not block
//		 > 0 	- wait specified time for events

// returns status code:
// - the total number of socket handles that are ready 
//   and contained in the copy fd_set structure, 
// - zero if the time limit expired
int EventDemultiplexer::Select (TimeValue * timeout)
{	
	struct timeval * t = 0;
	if (timeout != 0)
	{
		t = *timeout;
	}	
	// copy handlers bit mask
	_copy = _readHandles;
	//Syslog::Debug ("EventDemultiplexer::select");
	int numEvents = ::select (_maxHandle+1, &_copy, 0, 0, t);
	//Syslog::Debug ("EventDemultiplexer num events: %d\n", numEvents);
	if (numEvents == -1)
	{
		Syslog::Debug ("num events: %d, error: %d, message: %s", numEvents, errno, strerror (errno));
		throw SysException ("EventDemultiplexer: Select failed");
	}
	return numEvents;
}

bool EventDemultiplexer::IsHandleActivated (int handle) const
{
	return FD_ISSET (handle, &_copy);
}

// ----------------------------------------------------------------
void Reactor::Register (EventHandler & handler) 
{	
	int handle = handler.GetHandle ();
	// register new handle
	_demux.AddHandle (handle);
	_handlers.Add (handle, &handler);
	Syslog::Debug ("Reactor -> register handler");
}

void Reactor::UnRegister (EventHandler & handler) 
{	
	int handle = handler.GetHandle ();
	// register new handle
	_demux.RemoveHandle (handle);
	//_handlers.Remove (handle, &handler);
}

void Reactor::HandleEvents (TimeValue * timeout) 
{
	//Syslog::Debug ("Reactor::HandleEvents");
	while (true)
	{
		//Syslog::Debug ("inside Reactor::HandleEvents");
		int numEvents = _demux.Select (timeout);
		if (numEvents == 0)
		{
			//Syslog::Debug ("reactor -> 0 events have come");
			// timeout has occurred
			break;
		}
		Syslog::Debug ("Reactor -> dispatching events");
		Dispatch (numEvents);
	}
}

void Reactor::Dispatch (int numEvents) 
{
	int dispEvents = 0;
	for (int h=0; h<=_demux.GetMaxHandle (); ++h)
	{
		if (_demux.IsHandleActivated (h))
		{
			// get handler and dispatch event
			EventHandler * handler = _handlers.Get (h);
			assert (handler != 0);
			handler->HandleInput ();
			++dispEvents;
			if (dispEvents == numEvents)
				break;
		}
	}
}

