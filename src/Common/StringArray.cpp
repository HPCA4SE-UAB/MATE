
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
//
// StringArray.cpp
//
// ------------------------------------------------------------------

#include "StringArray.h"

using namespace std;
using namespace Common;

StringArray::StringArray (int size)
			: _size (size), _count (0), _arr (0)
{
	if (_size > 0)
	{
		_arr = new char * [_size];
		for (int i=0; i<_size; i++)
			_arr [i] = 0;
	}
}

StringArray::~StringArray ()
{
	for (int i=0; i<_count; i++)
	{
		delete [] _arr [i];
	}
	delete [] _arr;
}

void StringArray::AddString (char const * s)
{
	assert (_count < _size);
	//std::cout << "StringArray adding: [" << s << "]" << std::endl;
	if (s != 0)
	{
		int len = strlen (s);
		char * str = new char [len+1];
		strcpy (str, s);
		str [len] = '\0';
		_arr [_count++] = str;
	}
	else
	{
		_arr [_count++] = 0;
	}
}

void StringArray::Grow (int newSize)
{
	assert (newSize > _size);
	// allocate new array
	char ** newArr = new char * [newSize];
	// copy strings from old array to new array
	for (int i=0; i<_count; i++)
		newArr [i] = _arr [i];
	// fill the rest of array with 0's
	for (int j=_count; j<newSize; j++)
		newArr [j] = 0;
	// delete old array
	delete [] _arr;
	// switch arrays
	_arr = newArr;
	_size = newSize;
}

char const * StringArray::GetString (int idx) const
{
	assert (idx >= 0 && idx < _size);
	return _arr [idx];
}

void StringArray::Dump () const
{
	cout << "StringArray Dump" << endl;
	if (_arr == 0)
	{
		cout << "_arr == 0" << endl;
		return;
	}
	cout << "StringArray Size = " << _size << endl;
	cout << "StringArray Count = " << _count << endl;
	for (int i=0; i<_count; i++)
	{
		cout << "\tElement " << i << " = " << _arr [i] << endl;
	}
}
