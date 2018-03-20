
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
// Terminator.cpp
//
// AC shut down manager implementation
//
//----------------------------------------------------------------------

#include "ShutDownSlave.h"
#include "Ctrl.h"
#include <stdio.h>
#include "Socket.h"
#include "Config.h"
#include <sys/types.h>
#include <unistd.h>

using namespace std;

ShutDownSlave::ShutDownSlave(string analyzerHost, int analyzerPort, Controller ctrl)
:_analyzerHost(analyzerHost), _analyzerPort(analyzerPort), _SSock(analyzerPort), _ctrl( ctrl)
{
	_SSock.Listen();
	ActiveObject::Resume();
}

ShutDownSlave::~ShutDownSlave(){ /*TODO*/ }

void ShutDownSlave::Run (){
	char buf[10];
	char intSig[] = "Interrupt";
	// Loop that allows for more than one message received
	while(1){
		// Create client socket
		SocketPtr clientSocket = _SSock.Accept();
		// Blocking call that receives the message
		clientSocket->Receive(buf, sizeof(buf));
		// Memory comparison rather than strcmp
		if( memcmp(buf, intSig, strlen(intSig)) == 0 ){
			// Call interrupting function of the AC
			_ctrl.Interrupt();
			// Exit loop
			break;
		}
	}
	exit(0);
}

void ShutDownSlave::InitThread (){

}

void ShutDownSlave::FlushThread (){

}
