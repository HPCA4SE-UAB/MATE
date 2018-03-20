
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

//-----------------------------------------
//
// DateTime.cpp - represents date and time
//
// Anna Sikora, UAB, January, 2001
//
//-----------------------------------------

#include <strstream>
#include "DateTime.h"
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace Common;

DateTime::DateTime ()  // current date and time 
{
	time_t clock;
	// get current time in seconds from 1 Jan 1970 (UTC)
	// time_t time(time_t *tloc);
	//
	// time() returns the value of time in seconds  since  00:00:00
    // UTC, January 1, 1970
	time (&clock);
	// get date-time structure
	// struct tm *gmtime(const time_t *clock);;
	//struct tm * tmp = gmtime (&clock);
	
	struct tm * tmp = localtime (&clock);
	
	// we have pointer to the static tm structure
	// if we call gmtime once more, the static structure will be overwritten
	// so, we have to copy to our structure the part of memory 
	// where this tm structure is located 
	//
	// void *memcpy(void *dest, const void *src, size_t n);
	// memcpy() copies n bytes from  memory area src to dest.
	// It returns dest.
	memcpy (&_time, tmp, sizeof (_time));	
}

// assume format dd.mm.yyyy hh:MM:ss
string DateTime::GetStringValue () const 
{
	// ostringstream - output string stream 
	// special stream that can write to string 
	// it can also automatically grow
	int year = GetYear ();
	int month = GetMonth ();
	int day = GetDay ();
	int hour = GetHour ();
	int min = GetMinute ();
	int sec = GetSecond ();
	
	// check the value of sec, min and hour
	// if they are x0, add 0 at the end
	// if they are 0x, add 0 at the beginning
	//..........................

	/* changed by Ania*/
	/*ostrstream s;
	s << day << "-" << month << "-" << year << ", "
	  << hour << ":" << min << ":" << sec << " " << '\0'; 
	return s.str ();	
*/
	char s [30];
	sprintf (s, "%.2d-%.2d-%d %.2d:%.2d:%.2d ", day, month, year, hour,
min, sec);
	return s;
}

