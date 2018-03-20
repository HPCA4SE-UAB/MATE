
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

/*
 * ShutDownManager.cpp
 *
 *  Created on: Jul 5, 2011
 *      Author: rodrigo
 */

#include "ShutDownManager.h"
#include "Socket.h"
#include "Syslog.h"
#include <stdio.h>
#include "auto_vector.h"
#include "Host.h"

using namespace Model;

ShutDownManager::ShutDownManager() {
	_finished = false;
	Resume();
}

ShutDownManager::~ShutDownManager() {
	// TODO Auto-generated destructor stub
}

void ShutDownManager::Run() {
	while (1) {
		scanf("%c",&endExecution);
		if (endExecution == 's'){
			break;
		}
		if (endExecution == 'h') {
			Hosts hosts = _app->GetHosts();
			if (hosts.size()==0) {
				Syslog::Warn("[ShutDownManager] No hosts registered");
			}
			Hosts::iterator i = hosts.begin();
			while (i != hosts.end()) {
				Host * h = (*i);
				cerr << h->GetName() << endl;
				i++;
			}
		}
	}

	//Send a stop signal to the AC's
	Hosts hosts = _app->GetHosts();
	if (hosts.size()==0) { //If there aren't any hosts registered yet
		Syslog::Warn("[ShutDownManager] No hosts registered");
	}
	Hosts::iterator i = hosts.begin();
	int port = SIGNALS_PORT;
	char buf []="Interrupt";
	while (i != hosts.end()){
		Host * h = (*i);
		Socket *sock = new Socket(h->GetName(),port);
		sock->Send(buf,10,0);
		i++;
	}

	//Stop the Analyzer itself
	_finished = true;

}
