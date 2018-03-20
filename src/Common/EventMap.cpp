
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
// EventMap.cpp
//
// Anna Sikora, UAB, 2001
//
//----------------------------------------------------------------------

#include "EventMap.h"
#include "EventException.h"
#include <stdio.h>

using namespace std;
using namespace Common;

void EventMap::Add (string const & name, int id)
{
	Pair pair = _map.insert (StringMap::value_type (name, id));
	// check if key is unique
	if (!pair.second){
		string str = "";
		char buff[33];
		int size = sprintf(buff, "%d", id);

		str.append("name = ", 7);
		str.append(name);
		str.append(" id = ", 6);
		str.append(buff, size);

		throw EventException("Such a pair name-id already exists on the event map", str);
	}
}

int EventMap::GetId (string const & name) const
{
	StringMap::const_iterator theIterator;

	theIterator = _map.find (name);
	if (theIterator != _map.end ())    // value exists
	{
		return (*theIterator).second;
	}
	else
	{
		throw EventException("Name not found");
	}
}

