
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

#ifndef __EVENTMAP_H__
#define __EVENTMAP_H__

// note: get rid of warning C4786: identifier was truncated 
// to '255' characters...
#pragma warning (disable: 4786)

// C++ includes
#include <string>
#include <map>
#include <assert.h>

namespace Common {
	/**
	 * @class EventMap
	 * @brief Contains and manages a collection of Event objects
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2001
	 */
	
	// ------------------------------------------------------------
	class EventMap
	{
	private:
		typedef std::map<std::string, int> StringMap;
		typedef std::pair<StringMap::iterator, bool> Pair;

	public:

		/**
		 * @brief Constructor.
		 */
		EventMap () {}

		/**
		 * @brief Adds a new event into the map.
		 * @throws EventException
		 */
		void Add (std::string const & name, int id);

		/**
		 * @brief Returns id of the given event.
		 * @throws EventException
		 */
		int GetId (std::string const & name) const;

		/**
		 * @brief Returns map size.
		 */
		int GetSize () const
		{
			return _map.size ();
		}

	private:
		StringMap _map;
	};
}

#endif
