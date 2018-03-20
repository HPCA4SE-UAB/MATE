
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

#ifndef __REACTOR_H__
#define __REACTOR_H__


// note: get rid of warning C4786: identifier was truncated 
// to '255' characters...
#pragma warning (disable: 4786)

// Local includes
#include "TimeValue.h"
#include "EventHandler.h"
#include "SysException.h"

// C++ includes
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <map>
#include <sys/select.h>

namespace Common {
	/**
	 * @class HandlerMap
	 * @brief Contains and manages a collection of EventHandler objects.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class HandlerMap
	{
		typedef std::map<int, EventHandler*> Map;
		typedef std::pair<Map::iterator, bool> Pair;

	public:

		/**
		 * @brief Constructor.
		 */
		HandlerMap () {}

		/**
		 * @brief Adds the handler to the map.
		 */
		void Add (int handle, EventHandler * handler);

		/**
		 * @brief Returns the EventHandler object stored with the
		 * given handle.
		 */
		EventHandler * Get (int handle);

		/**
		 * @brief Returns map size.
		 */
		int GetSize () const { return _map.size ();	}

	private:
		Map _map;
	};

	/**
	 * @class EventDemultiplexer
	 * @brief Part of the reactor design pattern, takes requests coming
	 * from the reactor and passes them to different handlers.
	 *
	 * @version 1.0
	 * @since 1.0
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class EventDemultiplexer
	{
	public:

		/**
		 * @brief Constructor.
		 */
		EventDemultiplexer ()
			: _maxHandle (-1)
		{
			FD_ZERO (&_readHandles);
		}

		/**
		 * @brief Adds a new handle.
		 */
		void AddHandle (int handle);

		/**
		 * @brief Removes selected handle.
		 */
		void RemoveHandle (int handle);

		/**
		 * @brief Returns the number of socket handles ready or 0 if the time limit
		 * expired.
		 *
		 * @param timeout If the parameter is a TimeValue object, it will wait the
		 * object value for events. In the event that the value is 0 it will check without
		 * blocking. If the parameter is a 0 (not a TimeValue object, default value)
		 * it will check and block in a forever loop.
		 */
		int Select (TimeValue * timeout = 0);

		/**
		 * @brief Returns true if the given handle is activated,
		 * false otherwise.
		 */
		bool IsHandleActivated (int handle) const;

		/**
		 * @brief Returns value of the max handle.
		 */
		int GetMaxHandle () const { return _maxHandle; }

	private:
		fd_set	_readHandles;
		fd_set	_copy;	// contains active handles after select
		int		_maxHandle;
	};


	/**
	 * @class Reactor
	 * @brief Registers, removes and dispatches EventHandler objects.
	 *
	 * Uses reactor design pattern.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class Reactor
	{
	public:

		/**
		 * @brief Constructor.
		 */
		Reactor () {}

		/**
		 * @brief Registers a new EventHandler.
		 */
		void Register (EventHandler & handler);

		/**
		 * @brief Removes given EventHandler.
		 */
		void UnRegister (EventHandler & handler);

		/**
		 * @brief Runs event loop.
		 */
		void HandleEvents (TimeValue * timeout = 0);

		/**
		 * @brief Returns selected handler.
		 */
		EventHandler & GetHandler (int handle);

	private:
		void Dispatch (int numEvents);
	
	private:
		EventDemultiplexer 	_demux;
		HandlerMap			_handlers;
	};
}

#endif
