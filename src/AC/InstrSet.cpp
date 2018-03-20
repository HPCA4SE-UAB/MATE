
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
// InstrSet.cpp
// 
// Instrumentation set - class that represents a filtr of program 
//						instrumentation
//		   (current set of functions that are instrumented in a program)
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#include "InstrSet.h"
#include "Syslog.h"

InstrGroup::~InstrGroup ()
{
	for (int i=0; i<GetSize (); i++)
		delete _vector[i];
	_vector.clear ();
}
	
void InstrGroup::AddHandler (InstrPlace place, BPatchSnippetHandle * handle)
{
	SnippetHandler * handler = new SnippetHandler (_eventId, _funcName, place, handle);
	_vector.push_back (handler);
}

// remove all handlers for a given place
void InstrGroup::RemoveHandler (InstrPlace place)
{
	Iterator it = _vector.begin ();
	while (it != _vector.end ())
	{
		SnippetHandler * handler = (*it);
		// it = pointer to SnippetHandler
		if (handler->GetInstrPlace () == place)
		{
			delete handler; // delete this element
			it = _vector.erase (it);
		}
		else
		{
			it++;
		}
	}
}
