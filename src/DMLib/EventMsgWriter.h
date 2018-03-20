
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

#ifndef __ECPEVENTMSG_H__
#define __ECPEVENTMSG_H__

// Local includes
#include "ECPMsg.h"
#include "ByteStream.h"
#include "NetSer.h"

namespace DMLib {
	/**
	 * @class EventMsgWriter
	 * @brief Creates EventMsg objects.
	 *
	 * Loads the specifications of an EventMsg object and prepares it. Once
	 * it's been prepared it returns the object using the CloseEvent method.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2001
	 */

	// ------------------------------------------------------------
	// Class definition
	class EventMsgWriter
	{
	public:

		/**
		 * @brief Constructor.
		 */
		EventMsgWriter ()
			:  _stream (1024 * 16),
			  _ser (_stream)
		{}

		/**
		 * @brief Destructor.
		 */
		~EventMsgWriter () {}

		/**
		 * @brief Open the event and sets its specifications.
		 *
		 * @param timestamp Timestamp when the event occurs.
		 * @param eventId Id of the event.
		 * @param place Place where the event is located
		 * {instrUnknown, ipFuncEntry,	ipFuncExit}.
		 * @param paramCount Number of parameters.
		 */
		void OpenEvent (long_t timestamp, int eventId,
				InstrPlace place, int paramCount);

		/**
		 * @brief Adds an integer parameter to the event.
		 */
		void AddIntParam (int value);

		/**
		 * @brief Adds a float parameter to the event.
		 */
		void AddFloatParam (float value);

		/**
		 * @brief Adds a double parameter to the event.
		 */
		void AddDoubleParam (double value);

		/**
		 * @brief Adds a char parameter to the event.
		 */
		void AddCharParam (char c);

		/**
		 * @brief Adds a string parameter to the event.
		 */
		void AddStringParam (std::string const & s);

		/**
		 * @brief Closes the event and returns the object.
		 */
		EventMsg const & CloseEvent ();

	private:
		ByteStream	  	  _stream;
		NetworkSerializer _ser;
		EventMsg	  	  _event;
	};
}

#endif
