
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

#ifndef __EVENT_H__
#define __EVENT_H__

// Local includes
#include "Utils.h"

// C++ includes
#include <iostream>
#include <stdio.h>
#include <string>

namespace Common {
	/**
	 * @class Event
	 * @brief Encapsulates information to record an event.
	 *
	 * This information will be sent to the Analyzer, who will do the
	 * actual recording of the event attributes.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2004
	 */
	
	// ------------------------------------------------------------
	class Event
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param timestamp Time stamp when the event was initialized.
		 * @param eventId Id of the event.
		 * @param place Part on the program where it'll take place. {EventEntry, EventExit}
		 * @param tid Task id.
		 * @param paramCount Number of parameters.
		 * @param machine String representing the machine where the event takes place.
		 */
		Event (long64_t timestamp, int eventId, EventPlace & place, int tid, int paramCount,
				std::string const & machine)
		: _timestamp (timestamp), _place (place), _eventId (eventId),
				_tid (tid), _paramCount (paramCount), _machine (machine)
		{
			Syslog::Debug ("TIMESTAMP: %llu", _timestamp);
			Syslog::Debug ("PLACE: %d", _place);
			Syslog::Debug ("Event ID: %d", _eventId);
			Syslog::Debug ("Task ID: %d", _tid);
			Syslog::Debug ("Machine: %s", _machine.c_str ());
			Syslog::Debug ("ParamCount: %d", _paramCount);
		}

		/**
		 * @brief Destructor.
		 */
		~Event (){}

		/**
		 * @brief Returns timestamp.
		 */
		long64_t GetTimestamp () const { return _timestamp; }

		/**
		 * @brief Returns place {EventEntry, EventExit}.
		 */
		int GetPlace () { return _place; }

		/**
		 * @brief Returns event id.
		 */
		int GetEventId () const { return _eventId; }

		/**
		 * @brief Returns tid attribute.
		 */
		int GetTid () { return _tid;}

		/**
		 * @brief Returns count of the parameters.
		 */
		int GetParamCount () { return _paramCount;}
	
		/**
		 * @brief Returns name of the machine.
		 */
		std::string const & GetMachine () { return _machine; }

	private:
		// event attributes
		long64_t 		_timestamp;
		int 			_eventId;
		EventPlace	&	_place;
		int 			_paramCount;
		int				_tid;
		std::string const _machine;
	};
}

#endif
