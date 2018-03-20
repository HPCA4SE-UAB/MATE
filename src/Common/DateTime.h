
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

// MATE
// Copyright (C) 2002-2008 Ania Sikora, UAB.

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef __DATETIME_H__
#define __DATETIME_H__

// C++ includes
#include <sys/types.h>
#include <time.h>
#include <string>

namespace Common {
	/**
	 * @class DateTime
	 * @brief Holds a timestamp.
	 *
	 * @version 1.0
	 * @since 1.0
	 * @author Ania Sikora, 2001
	 *
	 */
	
	// ------------------------------------------------------------
	class DateTime
	{
	public:

		/**
		 * @brief Constructor, sets the current date and time.
		 */
		DateTime ();
	
		/**
		 * @brief Returns year represented by this date.
		 */
		int GetYear () const
		{
			return (_time.tm_year + 1900);
		}

		/**
		 * @brief Returns month represented by this date.
		 */
		int GetMonth () const
		{
			return (_time.tm_mon + 1);
		}

		/**
		 * @brief Returns day represented by this date.
		 */
		int GetDay () const
		{
			return _time.tm_mday;
		}

		/**
		 * @brief Returns hour represented by this date.
		 */
		int GetHour () const
		{
			return _time.tm_hour;
		}

		/**
		 * @brief Returns minute represented by this date.
		 */
		int GetMinute () const
		{
			return _time.tm_min;
		}

		/**
		 * @brief Returns second represented by this date.
		 */
		int GetSecond () const
		{
			return _time.tm_sec;
		}

		/**
		 * @brief Returns a string with the date.
		 *
		 * The format of the returned string is "dd.mm.yyyy hh:MM:ss".
		 */
		std::string GetStringValue () const;
	
	private:
	
		/* Already on time.h
		struct	tm {
			int	tm_sec;		//seconds after the minute - [0, 61]
			int	tm_min;		//minutes after the hour - [0, 59]
			int	tm_hour;	//hour since midnight - [0, 23]
			int	tm_mday;	//day of the month - [1, 31]
			int	tm_mon;		//months since January - [0, 11]
			int	tm_year;	//years since 1900
			int	tm_wday;	//days since Sunday - [0, 6]
			int	tm_yday;	//days since January 1 - [0, 365]
			int	tm_isdst;	//flag for alternate daylight savings time
		};
		*/
		struct tm 	_time;
	};
}

#endif

