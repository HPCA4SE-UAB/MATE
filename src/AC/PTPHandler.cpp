
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
// PTPHandler.cpp
// 
// PTPHandler - handles coming solutions from analyzer
// Communicator module
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#include "PTPHandler.h"
#include "Syslog.h"

void PTPHandler::HandleInput ( )
{	
	Syslog::Debug ("PTP handling intput");
	PTPMessage * msg = PTPProtocol::ReadMessageEx (*_socket);
	switch (msg->GetType ())
	{
	case PTPAddInstr:
	{
		AddInstrRequest * instrReq = (AddInstrRequest*)msg;

		_tm.GetMonitor ().AddInstr (*instrReq);
		delete msg;
		break;
	}
	case PTPRemoveInstr:
	{
		RemoveInstrRequest * instrReq = (RemoveInstrRequest*)msg;
		_tm.GetMonitor ().RemoveInstr (*instrReq);
		delete msg;
		break;
	}
	case PTPStartApp:
	{
		StartAppRequest * startReq = (StartAppRequest*)msg;
		//_tm.SetAnalyzerHost (_socket->GetRemoteAddress ().GetHostName ());
		Syslog::Debug("analyzer Port is %d !!!",_socket->GetLocalPort());
		_tm.SetAnalyzerHost (startReq->GetAnalyzerHost (), _socket->GetLocalPort());
		_tm.StartMPITask (startReq->GetAppPath (), startReq->GetArgs ());
		break;
	}
	default: 
		// tuning action
		TuningRequest * req = (TuningRequest *)msg;
		_tm.GetTuner ().Process (req);
		// note: this is object is consumed by the tuner
		break;
	}

	
}




