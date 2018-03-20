
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
// ECPEventMsg.cpp
// Anna Sikora, UAB, 2004
// This code is used by DMLib
//----------------------------------------------------------------------

#include "ECPMsg.h"
#include "NetSer.h"
#include <stdio.h>

using namespace Common;

//---------------------------------------------------------------------
int ECPMessage::GetDataSize () const
{
	CountingSerializer ser;
	Serialize (ser);
	return ser.GetSize ();
}

void EventMsg::Reset (long_t timestamp, int eventId, InstrPlace place, int paramCount)
{
	_timestamp = timestamp;
	_eventId = eventId;
	_place = place;
	_paramCount = paramCount;
	_paramBufSize = 0;
	if (_paramBuffer != 0)
	{
		//delete [] _paramBuffer;
	}
	_paramBuffer = 0;
}

void EventMsg::SetParams (char const * buffer, int size)
{
	if (_paramBuffer != 0)
				delete [] _paramBuffer;
	_paramBuffer = buffer;
	_paramBufSize = size;
}

void EventMsg::SetBuffer (char * buffer)
{
	if (_paramBuffer != 0)
		delete [] _paramBuffer;
	_paramBuffer = buffer;
}

int EventMsg::GetDataSize () const
{
	CountingSerializer ser;
	Serialize (ser);
	return ser.GetSize ();
}

void EventMsg::Serialize (Serializer & out) const
{
	//serialize data
	Syslog::Debug ( "[ECP] Event (T: %lld, EventId: %d, Place: %d, paramCount %d, paramBufSize: %d, paramBuf: %d\n", 
			_timestamp, _eventId, _place, _paramCount, _paramBufSize, _paramBuffer);

	out.PutLong (_timestamp);
	out.PutInt (_eventId);
	out.PutInt ((int)_place);
	out.PutInt (_paramCount);
	// serialize params
	out.PutInt (_paramBufSize);
	out.PutBuffer (_paramBuffer, _paramBufSize);

}

void EventMsg::DeSerialize (DeSerializer & in)
{
	_timestamp = in.GetLong ();
	_eventId = in.GetInt ();
	_place = (InstrPlace)in.GetInt ();
	_paramCount = in.GetInt ();
	// deserialize parameters
	int size = in.GetInt ();
	char * buf = new char [size];
	in.GetBuffer (buf, size);
	if (_paramBuffer != 0)
		delete [] _paramBuffer;
	_paramBuffer = buf;
	_paramBufSize = size;
	Syslog::Debug ( "[ECP]: DeSerialized EventMsg: timestamp: %lld, eventId: %d, place: %d, paramCount: %d, paramBufSize: %d, paramBuf: %d\n", 
			_timestamp, _eventId, _place, _paramCount, _paramBufSize, _paramBuffer);

}

void RegisterMsg::Serialize (Serializer & out) const
{
	Syslog::Debug ("[ECP]: Serialize RegisterMsg: PID: %d, mpiRank: %d, host: %s, taskName: %s, ACport: %d\n",
		_pid, _mpiRank, _host.c_str(), _taskName.c_str(), _ACport);
	out.PutInt (_pid);
	out.PutInt (_mpiRank);
	out.PutString (_host);
	out.PutString (_taskName);
	out.PutInt (_ACport);
}

void RegisterMsg::DeSerialize (DeSerializer & in)
{
	_pid = in.GetInt ();
	_mpiRank = in.GetInt ();
	_host = in.GetString ();
	_taskName = in.GetString ();
	_ACport = in.GetInt ();
	Syslog::Debug ("[ECP]: DeSerialized RegisterMsg: PID: %d, mpiRank: %d, host: %s, task: %s, ACport \n",
		_pid, _mpiRank, _host.c_str(), _taskName.c_str (), _ACport);
}