
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
// NetSer.cpp
//
//----------------------------------------------------------------------

#include "NetSer.h"

using namespace std;
using namespace Common;

void NetworkSerializer::PutLong (long_t l)
{
	// 64-bit long is converted to high and low integers
	int_t high, low;
	LONG2INTS (l, high, low);
	//Syslog::Debug ("NetSer -> long is: %lld, high int: %ld, low int: %ld", l, high, low);
	PutInt (high);
	PutInt (low);
}

void NetworkSerializer::PutShort (short_t s)
{
	// 16-bit value conversion to network byte-order (i.e. big endian)
	s = HTONS (s);
	_stream.Write (reinterpret_cast<char *> (&s), sizeof (short_t));
}

void NetworkSerializer::PutString (string const & str)
{
	int_t len = str.size();
	PutInt (len);
	_stream.Write (str.c_str(), len);
}

void NetworkSerializer::PutInt (int_t i)
{
	// 32-bit value conversion to network byte-order (i.e. big endian)
	i = HTONL (i);
	//Syslog::Debug ("[API] PutInt %d", i);
	_stream.Write (reinterpret_cast<char *> (&i), sizeof (int_t));
	//Syslog::Debug ("[API] PutInt-stream %s", reinterpret_cast<char *> (&i));
}

long_t NetworkDeSerializer::GetLong ()
{
	long_t l;

	// high and low integers are converted to 64-bit long
	int_t high, low;
	high = GetInt();
	low = GetInt();

	INTS2LONG (high, low, l);
	return l;
}

double_t NetworkDeSerializer::GetDouble ()
{
	double_t d;
	_stream.read (reinterpret_cast<char *> (&d), sizeof (double_t));
	if (_stream.bad())
		throw "stream read failed";

	return d;
}

short_t NetworkDeSerializer::GetShort ()
{
	short_t s;
	_stream.read (reinterpret_cast<char *> (&s), sizeof (short_t));
	if (_stream.bad())
		throw "stream read failed";

	// 16-bit value conversion to network byte-order (i.e. big endian)
	return NTOHS (s);
}

byte_t NetworkDeSerializer::GetByte ()
{
	byte_t b;
	_stream.read (reinterpret_cast<char *> (&b), sizeof (byte_t));
	if (_stream.bad())
		throw "stream read failed";

	return b;
}

char_t NetworkDeSerializer::GetChar ()
{
	char_t c;
	_stream.read (reinterpret_cast<char *> (&c), sizeof (char_t));
	if (_stream.bad())
		throw "stream read failed";

	return c;
}

string NetworkDeSerializer::GetString ()
{
	int_t len = GetInt ();
	string str;
	str.resize (len);
	_stream.read (&str [0], len);
	if (_stream.bad())
		throw "stream read failed";
	return str;
}

int_t NetworkDeSerializer::GetInt ()
{
	int_t i;
	// 32-bit value conversion to network byte-order (i.e. big endian)
	_stream.read (reinterpret_cast<char *> (&i), sizeof (int_t));
	if (_stream.bad())
		throw "stream read failed";

	return NTOHL (i);
}

void NetworkDeSerializer::GetBuffer (char * buffer, int bufferSize)
{
	_stream.read (buffer, bufferSize);
	if (_stream.bad ())
		throw "stream read failed";
}
