
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
// EventCollectorProxy.cpp
// 
// EventCollectorProxy - class that connects to the analyzer host
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#include "ECPProxy.h"
#include "ECPMsgHeader.h"
#include "ByteStream.h"

using namespace DMLib;
using namespace std;

void EventCollectorProxy::RegisterLib (int pid, int mpiRank, string host, string taskName, int ACport)
{
	Syslog::Debug ("[EventCollectorProxy] RegisterLib");
	RegisterMsg reg (pid, mpiRank, host, taskName, ACport);
	WriteMessage (reg);
	Syslog::Debug ("[EventCollectorProxy] RegisterLib sent");
}

void EventCollectorProxy::UnregisterLibrary (int tid)
{
	Syslog::Debug ("[EventCollectorProxy] UnregisterLib");
	UnRegisterMsg unreg (tid);
	WriteMessage (unreg);
	Syslog::Debug ("[EventCollectorProxy] UnregisterLib sent");
}

void EventCollectorProxy::SendEvent (EventMsg const & event)
{
	Syslog::Debug ("[EventCollectorProxy] SendEvent");
	WriteMessage (event);
	Syslog::Debug ("[EventCollectorProxy] SendEvent sent");
}

// write message to the socket
void EventCollectorProxy::WriteMessage (ECPMessage const & msg)
{

	// reuse data buffer
	ByteStream out (_sendBuffer, sizeof (_sendBuffer));
	NetworkSerializer ser (out);
	
	ECPMsgHeader header;
	header.SetMsgType (msg.GetType ());
	header.SetDataSize (msg.GetDataSize ());
	
	// serialize	
	header.Serialize (ser);
	msg.Serialize (ser);
	Syslog::Debug ("[EventCollectorProxy] SendEvent sending...");
	// send buffer
	_socket.Send (out.GetData (), out.GetDataSize ());
}

