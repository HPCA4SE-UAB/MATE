
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

#ifndef __CONFIGMAP_H__
#define __CONFIGMAP_H__

// note: get rid of warning C4786: identifier was truncated 
// to '255' characters...
#pragma warning (disable: 4786)

// Local includes
#include "ConfigException.h"

// C++ includes
#include <string>
#include <map>
#include <assert.h>
#include <iostream>

namespace Common {
	/**
	 * @class ConfigMap
	 * @brief Contains and manages a collection of Config objects.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2000
	 */
	
	// ------------------------------------------------------------
	class ConfigMap
	{
	
	public:
		class Iterator;
		friend class Iterator;

	private:
		typedef std::map<std::string, std::string> StringMap;
		typedef std::pair<StringMap::iterator, bool> Pair;

	public:

		/**
		 * @brief Constructor.
		 */
		ConfigMap () {}

		/**
		 * @brief Adds a new value to the map.
		 *
		 * If the entry already exists on the map returns false.
		 *
		 * @param section Section of the new entry.
		 * @param key Key of the new entry.
		 * @param value Value of the new entry.
		 *
		 * @return True if the insertion was successful, false otherwise.
		 */
		bool Add (std::string const & section,
				std::string const & key,
				std::string const & value);

		/**
		 * @brief Returns a requested value on the map.
		 *
		 * @param section Section to find the value.
		 * @param key Key to find the value.
		 *
		 * @throws ConfigException
		 */
		std::string const & GetValue (std::string const & section,
				std::string const & key) const;

		/**
		 * @brief Looks for the entry specified by the parameters of the function.
		 *
		 * @param section Section to find the entry.
		 * @param key Key to find the entry.
		 *
		 * @return True if the entry was found, false otherwise.
		 */
		bool Contains (std::string const & section,
				std::string const & key) const;

		/**
		 * @brief Returns size of the map.
		 */
		int GetSize () const {	return _map.size (); }

	private:
		//Makes a new key to the map using the names of the section and the key.
		std::string MakeKey (std::string const & section,
				std::string const & key) const
		{
			return section + "." + key;
		}

	private:
		StringMap _map;

	public:

		/**
		 * @class Iterator
		 * @brief Iterates over a ConfigMap object.
		 *
		 * @version 1.0b
		 * @since 1.0b
		 * @author Ania Sikora, 2000
		 */

		// ------------------------------------------------------------
		class Iterator
		{
			typedef std::map<std::string, std::string> StringMap;

		public:

			/**
			 * @brief Constructor.
			 */
			Iterator (ConfigMap const & map)
				: _map (map._map), _iter (_map.begin ())
			{}

			/**
			 * @brief Indicates whether the iterator is pointing to the end
			 * of the map or not.
			 */
			bool AtEnd () const
			{
				return (_iter == _map.end ());
			}

			/**
			 * @brief The pointer increases a position on the map.
			 */
			void Next ();

			/**
			 * @brief Returns section of the current position.
			 */
			std::string GetSection () const;

			/**
			 * @brief Returns key of the current position.
			 */
			std::string GetKey () const;

			/**
			 * @brief Returns value of the current position.
			 */
			std::string const & GetValue () const;

		private:
			StringMap const &			_map;
			StringMap::const_iterator	_iter;
		};

	};
}
#endif
