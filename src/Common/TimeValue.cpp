
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

// ----------------------------------------------
// TimeVal.cpp
// ----------------------------------------------

#include "TimeValue.h"

using namespace Common;

// 1 usec = 1 / 1.000.000 sec
const long ONE_SECOND_IN_USECS  = 1000000L;

void TimeValue::Normalize ()
{
	// revisit

	if (_t.tv_usec >= ONE_SECOND_IN_USECS)
    {
		do
        {
			_t.tv_sec++;
			_t.tv_usec -= ONE_SECOND_IN_USECS;
		}
		while (_t.tv_usec >= ONE_SECOND_IN_USECS);
    }
	else if (_t.tv_usec <= -ONE_SECOND_IN_USECS)
    {
		do
        {
			_t.tv_sec--;
			_t.tv_usec += ONE_SECOND_IN_USECS;
		}
		while (_t.tv_usec <= -ONE_SECOND_IN_USECS);
    }

	if (_t.tv_sec >= 1 && _t.tv_usec < 0)
    {
		_t.tv_sec--;
		_t.tv_usec += ONE_SECOND_IN_USECS;
	}
	else if (_t.tv_sec < 0 && _t.tv_usec > 0)
    {
		_t.tv_sec++;
		_t.tv_usec -= ONE_SECOND_IN_USECS;
    }
}

