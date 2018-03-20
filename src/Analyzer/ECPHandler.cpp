
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
// EventCollHandler.cpp
// 
// EventCollHandler - handles coming events
// 					one handler for one process (client)
//
// Anna Sikora, UAB, 2002-2010
//
//----------------------------------------------------------------------

//#include "EventCollector.h"
#include "ECPHandler.h"
#include "ECPMsg.h"
#include "Syslog.h"


void ECPHandler::HandleInput() {
	ECPMessage * msg = ECPProtocol::ReadMessageEx(*_socket);
	EventListener * listener = _collector->GetListener();
	bool isUnregister = false;
	
	switch (msg->GetType()) {
		case ECPEvent:
			{
				EventMsg * em = (EventMsg*) msg;
				//Syslog::Debug ("[ECP] event from %d", _tid);
				em->SetTid(_tid);
				break;
			}
		case ECPReg:
			{
				RegisterMsg * rm = (RegisterMsg*) msg;
				_tid = rm->GetPid();
				//Syslog::Debug ("[ECP] register worker %d", _tid);
				break;
			}
		case ECPUnReg:
			{
				isUnregister = true;
				break;
			}
		default:
			// ignore
			break;
	}
	
	if (listener != 0)
		listener->OnEvent(msg);
	else
		delete msg;

	if (isUnregister)	
		_service->Remove (this);
}


