
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

#ifndef __NETSER_H__
#define __NETSER_H__

// Local includes
#include "Serial.h"
#include "Types.h"
#include "OutputStream.h"
#include "Syslog.h"

// C++ includes
#include <string.h>

namespace Common {
	/**
	 * @class NetworkSerializer
	 * @brief Puts serialized data into an OutputStream object.
	 *
	 * @extends Serializer
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class NetworkSerializer: public Serializer
	{
	public:

		/**
		 * @brief Constructor.
		 * @param stream Stream where the serialized data will be written.
		 */
		NetworkSerializer (OutputStream & stream)
			: _stream (stream)
		{}

		/**
		 * @brief Puts a long into the stream.
		 */
		void PutLong (long_t l);

		/**
		 * @brief Puts a double into the stream.
		 */
		void PutDouble (double_t d)
		{
			 _stream.Write (reinterpret_cast<char *> (&d), sizeof (double_t));
		}

		/**
		 * @brief Puts a boolean into the stream.
		 */
		void PutBool (bool_t b)	{
			PutByte (b? 0: 1);
		}

		/**
		 * @brief Puts a short into the stream.
		 */
		void PutShort (short_t s);

		/**
		 * @brief Puts a byte into the stream.
		 */
		void PutByte (byte_t b)
		{
			 _stream.Write (reinterpret_cast<char *> (&b), sizeof (byte_t));
		}

		/**
		 * @brief Puts a char into the stream.
		 */
		void PutChar (char_t c)
		{
			 _stream.Write (reinterpret_cast<char *> (&c), sizeof (char_t));
		}

		/**
		 * @brief Puts a string into the stream.
		 */
		void PutString (std::string const & str);

		/**
		 * @brief Puts an integer long into the stream.
		 */
		void PutInt (int_t i);

		/**
		 * @brief Puts a buffer into the stream.
		 */
		void PutBuffer (char const * buffer, int bufferSize)
		{
			_stream.Write (buffer, bufferSize);
		}

	private:
		OutputStream & _stream;
	};


	/**
	 * @class NetworkDeSerializer
	 * @brief Extracts serialized data from an istream object.
	 *
	 * @extends DeSerializer
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class NetworkDeSerializer : public DeSerializer
	{
	public:

		/**
		 * @brief Constructor.
		 */
		NetworkDeSerializer (std::istream & stream)
			: _stream (stream)
		{}

		/**
		 * @brief Returns istream object where the data is serialized.
		 */
		std::istream & GetStream () { return _stream; }

		/**
		 * @brief Reads long value from the stream.
		 */
		long_t GetLong ();

		/**
		 * @brief Reads double value from the stream.
		 */
		double_t GetDouble ();

		/**
		 * @brief Reads bool value from the stream.
		 */
		bool_t GetBool () { return GetByte(); }

		/**
		 * @brief Reads short value from the stream.
		 */
		short_t GetShort ();

		/**
		 * @brief Reads byte value from the stream.
		 */
		byte_t GetByte ();

		/**
		 * @brief Reads char value from the stream.
		 */
		char_t GetChar ();

		/**
		 * @brief Reads string value from the stream.
		 */
		std::string GetString ();

		/**
		 * @brief Reads int value from the stream.
		 */
		int_t GetInt ();

		/**
		 * @brief Reads data directly from the stream.
		 */
		void GetBuffer (char * buffer, int bufferSize);

	private:
		std::istream & _stream;
	};

	/**
	 * @class CountingSerializer
	 * @brief Stores the size of serialized data.
	 *
	 * @extends Serializer
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class CountingSerializer: public Serializer
	{
	public:

		/**
		 * @brief Constructor.
		 */
		CountingSerializer ()
			: _size (0)
		{}

		/**
		 * @brief Returns size of the serialized data.
		 */
		int_t GetSize () const { return _size; }

		/**
		 * @brief Adds the size of a serialized long.
		 */
		void PutLong (long_t l)
		{
			_size += sizeof (l);
		}

		/**
		 * @brief Adds the size of a serialized double.
		 */
		void PutDouble (double_t d)
		{
			_size += sizeof (d);
		}

		/**
		 * @brief Adds the size of a serialized boolean.
		 */
		void PutBool (bool_t b)
		{
			_size += sizeof (byte_t);
		}

		/**
		 * @brief Adds the size of a serialized short.
		 */
		void PutShort (short_t s)
		{
			_size += sizeof (s);
		}

		/**
		 * @brief Adds the size of a serialized byte.
		 */
		void PutByte (byte_t b)
		{
			_size += sizeof (b);
		}

		/**
		 * @brief Adds the size of a serialized char.
		 */
		void PutChar (char_t c)
		{
			_size += sizeof (c);
		}

		/**
		 * @brief Adds the size of a serialized string.
		 */
		void PutString (std::string const & str)
		{
			_size += sizeof (int_t) + str.size();
		}

		/**
		 * @brief Adds the size of a serialized integer.
		 */
		void PutInt (int_t i)
		{
			_size += sizeof (i);
		}

		/**
		 * @brief Adds the size of a serialized buffer.
		 */
		void PutBuffer (char const * buffer, int bufferSize)
		{
			_size += bufferSize;
		}

	private:
		int_t	_size;
	};
}

#endif
