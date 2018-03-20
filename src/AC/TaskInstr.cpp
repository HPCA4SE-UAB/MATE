
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
// TaskInstr.h
// 
// TaskInstr - manages task instrumentation (add/remove)
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#include "TaskInstr.h"
#include "Syslog.h"
#include "SysException.h"

TaskInstr::~TaskInstr ()
{
	Map::iterator it = _map.begin ();
	while (it != _map.end ())
	{
		InstrGroup * group = it->second;
		delete group;
		it++;
	}
}

void TaskInstr::Add (int eventId, string const & functionName, InstrPlace instrPlace, 
					BPatchSnippetHandle * handler)
{
	//Syslog::Debug ("TaskInstr::add");
	InstrGroup * group = FindGroup (eventId);	
	if (!group)
	{
		//Syslog::Debug ("TaskInstr::add creating new group with id: %d", eventId);
		group = new InstrGroup (eventId, functionName);
		_map.insert (Map::value_type (eventId, group));
		//Syslog::Debug ("TaskInstr::Add group added");
	}	
	
	group->AddHandler (instrPlace, handler);
	Syslog::Debug ("TaskInstr::Add handler added for function %s, id: %d, place: %d", 
		functionName.c_str (), eventId, instrPlace);
}

void TaskInstr::Remove (int eventId, InstrPlace instrPlace)
{
	//Syslog::Debug ("TaskInstr::remove ");
	Map::iterator it = _map.find (eventId);
	if (it == _map.end ())
	{
		throw Exception ("TaskInstr::Remove cannot find instrumentation group");
	}
	InstrGroup * group = (*it).second;
	assert (group != 0);
	// remove all handlers from a given place
	group->RemoveHandler (instrPlace);
	// check if group is not empty ?
	if (group->IsEmpty ())
	{
		// remove group
		delete group;
		_map.erase (it);		
	}
	Syslog::Debug ("TaskInstr::Remove handler removed for function id: %d, place: %d", 
		eventId, instrPlace);
}

InstrGroup * TaskInstr::FindGroup (int eventId)
{
	//Syslog::Debug ("TaskInstr::Find");
	Map::const_iterator it = _map.find (eventId);
	if (it != _map.end ())    // value exists
		return (*it).second;
	else
		return 0;
}

