
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
// PTPProtocol.cpp
// 
// PTPProtocol - 
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#include "PTPProtocol.h"
#include "PTPMsgHeader.h"
#include "Syslog.h"
#include "OutputStream.h"
#include "ByteStream.h"

using namespace std;
using namespace Common;

// write message to the stream and then send the stream
void PTPProtocol::WriteMessage (PTPMessage & msg, OutputStream & stream)
{
	// prepare header
	PTPMsgHeader header;
	header.SetMsgType (msg.GetType ());
	header.SetDataSize (msg.GetDataSize ());
	// write
	NetworkSerializer ser (stream);
	header.Serialize (ser);
	msg.Serialize (ser);
}

// read message from the stream
PTPMessage * PTPProtocol::ReadMessage (istream & stream)
{
	// read header
	PTPMsgHeader header;

	NetworkDeSerializer deser (stream);
	header.DeSerialize (deser);
	// select message type
	PTPMsgType type = header.GetType ();
	PTPMessage * pMsg = 0;
	//switch (type)
//	{
//		case ECPReg:
//			printf ("REGISTER MESSAGE RECEIVED");
//			pMsg  = new RegisterMsg ();
//			break;
//		case ECPEvent:
//			pMsg = new EventMsg ();
//			break;
//		default:
//			printf ("ERROR: Read message format unknown");
//			throw "Unknown message type";
//	}
	// receive message
	assert (pMsg != 0);
	pMsg->DeSerialize (deser);
	return pMsg;
}

// write message to the socket
void PTPProtocol::WriteMessageEx (PTPMessage & msg, Socket & sock)
{
	// prepare header
	PTPMsgHeader header;
	header.SetMsgType (msg.GetType ());
	header.SetDataSize (msg.GetDataSize ());
	// serialize message to stream
	header.SetHeaderSize();
	int size = header.GetDataSize() + msg.GetDataSize();
	char buff[size];
	ByteStream stream(buff, size);
	//std::ostrstream stream;
	NetworkSerializer ser (stream);
	header.Serialize (ser);
	msg.Serialize (ser);
	// send stream
	sock.Send (stream);
}

// read message from the socket
PTPMessage * PTPProtocol::ReadMessageEx (Socket & sock)
{
	const int headerSize = 20;
	char buf [headerSize];

	int k = sock.ReceiveN (buf, sizeof (buf));
	
	strstream stream;
	stream.write (buf, sizeof (buf));
	// read header
	PTPMsgHeader header;
	NetworkDeSerializer deser (stream);
	header.DeSerialize (deser);

	int dataSize = header.GetDataSize ();
	char * buf2 = new char [dataSize];
	k = sock.ReceiveN (buf2, dataSize);

	stream.write (buf2, dataSize);
	delete buf2;
	// select message type
	PTPMsgType type = header.GetType ();
	PTPMessage * pMsg = 0;
	switch (type)
	{
	case PTPLoadLibrary:
	{
		Syslog::Debug ("[PTP] TUNING REQUEST RECEIVED: PTPLoadLibrary");
		pMsg = new LoadLibraryRequest ();
		break;
	}
	case PTPSetVariableValue:
	{
		Syslog::Debug ("[PTP] TUNING REQUEST RECEIVED: PTPSetVariableValue");
		pMsg = new SetVariableValueRequest ();
		break;
	}
	case PTPReplaceFunction:
	{
		Syslog::Debug ("[PTP] TUNING REQUEST RECEIVED: PTPReplaceFunction");
		pMsg = new ReplaceFunctionRequest ();
		break;
	}
	case PTPInsertFuncCall:
	{
		Syslog::Debug ("[PTP] TUNING REQUEST RECEIVED: PTPInsertFuncCall");
		pMsg = new InsertFunctionCallRequest ();
		break;
	}
	case PTPOneTimeFuncCall:
	{
		Syslog::Debug ("[PTP] TUNING REQUEST RECEIVED: PTPOneTimeFuncCall");
		pMsg = new OneTimeFunctionCallRequest ();
		break;
	}
	case PTPRemoveFuncCall:
	{
		Syslog::Debug ("[PTP] TUNING REQUEST RECEIVED: PTPRemoveFuncCall");
		pMsg = new RemoveFunctionCallRequest ();
		break;
	}
	case PTPFuncParamChange:
	{
		Syslog::Debug ("[PTP] TUNING REQUEST RECEIVED: PTPFuncParamChange");
		pMsg = new FunctionParamChangeRequest ();
		break;
	}
	case PTPAddInstr:
	{
		Syslog::Debug ("[PTP] ADD INSTRUMENTATION REQUEST RECEIVED");
		pMsg  = new AddInstrRequest ();
		break;
	}
	case PTPRemoveInstr:
	{
		Syslog::Debug ("[PTP] REMOVE INSTRUMENTATION REQUEST RECEIVED");
		pMsg  = new RemoveInstrRequest ();
		break;
	}
	case PTPStartApp:
	{
		Syslog::Debug ("[PTP] START APP REQUEST RECEIVED");
		pMsg  = new StartAppRequest ();
		break;		
	}
	default:
		throw "Unknown message type";
	}
	// receive message
	assert (pMsg != 0);
	pMsg->DeSerialize (deser);

	return pMsg;
}
