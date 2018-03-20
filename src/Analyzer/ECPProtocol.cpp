
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
// ECPProtocol.cpp
// 
// ECPProtocol - 
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#include "ECPProtocol.h"
#include "ECPMsgHeader.h"
#include "Syslog.h"


// write message to the socket
/*
void ECPProtocol::WriteMessageEx (ECPMessage const & msg, Socket & sock)
{
	// prepare header
	ECPMsgHeader header;
	header.SetMsgType (msg.GetType ());
	header.SetDataSize (msg.GetDataSize ());
	// serialize message to stream
	std::ostrstream stream;
	NetworkSerializer ser (stream);
	header.Serialize (ser);
	msg.Serialize (ser);
	// send stream
	sock.Send (stream);
}
*/

ECPMsgHeader ECPProtocol::ReadMessageHeader(Socket & sock) {
	const int HEADER_SIZE = 20;
	char buf[HEADER_SIZE];
	//Syslog::Debug ("[ECP]: ReadMessageHeader");
	
	int k = sock.ReceiveN(buf, sizeof (buf));
	//Syslog::Debug ("[ECP]:Receive %d bytes of header", k);
	
	std::strstream stream;
	stream.write(buf, sizeof(buf));
	// read header
	ECPMsgHeader header;
	NetworkDeSerializer deser(stream);
	header.DeSerialize(deser);
	return header;
}

ECPMessage * ECPProtocol::ReadMessageEx(Socket & sock) {
	ECPMsgHeader header = ReadMessageHeader(sock);
	
	int dataSize = header.GetDataSize();
	//Syslog::Debug ("[ECP]: DataSize = %d, ", dataSize);
	char * buf = new char[dataSize];
	int k = sock.ReceiveN(buf, dataSize);
	//Syslog::Debug ("[ECP]: K=%d bytes received\n", k);

	std::strstream stream;
	stream.write(buf, dataSize);
	delete buf;

	// select message type
	ECPMsgType type = header.GetType();
	ECPMessage * pMsg = 0;
	switch (type) {
		case ECPReg:
			//Syslog::Debug ("[ECP]: REGISTER MESSAGE RECEIVED");
			pMsg  = new RegisterMsg();
			break;
		case ECPEvent:
			//Syslog::Debug ("[ECP]: EVENT MESSAGE RECEIVED");
			pMsg = new EventMsg();
			break;
		case ECPUnReg:
			//Syslog::Debug ("[ECP]: UNREGISTER LIBRARY MESSAGE RECEIVED");
			pMsg = new UnRegisterMsg();
			break;
		default:
			//Syslog::Debug ("[ECP]: ERROR: Read message format unknown");		
			throw "Unknown message type";			
	}
	// receive message
	assert(pMsg != 0);
	NetworkDeSerializer deser(stream);
	pMsg->DeSerialize(deser);
	return pMsg;
}

