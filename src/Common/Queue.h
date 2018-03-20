
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

#ifndef __QUEUE_H__
#define __QUEUE_H__

// Local includes
#include "sync.h"

namespace Common {
	/**
	 * @class Queue
	 * @brief Data structure that stores objects of any class.
	 *
	 * This data structure manages the objects using a FIFO priority.
	 *
	 * @version 1.0
	 * @since 1.0
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	template <class T>
	class Queue
	{
	public:

		/**
		 * @brief Constructor.
		 */
		Queue (int maxSize);

		/**
		 * @brief Destructor.
		 */
		~Queue ();

		/**
		 * @brief Returns true if the queue is empty, false otherwise.
		 */
		bool IsEmpty () const { return _count == 0; }

		/**
		 * @brief Returns true if the queue is full, false otherwise.
		 */
		bool IsFull () const { return _count == _maxSize; }

		/**
		 * @brief Returns the maximum quantity of objects the queue can store.
		 */
		int GetMaxSize () const { return _maxSize; }

		/**
		 * @brief Returns current size of the queue.
		 */
		int GetCount () const { return _count; }

		/**
		 * @brief Returns first object of the queue and removes it.
		 *
		 * Returns false if the queue is empty.
		 *
		 */
		bool Get (T & item);

		/**
		 * @brief Returns first object of the queue and removes it.
		 *
		 * If the queue is empty, waits until it has any object.
		 *
		 */
		void GetB (T & item);

		/**
		 * @brief Puts an object at the end of the queue.
		 *
		 * If the queue is full, it waits until there's space.
		 *
		 */
		void Put (T & item);

	private:
		void LokPut (T & item);
		T LokGet ();

	private:
		// we don't want to copy the queues
		Queue & operator= (Queue const & queue) {}

	private:
		T *			_arr;
		int			_maxSize;
		int			_count;
		int			_head;
		int			_tail;
		Semaphore	_full;	// counts number of full buffer slots
		Semaphore	_empty;	// counts number of empty buffer slots
		Mutex		_mutex;	// Controls access to critical section
	};
}

using namespace Common;

template <class T>
Queue<T>::Queue (int maxSize)
	: _maxSize (maxSize), _head (0), _tail (0), _count (0),
	  _full (0), _empty (maxSize)
{
	_arr = new T [maxSize];
	if (_arr == 0)
		throw "Can't allocate queue";
}

template <class T>
Queue<T>::~Queue ()
{
	delete [] _arr;
}

template <class T>
bool Queue<T>::Get (T & item)
{
	// wait for an element
	if (_full.TryWait ())
	{
		item = LokGet ();
		_empty.Post ();
		return true;
	}
	return false;
}

template <class T>
void Queue<T>::GetB (T & item)
{
	// wait for an element
	_full.Wait ();
	item = LokGet ();
	_empty.Post ();
}


template <class T>
void Queue<T>::Put (T & item)
{
	// wait for a free slot
	_empty.Wait ();
	// add element
	LokPut (item);
	// signal new element
	_full.Post ();
}

template <class T>
void Queue<T>::LokPut (T & item)
{
	MutexLock lock (_mutex);
	// put the element
	_arr [_tail] = item;
	_tail = (_tail + 1) % _maxSize;
	++_count;
}

template <class T>
T Queue<T>::LokGet ()
{
	MutexLock lock (_mutex);
	T item = _arr [_head];
	_head = (_head + 1) % _maxSize;
	--_count;
	return item;
}

#endif
