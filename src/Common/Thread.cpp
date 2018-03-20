
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
// thread.cpp
// ------------------------------------------------------------------

#include "Thread.h"
#include "SysException.h"
#include <signal.h>
#include <stdio.h>

using namespace Common;

Thread::Thread (void* (*pFun) (void* arg), void* pArg)
{
	// initialize thread objects with default values */
	int status = ::pthread_attr_init (&_attrs);
	if (status != 0)
		throw SysException("thread attributes init failed");
	// create thread
	status = ::pthread_create (&_handle, &_attrs, pFun, pArg);
	if (status != 0)
		throw SysException("thread creation failed");
}

void Thread::WaitForDeath ()
{
	// wait for termination of the thread
	int status = ::pthread_join(_handle, (void**)0);
	if (status != 0)
		throw SysException("join thread failed");
}

void Thread::Exit ()
{
	::pthread_kill(_handle, 0);
}

