
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

#ifndef __ACTIVEOBJECT_H__
#define __ACTIVEOBJECT_H__

// Local includes
#include "Thread.h"

namespace Common {
	/**
	 * @class ActiveObject
	 * @brief Abstract class, encapsulates OS thread (pthreads) POSIX compatible.
	 *
	 * Last thing in the constructor of a class derived from ActiveObject
	 * must be a call to <i>_thread.Resume();</i><br>
	 * Inside the loop the Run method must keep checking <i>_isDying</i>:
	 * @code
	   if (_isDying)
	       return;
	   \endcode
	 *
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class ActiveObject
	{
	public:
		/**
		 * @brief Constructor.
		 */
		ActiveObject ();

		/**
		 * @brief Destructor.
		 */
		virtual ~ActiveObject () {}

		/**
		 * @brief Stops the thread execution.
		 */
		void Kill ();

	protected:

		virtual void InitThread () = 0;
		virtual void Run () = 0;
		virtual void FlushThread () = 0;

		/**
		 * @brief Continues with the execution of the thread.
		 */
		void Resume ();

	protected:
		int         _isDying;

	private:
		void SignalStartup ();
		void WaitForStartup ();
		void WaitForResume ();

	private:
		static void * ThreadEntry (void *pArg);
		Semaphore	_startSem;	// controls thread startup
		Semaphore	_resumeSem;	// controls thread resume
		Thread      _thread;
		bool		_resumed;
	};
}

#endif
