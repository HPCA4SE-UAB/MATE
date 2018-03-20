
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

#ifndef __CONFIG_H__
#define __CONFIG_H__

// Local includes
#include "ConfigMap.h"
#include "ConfigReader.h"

// C++ includes
#include <iostream>
#include <string>
#include <stdlib.h>

namespace Common {
	/**
	 * @class Config
	 * @brief Manages a configuration of the system.
	 *
	 * The configuration is based on section and keys, and the format is
	 * the following:
	 * @code
	   [section]
	   key = value
	   key = value
	   ...

	   [newsection]
	   key = value
	   ...
	   @endcode
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2000
	 */

	// ------------------------------------------------------------
	class Config
	{
	public:
		class KeyIterator;
		friend class KeyIterator;
		friend class ConfigReader;
	
	public:
		/**
		 * @brief Constructor.
		 */
		Config () {}
	
		/**
		 * @brief Returns string value of the entry specified by the parameters.
		 *
		 * @param section Section to find the value.
		 * @param key Key to find the value.
		 *
		 * @throws ConfigException
		 */
		std::string const & GetStringValue (std::string const & section,
										std::string const & key) const
		{
			return _map.GetValue (section, key);
		}

		/**
		 * @brief Returns integer value of the entry specified by the parameters.
		 *
		 * @param section Section to find the value.
		 * @param key Key to find the value.

		 * @throws ConfigException
		 */
		int GetIntValue (std::string const & section,
					std::string const & key) const;


		/**
		 * @brief Returns integer value of the entry specified by the parameters.
		 *
		 * If the configuration doesn't contain the specified entry,
		 * returns the default value.
		 *
		 * @param section Section to find the value.
		 * @param key Key to find the value.
		 * @param defaultValue Value returned if the requested entry is not
		 * inside the configuration.
		 *
		 * @return Integer containing the requested value.
		 */
		int GetIntValue (std::string const & section,
				std::string const & key, int defaultValue) const
		{
			return (_map.Contains (section, key))?
					GetIntValue (section, key) :
					defaultValue;
		}

		/**
		 * @brief Returns boolean value of the entry specified by the parameters.
		 *
		 * @param section Section to find the value.
		 * @param key Key to find the value.
		 *
		 * @return Boolean containing the requested value.
		 *
		 * @throws ConfigException
		 */
		bool GetBoolValue (std::string const & section,
				std::string const & key) const;

		/**
		 * @brief Returns boolean value of the entry specified by the parameters.
		 *
		 * If the configuration doesn't contain the specified entry,
		 * returns the default value.
		 *
		 * @param section Section to find the value.
		 * @param key Key to find the value.
		 * @param defaultValue Value returned if the requested entry is not
		 * inside the configuration.
		 *
		 * @return Boolean containing the requested value.
		 */
		bool GetBoolValue (std::string const & section,
				std::string const & key, bool defaultValue) const
		{
			return (_map.Contains (section, key))?
				GetBoolValue (section, key) :
			    defaultValue;
		}

		/**
		 * @brief Finds an entry on the configuration.
		 *
		 * @param section Section to find the entry.
		 * @param key Key to find the entry.
		 *
		 * @return True if the entry was found, false otherwise.
		 */
		bool Contains (std::string const & section,
				std::string const & key) const
		{
			return _map.Contains (section, key);
		}

		/**
		 * @brief Returns an iterator of the keys inside the requested section.
		 *
		 * @param section Section requested.
		 *
		 * @return Iterator to the keys inside the section.
		 */
		KeyIterator GetKeys (std::string const & section) const
		{
			return KeyIterator (*this, section);
		}

		/**
		 * @brief Adds a new entry to the configuration.
		 *
		 * @param section Section of the new entry.
		 * @param key Key of the new entry.
		 * @param value Value of the new entry.
		 */
		void AddEntry (std::string const & section,
				std::string const & key,
				std::string const & value)
		{
			_map.Add (section, key, value);
		}

	private:
		ConfigMap _map;


	public:
		/**
		 * @class KeyIterator
		 * @brief Iterates over the keys of a Config object.
		 *
		 * @version 1.0b
		 * @since 1.0b
		 * @author Ania Sikora, 2000
		 */

		// ------------------------------------------------------------
		class KeyIterator
		{
		public:

			/**
			 * @brief Constructor.
			 */
			KeyIterator (Config const & config, std::string const & section)
				: _iter (config._map), _section (section)
			{
				if (!_iter.AtEnd ())
				{
					if (_section.compare (_iter.GetSection ()) != 0)
						Next ();
				}
			}

			/**
			 * @brief Indicates whether the iterator is pointing to the end
			 * of the map or not.
			 */
			bool AtEnd () const	{ return _iter.AtEnd (); }

			/**
			 * @brief The pointer increases a position on the config.
			 */
			void Next ();

			/**
			 * @brief Returns key of the current position.
			 */
			std::string GetKey () const;

			/**
			 * @brief Returns value of the current position.
			 */
			std::string const & GetValue () const;

			/**
			 * @brief Returns integer value of the current position.
			 */
			int GetIntValue () const;

		private:
			ConfigMap::Iterator _iter;
			std::string			_section;
		};

	};

	/**
	 * @class ConfigHelper
	 * @brief Static class that contains methods to manage Config objects.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Noel De Martin, 2011
	 */

	// ------------------------------------------------------------
	class ConfigHelper
	{

	public:

		/**
		 * @brief Returns a Config object loaded from the given file.
		 *
		 * @throws ConfigException
		 */
		static Config ReadFromFile(std::string const & fileName){
			return FileConfigReader(fileName).Read();
		}

		//To implement
		/*static void SaveToFile(std::string const & fileName, Config& config){
			FileConfigWritter writer(conf);
			writer.write(fileName);
		}*/

	};
}

#endif
