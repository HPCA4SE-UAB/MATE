
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

// ------------------------------------------------------------------
// EventMsgWriter.cpp
// ------------------------------------------------------------------

#include "EventMsgWriter.h"
#include "Syslog.h"

using namespace std;
using namespace DMLib;

void EventMsgWriter::OpenEvent (long_t timestamp, int eventId, InstrPlace place, int paramCount)
{
	_event.Reset (timestamp, eventId, place, paramCount);
	_stream.Reset ();
}

void EventMsgWriter::AddIntParam (int value)
{
	_ser.PutByte ((byte_t)avtInteger);
	Syslog::Debug ("AddIntParam PutByte: <%d>", (byte_t)avtInteger);
	_ser.PutInt (value);
	Syslog::Debug ("AddIntParam: %d %ld %d %ld", value, sizeof(value), avtInteger, sizeof(avtInteger));
}

void EventMsgWriter::AddFloatParam (float value)
{
	_ser.PutByte ((byte_t)avtFloat);
	_ser.PutDouble ((double)value);
	Syslog::Debug ("AddFloatParam: %f", value);
}

void EventMsgWriter::AddDoubleParam (double value)
{
	_ser.PutByte ((byte_t)avtDouble);
	_ser.PutDouble (value);
	Syslog::Debug ("AddDoubleParam: %lf", value);
}

void EventMsgWriter::AddCharParam (char c)
{
	_ser.PutByte ((byte_t)avtChar);
	_ser.PutChar (c);
	Syslog::Debug ("AddCharParam: %c", c);
}

void EventMsgWriter::AddStringParam (string const & s)
{
	_ser.PutByte ((byte_t)avtString);
	if (s.size() != 0)
	{
		_ser.PutString (s);
		//Syslog::Debug ("AddStringParam: %s", s);
	}
	else
	{
		_ser.PutString ("");
		//Syslog::Debug ("AddStringParam: empty string");
	}
}

EventMsg const & EventMsgWriter::CloseEvent ()
{
	// connect event with param buffer
	_event.SetParams (_stream.GetData (), _stream.GetDataSize ());
	Syslog::Debug ("Closing event with params: <%s> %d", _stream.GetData (), _stream.GetDataSize ());
	return _event;
}
