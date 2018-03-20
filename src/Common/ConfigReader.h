
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

#ifndef __CONFIGREADER_H__
#define __CONFIGREADER_H__

// Local includes
#include "ConfigException.h"

// C++ includes
#include <string>
#include <fstream>
#include <ctype.h>
#include <string.h>
#include <iostream>

namespace Common {

	class Config;

	/**
	 * @class ConfigReader
	 * @brief Abstract class, generates Config objects from reading sources.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2000
	 */

	// ------------------------------------------------------------
	class ConfigReader
	{
	public:
		/**
		 * @brief Constructor.
		 */
		ConfigReader() {};

		virtual Config Read() = 0;

	protected:
		/**
		 * @brief Loads the information of the line into the Config object.
		 */
		void AnalyzeLine (Config& config, std::string const & line);

	private:
		void ReadSection (Config& config, std::string const & line);
		void ReadKeyValue (Config& config, std::string const & line);
		// return length of a given string after right trim
		std::string TrimStr (std::string str);

	private:
		std::string		_section;	// current section name
		std::string		_key;		// current key name
		std::string		_value;		// current value
	};

	/**
	 * @class FileConfigReader
	 * @brief Parses the content of a file into a Config object.
	 *
	 * @brief Extends ConfigReader
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Noel De Martin, 2011
	 */

	// ------------------------------------------------------------
	class FileConfigReader : public ConfigReader
	{
		enum { MaxLineLen = 1024 };

	public:
		/**
		 * @brief Constructor.
		 *
		 * @param fileName Path of the file to read.
		 *
		 * @throws ConfigException
		 */
		FileConfigReader (std::string const & fileName)
		: _ifs (fileName.c_str ())
		{
			if (!_ifs)
				throw ConfigException ("Cannot open configuration file", fileName);
		}

		/**
		* @brief Parses the configuration of the file into a Config
		* object.
		*
		* @throws ConfigException
		*/
		Config Read ();

	private:
		std::ifstream	_ifs;
	};
}

#endif
