
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

#ifndef __AUTO_VECTOR_H__
#define __AUTO_VECTOR_H__

#include <iterator>
#include "stdio.h"
#include <cassert>
#include <memory>
//#include "auto_ptr.h"

/**
 * @class 	auto_vector
 * @brief	Class with the methods to create, handle and destroy an array of _auto_ptr_ elements.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */

using namespace std;

template <class T>
class auto_iterator;

template <class T>
class auto_vector
{
public:

	/**
	 * @brief Constructor
	 * @param capacity Vector capacity. By default this is 0
	 */
	explicit auto_vector (size_t capacity = 0)
			: _capacity (capacity), _end (0)
	{
		_arr = new auto_ptr<T> [_capacity];
	}

	/**
	 * @brief Destructor
	 */
	~auto_vector ()
	{
		delete[] _arr;
	}

	T const * operator [] (size_t i) const
	{
		return _arr [i].get ();
	}

	T * operator [] (size_t i)
	{
		return _arr [i].get ();
	}

	/**
	 * @brief Assigns _p_ to the _ith_ position in the auto_vector array _arr
	 * @param i Position where to make the assignment
	 * @param p Data that will be assigned
	 */
	void assign (size_t i, auto_ptr<T> & p)
	{
		assert (i < _end);
		_arr [i] = p;
	}

	/**
	 * @brief Deletes position _i_ in the array
	 * @param i
	 * @param p
	 */
	void remove_direct (size_t i, T * p)
	{
		//Dump ();
		/*assert (i < _end);
		auto_ptr<T> ptr (p);
		_arr [i] = ptr;*/
		T * t = _arr [i].release ();
		delete t;
		assert (_arr [i].get () == 0);
		--_end;
		//Dump ();
	}

	/**
	 * @brief Prints the contents in the auto_vector array _arr
	 */
	void Dump ()
	{
		for (int j=0; j<_end; j++)
			fprintf (stderr, "[auto_vector] _arr [%d] = %p\n", j, _arr [j].get ());
	}

	/**
	 * @brief Deletes all contents in the auto_vector array _arr
	 */
	void clear ()
	{
		for (size_t i = 0; i<_end; ++i)
		{
			remove_direct (i, 0);
		}
		_end = 0;
	}

	/**
	 * @brief Pushes an element _p_ at the end of the auto_vector array _arr
	 * @param p
	 */
	void push_back (auto_ptr<T> & p)
	{
		//Dump ();
		if (_end == _capacity)
			grow (_end + 1);
		_arr [_end++] = p;

		//Dump ();
	}

	/**
	 * @brief Pops the last element of the auto_vector array _arr
	 * @return Last element in _arr
	 */
	auto_ptr<T> pop_back ()
	{
		assert (_end > 0);
		return _arr [--_end];
	}

	/**
	 * @brief Gets the contents from vector _arr in the given position
	 * @param i Position of the element we want to get from _arr
	 * @return Contents of the _ith_ element in _arr
	 */
	auto_ptr<T> acquire (size_t i)
	{
		assert (i < _end);
		return _arr [i];
	}

	typedef auto_iterator<T> iterator;

	/**
	 * @brief Returns a pointer to the first position in the array _arr
	 * @return First position in _arr
	 */
	iterator begin () const { return _arr; }

	/**
	 * @brief Returns a pointer to the last position in the array _arr
	 * @return Ending position in _arr
	 */
	iterator end () const { return _arr + _end; }

	/**
	 * @brief Returns the size of _arr
	 * @return Number of elements in _arr
	 */
	int size () const { return _end; }

private:
	void grow (size_t reqCapacity)
	{
		// calc size of a new array
		size_t newCapacity = _capacity * 2;
		if (newCapacity < reqCapacity)
			newCapacity = reqCapacity + 1;
		// create new array
		auto_ptr<T>	*newArr = new auto_ptr<T> [newCapacity];
		// transfer objects from the old to the new array
		for (int i=0; i<_end; i++)
			newArr[i] = _arr[i];
		// delete old array
		delete[] _arr;
		// switch arrays
		_arr = newArr;
		_capacity = newCapacity;
	}

private:
	auto_ptr<T>	*_arr;
	size_t		 _capacity;
	size_t		 _end;
};

using namespace std;

template <class T>
class auto_iterator;


/**
 * @class 	auto_iterator
 * @brief	Class that implements the _auto_ptrs_ operators.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */
template <class T>
class auto_iterator //: public iterator<random_access_iterator_tag, T *>
{
public:
	auto_iterator () : _pp (0) {}
	auto_iterator (auto_ptr<T> * pp) : _pp (pp) {}
	
	bool operator != (auto_iterator<T> const & it) const
	{
		return it._pp != _pp;
	}

	auto_iterator const & operator++ (int)
	{ 
		_pp++;
		return *this;
	}
	auto_iterator operator++ () { return ++_pp; }
	T * operator * () { return _pp->get (); }
	T const * operator * () const { return _pp->get (); }
	T * operator-> () { return _pp->get (); }
private:
	auto_ptr<T> * _pp;
};

#endif
