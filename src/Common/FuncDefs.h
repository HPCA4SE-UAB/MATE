
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

#ifndef __FUNCDEFS_H__
#define __FUNCDEFS_H__

// note: get rid of warning C4786: identifier was truncated 
// to '255' characters...
#pragma warning (disable: 4786)

// C++ includes
#include <iostream>
#include <string>
#include <map>

namespace Common {
	/**
	 * @class FuncDef
	 * @brief Represents definition of the function to be traced.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */
	
	// ------------------------------------------------------------
	class FuncDef
	{
	public:
	
		/**
		 * @brief Constructor.
		 *
		 * @param name Name of the function
		 * @param paramFormat String denoting types of the parameters.
		 * 	S: String, I: Integer, P: Pointer.
		 * @param paramCount Number of parameters.
		 * @param funcId Function Id.
		 */
		FuncDef (std::string const & name, std::string const & paramFormat,
				 int paramCount, int funcId)
			: _name (name), _paramFormat (paramFormat), _paramCount (paramCount), _funcId (funcId)
		{}

		/**
		 * @brief Returns name of the function.
		 */
		std::string const & GetName () const
		{
			return _name;
		}

		/**
		 * @brief Returns format of the parameters.
		 */
		std::string const & GetParamFormat () const
		{
			return _paramFormat;
		}

		/**
		 * @brief Returns number of parameters used by the function.
		 */
		int GetParamCount () const
		{
			return _paramCount;
		}

		/**
		 * @brief Returns Id of the function.
		 */
		int GetFuncId () const
		{
			return _funcId;
		}

	private:
		std::string 	_name;
		std::string 	_paramFormat;
		int 			_paramCount;
		int 			_funcId;
	};
	
	/**
	 * @class FuncDefs
	 * @brief Creates and stores objects of the FuncDef class.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */
	
	// ------------------------------------------------------------
	class FuncDefs
	{
		typedef std::map<std::string, FuncDef> Map;
		typedef std::pair<Map::iterator, bool> Pair;

	public:
	
		/**
		 * @brief Constructor.
		 * @throws FuncDefException
		 */
		FuncDefs ();
	
		/**
		 * @brief Adds a FuncDef object.
		 *
		 * Uses the default constructor of FuncDef with the given parameters.
		 *
		 * @throws FuncDefException
		 */
		void Add (std::string const & funcName,
				  std::string const & paramFormat,
				  int paramCount, int funcId);

		/**
		 * @brief Returns a FuncDef object with the given name.
		 * @throws FuncDefException
		 */
		FuncDef const & Find (std::string const & name);

		/**
		 * @brief Returns number of FuncDef objects stored.
		 */
		int GetSize () const
		{
			return _map.size ();
		}

	private:
		Map _map;
	};
}

#endif
