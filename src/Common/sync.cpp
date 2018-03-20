
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

// ------------------------------------------------------------------
// sync.cpp
//
// ------------------------------------------------------------------

#include "sync.h"

using namespace Common;

void Semaphore::Wait ()
{
	int status = ::sem_wait (&_semaphore);
	if (status != 0)
		throw SysException("Semaphore wait failed");
}

bool Semaphore::TryWait ()
{
	int status = ::sem_trywait (&_semaphore);
	return (status == 0) ? true : false;
}

void Semaphore::Post ()
{
	int status = ::sem_post (&_semaphore);
	if (status != 0)
		throw SysException("Semaphore post failed");
}

void Mutex::Enter ()
{
	int status = ::pthread_mutex_lock (&_mutex);
	if (status != 0)
		throw SysException("Enter mutex failed");
}

bool Mutex::CanEnter ()
{
	int status = ::pthread_mutex_trylock (&_mutex);
	return (status == 0);
}

void Mutex::Leave ()
{
	int status = ::pthread_mutex_unlock (&_mutex);
	if (status != 0)
		throw SysException("Leave mutex failed");
}
