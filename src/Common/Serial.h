
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

#ifndef __SERIAL_H__
#define __SERIAL_H__

// Local includes
#include "Types.h"

// C++ includes
#include <string>

namespace Common {
	/**
	 * @class DeSerializer
	 * @brief Abstract class, recovers serialized data from a stream.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class DeSerializer
	{
	public:

		/**
		 * @brief Reads byte value from the stream.
		 */
		virtual byte_t GetByte () = 0;

		/**
		 * @brief Reads char value from the stream.
		 */
		virtual char_t GetChar () = 0;

		/**
		 * @brief Reads bool value from the stream.
		 */
		virtual bool_t GetBool () = 0;

		/**
		 * @brief Reads short value from the stream.
		 */
		virtual short_t GetShort () = 0;

		/**
		 * @brief Reads int value from the stream.
		 */
		virtual int_t GetInt () = 0;

		/**
		 * @brief Reads long value from the stream.
		 */
		virtual long_t GetLong () = 0;

		/**
		 * @brief Reads double value from the stream.
		 */
		virtual double_t GetDouble () = 0;

		/**
		 * @brief Reads string value from the stream.
		 */
		virtual std::string GetString () = 0;

		/**
		 * @brief Reads data directly from the stream.
		 */
		virtual void GetBuffer (char * buffer, int bufferSize) = 0;
	};

	/**
	 * @class Serializer
	 * @brief Abstract class, prepares objects to be passed on a stream.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class Serializer
	{
	public:

		/**
		 * @brief Adds the size of a serialized byte.
		 */
		virtual void PutByte (byte_t b) = 0;

		/**
		 * @brief Adds the size of a serialized char.
		 */
		virtual void PutChar (char_t c) = 0;

		/**
		 * @brief Adds the size of a serialized bool.
		 */
		virtual void PutBool (bool_t b) = 0;

		/**
		 * @brief Adds the size of a serialized short.
		 */
		virtual void PutShort (short_t s) = 0;

		/**
		 * @brief Adds the size of a serialized int.
		 */
		virtual void PutInt (int_t i) = 0;

		/**
		 * @brief Adds the size of a serialized long.
		 */
		virtual void PutLong (long_t l) = 0;

		/**
		 * @brief Adds the size of a serialized double.
		 */
		virtual void PutDouble (double_t d) = 0;

		/**
		 * @brief Adds the size of a serialized string.
		 */
		virtual void PutString (std::string const & str) = 0;

		/**
		 * @brief Adds the size of a serialized buffer.
		 */
		virtual void PutBuffer (char const * buffer, int bufferSize) = 0;
	};

	/**
	 * @class Serializable
	 * @brief Abstract class, makes an object able to be passed
	 * through a stream using Serializer and DeSerializer objects.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class Serializable
	{
	public:


		/**
		 * @brief Destructor
		 */
		virtual ~Serializable () {}

		/**
		 * @brief Sends the message header.
		 */
		virtual void Serialize (Serializer & out) const = 0;

		/**
		 * @brief Receives the message header.
		 */
		virtual void DeSerialize (DeSerializer & in) = 0;
	};
}

#endif
