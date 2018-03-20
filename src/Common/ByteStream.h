
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

#ifndef __BYTESTREAM_H__
#define __BYTESTREAM_H__

// Local includes
#include "OutputStream.h"

namespace Common {
	/**
	 * @class ByteStream
	 * @brief Stores stream of bytes.
	 *
	 * @extends OutputStream
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class ByteStream : public OutputStream
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param buf Intern buffer to be used.
		 * @param bufSize Size of the intern buffer.
		 */
		ByteStream (char * buf, size_t bufSize)
			: _buf(buf), _size (bufSize), _count (0)
		{
			_buf[0] = '\0';
		}

		/**
		 * @brief Constructor.
		 * Creates an intern buffer.
		 *
		 * @param bufSize Size of the intern buffer.
		 */
		ByteStream (size_t bufSize)
			: _size (bufSize), _count (0)
		{
			_buf = (char *) malloc(sizeof(char) * _size);
			_buf[0] = '\0';
		}

		/**
		 * @brief Adds the content of the buffer to the stream.
		 *
		 * @param buf Buffer to read.
		 * @param bufSize Size of the given buffer.
		 */
		void Write (char const * buf, size_t bufSize);
	
		/**
		 * @brief Returns pointer to the intern buffer.
		 *
		 * When used the returned pointer should always use the GetDataSize()
		 * method to iterate over the buffer.
		 *
		 * @return Read-only pointer to the intern buffer.
		 */
		char const * GetData () const { return _buf; }

		/**
		 * @brief Returns size of the stream.
		 * @return Buffer size.
		 */
		size_t GetDataSize () const { return _count; }

		/**
		 * @brief Clears the stream.
		 */
		void Reset ();

	private:
		char * _buf;
		size_t _size;
		size_t _count;
	};
}

#endif
