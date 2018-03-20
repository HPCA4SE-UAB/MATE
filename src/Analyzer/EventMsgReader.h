
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

//----------------------------------------------------------------------
//
// EventMsgReader.h
// 
//
// Anna Sikora, UAB, 2004
//
// This code is used by Analyzer
//----------------------------------------------------------------------

#ifndef ECPMSG_H
#define ECPMSG_H

#include "NetSer.h"
#include "ECPMsgHeader.h"
#include "Utils.h"
#include <stdarg.h>
#include <strstream>
#include <cassert>

/**
 * @brief Provides methods for getting data from event messages.
 * The data structure that supports the class consist in the message to be
 * processed, a buffer to hold the data and a deserializer object to
 * reconstruct the information.
 *
 */
class EventMsgReader {
	public:
		/**
		 * @brief Constructor.
		 * @param msg input message.
		 */
		EventMsgReader (EventMsg const & msg)
			: _msg (msg),
			  _stream (msg.GetParamBuffer(), msg.GetParamBufSize()),
			  _deser (_stream)
		{
			//DumpValues();
		}

		/**
		 * @brief Getter of ParamCount.
		 * @return Number of parameters.
		 */
		int GetParamCount() const { return _msg.GetParamCount(); }

		/**
		 * @brief	getter of AttrType.
		 * @return 	Type of the attribute.
		 */
		AttrValueType GetAttrType() 
		{ 
			// return  (AttrValueType)_deser.GetByte(); 
			byte_t type = _deser.GetByte();
			return (AttrValueType) type;
		}

		/**
		 * @brief Gets an integer from the stream.
		 * @return Integer value.
		 */
		int GetIntValue();

		/**
		 * @brief Gets a float from the stream.
		 * @return Float value.
		 */
		float GetFloatValue();

		/**
		 * @brief Gets a double from the stream.
		 * @return Double value.
		 */
		double GetDoubleValue();

		/**
		 * @brief Gets a character from the stream.
		 * @return Character value.
		 */
		char GetCharValue();

		/**
		 * @brief Gets a short from the stream.
		 * @return Short value.
		 */
		short GetShortValue();

		/**
		 * @brief Get a string from the stream.
		 * @return String value.
		 */
		std::string GetStringValue();

		/**
		 * @brief Gets the value of each ECP event parameter.
		 * For each parameter checks the type and use the proper getter.
		 */
		void DumpValues();

	private:
		EventMsg const	  & _msg;
		std::istrstream 	_stream;
		NetworkDeSerializer _deser;
};

inline int EventMsgReader::GetIntValue() {
	return _deser.GetInt();
}

inline float EventMsgReader::GetFloatValue() {
	return (float)_deser.GetDouble();
}

inline double EventMsgReader::GetDoubleValue() {
	return _deser.GetDouble();
}

inline char EventMsgReader::GetCharValue() {
	return _deser.GetChar();
}

inline short EventMsgReader::GetShortValue() {
	return _deser.GetShort();
}

inline std::string EventMsgReader::GetStringValue() {
	return _deser.GetString();
}
#endif



