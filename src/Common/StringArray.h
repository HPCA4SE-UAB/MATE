
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

#ifndef __STRINGARRAY_H__
#define __STRINGARRAY_H__

// C++ includes
#include <string.h>
#include <assert.h>
#include <iostream>

namespace Common {
	/**
	 * @class StringArray
	 * @brief Container of strings.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class StringArray
	{
	public:

		/**
		 * @brief Constructor.
		 * @param size Size of the array, default 0.
		 */
		StringArray (int size = 0);

		/**
		 * @brief Destructor.
		 */
		~StringArray ();

		/**
		 * @brief Adds a string to the array.
		 */
		void AddString (char const * s);

		/**
		 * @brief Increments max size of the array.
		 */
		void Grow (int newSize);
	
		/**
		 * @brief Returns number of strings currently stored.
		 */
		int GetCount () const {	return _count; }
	
		/**
		 * @brief Returns max size of the array.
		 */
		int GetSize () const { return _size; }

		/**
		 * @brief Returns string stored on the given position.
		 */
		char const * GetString (int idx) const;

		/**
		 * @brief Returns a pointer to the actual array.
		 */
		char ** GetAccess () const	{ return _arr; }

		/**
		 * @brief Writes current state of the array on the standard output.
		 */
		void Dump () const;

	private:
		char ** _arr;
		int		_size;
		int		_count;
	};
}

#endif
