
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

/**
 * activeobject.cpp
 */

#include "ActiveObject.h"

using namespace Common;

ActiveObject::ActiveObject ()
: _isDying (0), _resumed (false),
#pragma warning(disable: 4355) // 'this' used before initialized
  _thread (ThreadEntry, this)
#pragma warning(default: 4355)
{
	 // wait for thread to start
	 // and to suspend on _resumeSem
	WaitForStartup ();
}

// FlushThread must reset all the events
// on which the thread might be waiting.

void ActiveObject::Kill ()
{
    _isDying++;
	// resume thead if it didn't start yet
	if (!_resumed)
		_resumeSem.Post ();
    FlushThread ();
    // Let's make sure it's gone
    _thread.WaitForDeath ();
}

void ActiveObject::SignalStartup ()
{
	_startSem.Post (); 	
}

void ActiveObject::WaitForStartup ()
{
	_startSem.Wait (); 
}

void ActiveObject::Resume ()
{
	_resumeSem.Post (); 
	_resumed = true;
}

void ActiveObject::WaitForResume ()
{
	_resumeSem.Wait ();
}

void * ActiveObject::ThreadEntry (void* pArg)
{
    ActiveObject * pActive = (ActiveObject *) pArg;
    /* I'm a new thread, and I must co-operate so I can be suspended. */
    pActive->SignalStartup ();
    pActive->WaitForResume ();
    pActive->InitThread ();
    pActive->Run ();
    return 0;
}

