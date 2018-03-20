
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
// Utils.cpp
//
// Anna Sikora, UAB, 2003
//
//----------------------------------------------------------------------

#include "Utils.h"
#include "Exception.h"
//PAOLA: AGREGADO 04.06.04
#include <stdio.h>
#include <boost/assign/list_of.hpp>

using namespace std;

std::map<std::string, AttrSource> AttrSourceMap = boost::assign::map_list_of("FuncParamValue", asFuncParamValue)\
	("VarValue", asVarValue)("FuncReturnValue",asFuncReturnValue)("ConstValue",asConstValue)\
	("FuncParamPointer",asFuncParamPointerValue)("SendMessageSize",asSendMessageSize)("RecvMessageSize",asRecvMessageSize)\
	("SenderTid",asSenderTid);
std::map<std::string, AttrValueType> AttrValueMap = boost::assign::map_list_of("Integer", avtInteger)("Short", avtShort)\
	("Float",avtFloat)("Double",avtDouble)("Char",avtChar)("String",avtString);
std::map<std::string, InstrPlace> InstrPlaceMap = boost::assign::map_list_of("FuncEntry", ipFuncEntry)("FuncExit", ipFuncExit);

int StringToEnum(std::string s, int typeEnum)
{
	switch(typeEnum)
	{
		case 1: // Source
			return AttrSourceMap[s];
			break;
		case 2: // Type
			return AttrValueMap[s];
			break;
		case 3: // Place
			return InstrPlaceMap[s];
			break;
		default: // Others
			Syslog::Debug("Error in to convert at type of enum");
			break;
	}
}

long_t CurrentTimeMillisec ()
{
	struct timeval t;
	::gettimeofday (&t, 0);
	
	//czas w milisekundach, ktore uplynely od 1.1.1970 UTC.
	return ((long_t)t.tv_sec) * 1000 + (long_t)(t.tv_usec/1000);
}

long_t CurrentTimeMicrosec ()
{
	struct timeval t;
	::gettimeofday (&t, 0);
	
	//czas w microsekundach, ktore uplynely od 1.1.1970 UTC.
	return ((long_t)t.tv_sec) * 1000000L + (long_t)(t.tv_usec);
}

/*long CurrentTimeMillisec ()
{
	struct timeval t;
	::gettimeofday (&t, 0);
	
	//czas w milisekundach, ktore uplynely od 1.1.1970 UTC.
	return (t.tv_sec*1000) + (t.tv_usec/1000);
}*/	

void AttributeValue::Serialize (Serializer & out) const
{

	out.PutInt (type);
	switch (type)
	{
	case avtInteger:
		out.PutInt (intValue);
		break;			
	case avtShort:
		out.PutShort (shortValue);
		break;		
	case avtFloat:
		out.PutDouble ((double)floatValue);
		break;		
	case avtDouble:
		out.PutDouble (doubleValue);		
		break;		
	case avtChar:
		out.PutChar (charValue);		
		break;		
	case avtString:
		out.PutString (strValue);		
		break;
	default:
		assert (0);
		break;
	}
}

void AttributeValue::DeSerialize (DeSerializer & in)
{
	type = (AttrValueType) in.GetInt ();
	switch (type)
	{
	case avtInteger:
		intValue = in.GetInt ();
		break;			
	case avtShort:
		shortValue = in.GetShort ();
		break;		
	case avtFloat:
		floatValue = (float)in.GetDouble ();
		break;		
	case avtDouble:
		doubleValue = in.GetDouble ();
		break;		
	case avtChar:
		charValue = in.GetChar ();
		break;		
	case avtString:
		strValue = in.GetString ();
		break;
	default:
		throw "Invalid data type in Attribute Value";
		break;
	}		
}

int AttributeValue::GetSize () const
{
	switch (type)
	{
	case avtInteger: return sizeof (int);
	case avtShort: return sizeof (short);
	case avtFloat: return sizeof (float);
	case avtDouble: return sizeof (double);
	case avtChar: return sizeof (char);
	case avtString: return strlen (strValue.c_str ()) + 1; // with \0
	default:
		assert (0);
		return 0;
	}	
}

string AttributeValue::ToString () const
{
	char buf [20];
	switch (type)
	{
	case avtInteger: 
		sprintf(buf, "%d", intValue);
		break;
	case avtShort: 
		sprintf (buf, "%d", shortValue); // short ?
		break;	
	case avtFloat: 
		sprintf (buf, "%f", floatValue); 
		break;	
	case avtDouble: 
		sprintf (buf, "%f", doubleValue);  // o %g ?
		break;	
	case avtChar: 
		sprintf (buf, "%c", charValue);  // o %g ?
		break;
	case avtString: 
		return strValue; 
	default:
		assert (0);
		return 0;
	}	
	return buf;
}
	
void * AttributeValue::GetValueBuffer ()
{
	if (type == avtString)
		return (void*)strValue.c_str ();
	else
		return (void*)&intValue; // valid for all other types
}

void AttributeValue::CheckType (AttrValueType requested) const
{
	if (type != requested)
	{
		Syslog::Debug ("Requested attribute data type <%s> %d differs from definition <%s> %d", 
			Attribute::GetTypeString (requested).c_str (), requested,
			Attribute::GetTypeString (type).c_str (), type);
		throw Exception ("Invalid data type requested");
	}
} 

static string SourceNames [] =
{
	string("FuncParamValue"),
	string("VarValue"),
	string("FuncReturnValue"),
	string("ConstValue"),
	string("FuncParamPointerValue"),
	string("SendMessageSize"),
	string("RecvMessageSize"),
	string("SenderTid")
};

string Attribute::GetSourceString () const
{
	if (source >= asFuncParamValue && source <= asSenderTid)
		return SourceNames[source];
	else
		return "unknown";
}

string Attribute::GetTypeString (AttrValueType type) 
{
	switch (type)
	{
	case avtInteger:
		return "int";
	case avtShort:
		return "short";
	case avtFloat:
		return "float";
	case avtDouble:
		return "double";
	case avtChar:
		return "char";
	case avtString:
		return "char*";
	default:
		return "unknown";
	}
}

