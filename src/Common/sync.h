
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

#ifndef __SYNC_H__
#define __SYNC_H__

// Local includes
#include "SysException.h"

// C++ includes
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>

namespace Common {
	/**
	 * @class Semaphore
	 * @brief Synchronizes access to a resource.
	 *
	 * A semaphore is generally used as a synchronization object between multiple
	 * threads or to protect a limited and finite resource such as a memory or
	 * thread pool.  The semaphore has a counter which only permits access by
	 * one or more threads when the value of the semaphore is non-zero.  Each
	 * access reduces the current value of the semaphore by 1.  One or more
	 * threads can wait on a semaphore until it is no longer 0, and hence the
	 * semaphore can be used as a simple thread synchronization object to enable
	 * one thread to pause others until the thread is ready or has provided data
	 * for them.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class Semaphore
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param initialValue Initial value of the semaphore, default 0.
		 * @param crossProcess Flag indicating whether or not the semaphore should
		 * be shared with forked processes, default false.
		 */
		Semaphore (int initialValue = 0, bool crossProcess = false)
		{
			int status = ::sem_init (&_semaphore, crossProcess, initialValue);
			if (status == -1)
				throw SysException("Semaphore creation failed");
		}

		/**
		 * @brief Destructor.
		 */
		~Semaphore ()
		{
			::sem_destroy (&_semaphore);
		}

		/**
		 * @brief Stops the current thread until the access to the
		 * resource is open.
		 *
		 * Wait is used to keep a thread held until
		 * the semaphore counter is greater than 0.
		 * If the current thread is held, then another
		 * thread must increment the semaphore.
		 * Once the thread is accepted, the semaphore
		 * is automatically decremented,
		 * and the thread continues execution.
		 *
		 */
		void Wait ();

		/**
		 * @brief Returns true if the access to the resource is open,
		 * false otherwise.
		 *
		 * TryWait is a non-blocking variant of Wait.
		 * If the semaphore counter is greater than 0,
		 * then the thread is accepted and the semaphore
		 * counter is decreased. If the semaphore counter
		 * is 0 TryWait returns immediately with false.
		 *
		 */
		bool TryWait ();

		/**
		 * @brief Gives a signal to the semaphore indicating that a client
		 * has stopped using the resource.
		 *
		 * Posting to a semaphore increments its current
		 * value and releases the first thread waiting
		 * for the semaphore if it is currently at 0.
		 *
		 */
		void Post ();

	protected:
		Semaphore (sem_t s) : _semaphore (s) {}

	protected:
		sem_t _semaphore;	// semaphore handle
	};

	/**
	 * @class Mutex
	 * @brief Guarantees non concurrent access to a resource.
	 *
	 * Mutual-Exclusion Object
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class Mutex
	{
		friend class MutexLock;
	public:

		/**
		 * @brief Constructor.
		 *
		 * The mutex is always initialized as a recursive entity.
		 *
		 */
		Mutex ()
		{
			int status = ::pthread_mutex_init (&_mutex, 0);
			if (status != 0)
				throw SysException("Mutex creation failed");
		}

		/**
		 * @brief Destructor.
		 */
		~Mutex()
		{
			::pthread_mutex_destroy (&_mutex);
		}

		operator pthread_mutex_t * () { return &_mutex; }

	protected:

		/**
		 * @brief Denotes that someone starts using the resource.
		 * @throws SysException
		 */
		void Enter ();

		/**
		 * @brief Returns true if the resource is not being used, false otherwise.
		 */
		bool CanEnter ();

		/**
		 * @brief Denotes that someone stops using the resource.
		 * @throws SysException
		 */
		void Leave ();

	private:
		pthread_mutex_t	_mutex;
	};

	/**
	 * @class MutexLock
	 * @brief System to manage access to a resource with a mutex.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	// Mutex lock
	class MutexLock
	{
	public:

		/**
		 * @brief Constructor.
		 */
		MutexLock (Mutex & mutex)
			: _mutex (mutex)
		{
			_mutex.Enter ();
		}

		/**
		 * @brief Destructor.
		 */
		~MutexLock ()
		{
			_mutex.Leave ();
		}
	private:
		Mutex & _mutex;
	};
}

#endif
