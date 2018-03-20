
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

#ifndef __PTPMSGHEADER_H__
#define __PTPMSGHEADER_H__

// Local includes
#include "Serial.h"
#include "Utils.h"

// C++ includes
#include <stdio.h>

namespace Common {
	/**
	 * @class PTPMsgHeader
	 * @brief Represents header of a PTPMessage object.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class PTPMsgHeader : public Serializable
	{
	public:
	
		/**
		 * @brief Constructor.
		 */
		PTPMsgHeader ()
		{
			_magic = -1;
			_version = -1;
			_type = PTPunknown;
			_dataSize = -1;
			SetHeaderSize ();
		}

		/**
		 * @brief Sends the message header.
		 */
		void Serialize (Serializer & out) const;
	
		/**
		 * @brief Receives the message header.
		 */
		void DeSerialize (DeSerializer & in);

		/**
		 * @brief Returns magic attribute.
		 */
		int GetMagic () const { return _magic; }

		/**
		 * @brief Returns version attribute.
		 */
		int GetVersion () const { return _version; }

		/**
		 * @brief Returns the type of the message.
		 */
		PTPMsgType GetType () const { return _type; }

		/**
		 * @brief Returns data size.
		 */
		int GetDataSize () const { return _dataSize; }

		/**
		 * @brief Returns header size.
		 */
		int GetHeaderSize () const { return _headerSize; }

		/**
		 * @brief Sets the magic attribute.
		 */
		void SetMagic (int magic)
		{
			_magic = magic;
		}

		/**
		 * @brief Sets the version attribute.
		 */
		void SetVersion (int version)
		{
			_version = version;
		}

		/**
		 * @brief Sets the type of the message.
		 */
		void SetMsgType (PTPMsgType type)
		{
			_type = type;
		}

		/**
		 * @brief Sets data size.
		 */
		void SetDataSize (int size)
		{
			_dataSize = size;
		}

		/**
		 * @brief Updates header size.
		 */
		void SetHeaderSize ()
		{
			_headerSize = sizeof (_magic)
						+ sizeof (_version)
						+ sizeof (_type)
						+ sizeof (_dataSize)
						+ sizeof (_headerSize);
		}

	private:
		// header attributes
		int		 	_magic;
		int 		_version;
		PTPMsgType	_type;
		int 		_dataSize;
		int 		_headerSize;
	};
}

#endif
