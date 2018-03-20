
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
// Service.cpp
// 
// Service - holds a list of ECPHandler
// 
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#include "Service.h"
#include "ECPHandler.h"

void Service::Add(ECPHandler * handler) {
	_list.push_back(handler);
	handler->SetService(this);
}
	
void Service::Remove(ECPHandler * handler) {
	//Syslog::Debug ("Handler list contains %d elements.", _list.size ());
	//Syslog::Debug ("Removing handler from service");
	_reactor.UnRegister(*handler);
	//Syslog::Debug ("Handler unregistered from reactor");
	
	int idx = 0;
	Iter iter = _list.begin();
	while (iter != _list.end()) {
	   	ECPHandler * h = (*iter);
    	if (h == handler)
      		break;
      	idx++;
    	iter++;
 	}
 	if (iter == _list.end())
 		throw "Given Event Collector Handler does not exist in the service";
 	delete _list[idx];
	_list.erase(iter);
}

