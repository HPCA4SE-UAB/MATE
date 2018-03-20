
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

//----------------------------------------------------------------------
//
// Utils.h
// 
// 1. constants corresponding to messages exchanged 
//		between Analyzer and Tuner/Tracer
// 2. util functions as CurrentTimeMillisec, CurrentTimeMicrosec
//
// Anna Sikora, UAB, 2003-2004
//
//----------------------------------------------------------------------

#ifndef __UTILS_H__
#define __UTILS_H__

// Local includes
#include "Serial.h"
#include "Syslog.h"
#include "Types.h"

// C++ includes
#include <sys/types.h>
#include <string>
#include <sys/time.h> /* timeval */

/**
 * @namespace Common
 */
namespace Common {
	// this could be removed
	enum TuningAction
	{
		taOneTimeFuncCall,
		taFuncParamChange,
		taVariableChange,
		taFuncCallReplace,
		taFuncCallAdd
	};

	enum PTPMsgType
	{
		PTPunknown,
		PTPLoadLibrary,
		PTPSetVariableValue,
		PTPReplaceFunction,
		PTPInsertFuncCall,
		PTPOneTimeFuncCall,
		PTPRemoveFuncCall,
		PTPFuncParamChange,
		PTPAddInstr,
		PTPRemoveInstr,
		PTPStartApp
	};
	
	//*************************************************
	enum InstrPlace
	{
		instrUnknown = -1,
		ipFuncEntry = 0,
		ipFuncExit
	};

	enum EventPlace
	{
		eventEntry = 1,
		eventExit = 2
	};

	typedef enum AttrSource
	{
		asFuncParamValue = 0,
		asVarValue,
		asFuncReturnValue,
		asConstValue,
		asFuncParamPointerValue,
		asSendMessageSize,	// this attr can be inserted only to the ENTRY of function pvm_send
		asRecvMessageSize,	// this attr can be inserted only to the EXIT of function pvm_recv
		asSenderTid,	// this attr can be inserted only to the EXIT of function pvm_recv or pvm_send
	};

	typedef enum AttrValueType
	{
		avtInteger = 0,
		avtShort,
		avtFloat,
		avtDouble,
		avtChar,
		avtString
	};
	
	/**
	 * @class Attribute
	 * @brief Contains the necessary information of an attribute to
	 * be inserted in a program.
	 *
	 * @extends Serializable
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class Attribute : public Serializable
	{
	public:

		/**
		 * @brief Copy constructor.
		 */
		Attribute (Attribute const & a)
			: source (a.source), type (a.type), id (a.id)
		{}

		/**
		 * @brief Constructor.
		 *
		 * Creates a default Attribute object of the integer type.
		 */
		Attribute ()
			: source (asFuncParamValue), type (avtInteger)
		{}

		/**
		 * @brief Sends the data serialized.
		 */
		void Serialize (Serializer & out) const
		{
			out.PutInt (source);
			out.PutInt (type);
			out.PutString (id);
		}
	
		/**
		 * @brief Gets the data deserialized.
		 */
		void DeSerialize (DeSerializer & in)
		{
			source = (AttrSource) in.GetInt ();
			type = (AttrValueType) in.GetInt ();
			id = in.GetString ();
		}

		/**
		 * @brief Returns the string of the source.
		 */
		string GetSourceString () const;

		/**
		 * @brief Returns the type of the attribute.
		 */
		string GetTypeString () const
		{
			return GetTypeString (type);
		}

		/**
		 * @brief Given a value of the enumerator AttrValueType returns
		 * the type in a string.
		 */
		static string GetTypeString (AttrValueType type);

		/**
		 * @brief Logs the information of the attribute on the System Log.
		 */
		void Dump () const
		{
			Syslog::Debug ("Attribute (source: %s, type: %s, id: %s)",
					GetSourceString ().c_str (), GetTypeString ().c_str (), id.c_str ());
		}

		AttrSource 		source; //source of attribute
		AttrValueType 	type;	// type of attribute value
		string			id;		// source-dependent object identifier
								// e.g variable name, func name, idx of func parameter
	};
	
	/**
	 * @class AttributeValue
	 * @brief Contains the vale of an attribute.
	 *
	 * @extends Serializable
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class AttributeValue : public Serializable
	{
	public:
		/**
		 * @brief Constructor.
		 */
		AttributeValue ();

		/**
		 * @brief Copy constructor.
		 */
		AttributeValue (AttributeValue const & av);

		/**
		 * @brief Assignation operator, copies content of the given object.
		 */
		void operator= (AttributeValue const & av);

		union
		{
			int 		intValue;
			short 		shortValue;
			float 		floatValue;
			double 		doubleValue;
			char   		charValue;
		};


		/**
		 * @brief Returns type of the attribute.
		 */
		AttrValueType GetType () const { return type; }

		/**
		 * @brief Sets the type of the attribute.
		 */
		 void SetType (AttrValueType attrType) { type = attrType; }
		 
		 /**
		 * @brief Sets the string value of the attribute.
		 */
		 void SetStrValue (std::string attrStrValue) { strValue = attrStrValue; }

		/**
		 * @brief Gets the integer value.
		 * @throws Exception
		 */
		int GetIntValue () const;

		/**
		 * @brief Gets the string value.
		 * @throws Exception
		 */
		std::string GetStringValue () const;

		/**
		 * @brief Gets the short value.
		 * @throws Exception
		 */
		short GetShortValue () const;

		/**
		 * @brief Gets the float value.
		 * @throws Exception
		 */
		float GetFloatValue () const;

		/**
		 * @brief Gets the double value.
		 * @throws Exception
		 */
		double GetDoubleValue () const;

		/**
		 * @brief Gets the char value.
		 * @throws Exception
		 */
		char GetCharValue () const;

		/**
		 * @brief Gets the pointer to the buffer.
		 */
		void * GetValueBuffer ();

		/**
		 * @brief Gets the size of the value in memory.
		 */
		int GetSize () const;

		/**
		 * @brief Returns the value in a string.
		 */
		string ToString () const;

		/**
		 * @brief Sends the data serialized.
		 */
		void Serialize (Serializer & out) const;

		/**
		 * @brief Gets the data deserialized.
		 */
		void DeSerialize (DeSerializer & in);

	private:
		void CheckType (AttrValueType requested) const;

		AttrValueType type;
		std::string strValue;
	};
	
	//*************************************************
	
	typedef enum TuningSync
	{
		tsBreakpoint,
		tsNone
	};
	
	/**
	 * @class Breakpoint
	 * @brief Denotes place in a function (on the entry or at the end).
	 *
	 * @extends Serializable
	 *
	 * @version 1.0
	 * @since 1.0
	 * @author Ania Sikora, 2002
	 */
	
	// ------------------------------------------------------------
	class Breakpoint : public Serializable
	{
	public:

		/**
		 * @brief Constructor.
		 */
		Breakpoint ()
			: place (ipFuncEntry)
		{}

		/**
		 * @brief Copy Constructor.
		 */
		Breakpoint (Breakpoint const & b)
			: funcName (b.funcName), place (b.place)
		{}

		/**
		 * @brief Serializes the breakpoint through the given Serializer.
		 */
		void Serialize (Serializer & out) const
		{
			out.PutString (funcName);
			out.PutInt (place);
			Syslog::Debug ("Breakpoint: serialized");
		}
	
		/**
		 * @brief Deserializes the breakpoint from the given DeSerializer.
		 */
		void DeSerialize (DeSerializer & in)
		{
			funcName = in.GetString ();
			place = (InstrPlace) in.GetInt ();
			Syslog::Debug ("Breakpoint: deserialized");
		}

		std::string		funcName;
		InstrPlace 		place;
	};
}

typedef unsigned long long long64_t;

long_t CurrentTimeMillisec ();
long_t CurrentTimeMicrosec ();

//long CurrentTimeMillisec ();
inline int AttributeValue::GetIntValue () const
{
	CheckType (avtInteger);
	return intValue;
}
inline std::string AttributeValue::GetStringValue () const
{
	CheckType (avtString);
	return strValue;
}	
inline short AttributeValue::GetShortValue () const
{
	CheckType (avtShort);
	return shortValue;
}	
inline float AttributeValue::GetFloatValue () const
{
	CheckType (avtFloat);
	return floatValue;
}	
inline double AttributeValue::GetDoubleValue () const
{
	CheckType (avtDouble);
	return doubleValue;
}	
inline char AttributeValue::GetCharValue () const
{
	CheckType (avtChar);
	return charValue;
}		
inline AttributeValue::AttributeValue ()
: doubleValue (0), type (avtInteger) 
{}
inline AttributeValue::AttributeValue (AttributeValue const & av)
 : strValue (av.strValue), doubleValue (av.doubleValue), type (av.type)
{}
inline void AttributeValue::operator= (AttributeValue const & av)
{
	type = av.type;	
	strValue  = av.strValue;
	doubleValue = av.doubleValue;	
}

/**
 * @brief Given a one string convert it to enum type.
 *
 * @param String String to convert.
 * @param int id that defines the type of enum; 1-AttrSource, 2-AttrValueType and 3-InstrPlace.
 *
 * @return Value of the enum.
 */
int StringToEnum(std::string, int);

#endif
