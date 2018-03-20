
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

#ifndef __TIMEVAL_H__
#define __TIMEVAL_H__

// Local includes
#include "Utils.h"
#include "Types.h"

// C++ includes
#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/types.h>

namespace Common {
	/**
	 * @class TimeValue
	 * @brief Stores a time value up to microseconds.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class TimeValue
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * By default the value of the time value is 0 seconds and
		 * 0 microseconds.
		 *
		 */
		TimeValue ()
		{
			_t.tv_sec = 0;
			_t.tv_usec = 0;
		}

		/**
		 * @brief Constructor.
		 *
		 * Sets seconds and microseconds (microseconds are 0 by default).
		 *
		 * @param secs Seconds.
		 * @param usecs Microseconds.
		 *
		 */
		TimeValue (long secs, long usecs = 0)
		{
			_t.tv_sec = secs;
			_t.tv_usec = usecs;
			Normalize ();
		}

		/**
		 * @brief Constructor.
		 *
		 * Assigns the values of the TimeValue object.
		 *
		 */
		TimeValue (TimeValue const & t)
		{
			_t.tv_sec = t._t.tv_sec;
			_t.tv_usec = t._t.tv_usec;
		}

		/**
		 * @brief Constructor.
		 *
		 * Assigns values of the given structure.
		 *
		 */
		TimeValue (timeval const & t)
		{
			_t.tv_sec = t.tv_sec;
			_t.tv_usec = t.tv_usec;
		}

		/**
		 * @brief Constructor.
		 *
		 * Assigns values of the given structure converted to
		 * the used format.
		 *
		 */
		TimeValue (struct timeb const & tb)
		{
			_t.tv_sec = tb.time;
			_t.tv_usec = 1000 * tb.millitm;
		}

		TimeValue & operator= (TimeValue const & t)
		{
			_t.tv_sec = t._t.tv_sec;
			_t.tv_usec = t._t.tv_usec;
			return *this;
		}

		TimeValue & operator= (timeval const & t)
		{
			_t.tv_sec = t.tv_sec;
			_t.tv_usec = t.tv_usec;
			return *this;
		}
		friend TimeValue operator+ (TimeValue const & t1, TimeValue const & t2);
		friend TimeValue operator- (TimeValue const & t1, TimeValue const & t2);
		friend TimeValue operator/ (TimeValue const & t1, int value);
		friend bool operator< (TimeValue const & t1, TimeValue const & t2);
		friend bool operator> (TimeValue const & t1, TimeValue const & t2);
		friend bool operator== (TimeValue const & t1, TimeValue const & t2);
		friend bool operator!= (TimeValue const & t1, TimeValue const & t2);
		void operator+= (TimeValue const & t)
		{
			_t.tv_sec += t._t.tv_sec;
			_t.tv_usec += t._t.tv_usec;
			Normalize ();
		}
		void operator-= (TimeValue const & t)
		{
			_t.tv_sec -= t._t.tv_sec;
			_t.tv_usec -= t._t.tv_usec;
			Normalize ();
		}

		/**
		 * @brief Returns seconds of the object.
		 */
		long GetSeconds () const { return _t.tv_sec; }

		/**
		 * @brief Returns microseconds of the object without counting
		 * on the seconds.
		 */
		long GetMicroseconds () const { return _t.tv_usec; }

		/**
		 * @brief Returns milliseconds of the object.
		 */
		long_t GetMilliseconds () const
		{
			// 8-byte type
			return ((long_t)_t.tv_sec) * 1000 + (long_t)(_t.tv_usec/1000);
		}

		/**
		 * @brief Returns microseconds of the object.
		 */
		long_t GetTotalMicroseconds () const
		{
			// 8-byte type
			return ((long_t)_t.tv_sec) * 1000000 + (long_t)(_t.tv_usec);
		}

		/**
		 * @brief Sets seconds and microseconds to 0.
		 */
		void Zero ()
		{
			_t.tv_sec = 0;
			_t.tv_usec = 0;
		}

		/**
		 * @brief Sets values to current time.
		 */
		void SetCurrentTime ()
		{
			struct timeb tb;
			::ftime (&tb);
			_t.tv_sec = tb.time;
			_t.tv_usec = 1000 * tb.millitm;
		}

		/**
		 * @brief Returns current time.
		 */
		static TimeValue Now ()
		{
			struct timeb tb;
			::ftime (&tb);
			return tb;
		}
		operator struct timeval * () { return &_t; }

	private:
		void Normalize ();
	
	private:
		struct timeval _t;
	};

	inline bool operator > (TimeValue const & t1, TimeValue const & t2)
	{
		if (t1._t.tv_sec > t2._t.tv_sec)
			return true;
		else if (t1._t.tv_sec == t2._t.tv_sec
				 && t1._t.tv_usec > t2._t.tv_usec)
			return true;
		else
			return false;
	}

	inline bool operator < (TimeValue const & t1, TimeValue const & t2)
	{
		if (t1._t.tv_sec > t2._t.tv_sec)
			return false;
		else if (t1._t.tv_sec == t2._t.tv_sec
				 && t1._t.tv_usec > t2._t.tv_usec)
			return false;
		else
			return true;
	}

	inline bool operator== (TimeValue const & t1, TimeValue const & t2)
	{
		return (t1._t.tv_sec == t2._t.tv_sec) && (t1._t.tv_usec == t2._t.tv_usec);
	}

	inline bool operator!= (TimeValue const & t1, TimeValue const & t2)
	{
		return (t1._t.tv_sec != t2._t.tv_sec) || (t1._t.tv_usec != t2._t.tv_usec);
	}

	inline TimeValue operator+ (TimeValue const & t1, TimeValue const & t2)
	{
		TimeValue sum (t1._t.tv_sec + t2._t.tv_sec,
					   t1._t.tv_usec + t2._t.tv_usec);
		sum.Normalize ();
		return sum;
	}

	inline TimeValue operator- (TimeValue const & t1, TimeValue const & t2)
	{
		TimeValue delta (t1._t.tv_sec - t2._t.tv_sec,
					   t1._t.tv_usec - t2._t.tv_usec);
		delta.Normalize ();
		return delta;
	}

	inline TimeValue operator/ (TimeValue const & t1, int value)
	{
		TimeValue div (t1._t.tv_sec / value, t1._t.tv_usec / value);
		div.Normalize ();
		return div;
	}
}

#endif
