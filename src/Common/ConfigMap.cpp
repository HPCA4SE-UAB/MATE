
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

//----------------------------------------------------------------------
//
// ConfigMap.cpp
//
//----------------------------------------------------------------------

#include "ConfigMap.h"

using namespace std;
using namespace Common;

bool ConfigMap::Add (string const & section, string const & key,
		string const & value)
{
	if (this->Contains(section, key)){
		return false;
	} else {
		string iKey = MakeKey (section, key);
		_map.insert (StringMap::value_type (iKey, value));
		return true;
	}
}

string const & ConfigMap::GetValue (string const & section,
									  string const & key) const
{
	string iKey = MakeKey (section, key);
	StringMap::const_iterator theIterator;

	theIterator = _map.find (iKey);
	if (theIterator != _map.end ())    // value exists
	{
		return (*theIterator).second;
	}
	else
	{
		cerr << "key " << key.c_str () << " not found in section "
				<< section.c_str () << endl;
		throw ConfigException("Key not found", key);
	}
}

bool ConfigMap::Contains (string const & section, string const & key) const
{
	string iKey = MakeKey (section, key);
	StringMap::const_iterator it = _map.find (iKey);
	return it != _map.end ();
}

void ConfigMap::Iterator::Next ()
{
	assert (_iter != _map.end ());
	_iter++;
}

string ConfigMap::Iterator::GetSection () const
{
	assert (!AtEnd ());
	string const & s = (*_iter).first;
	int pos = s.find (".");
	assert (pos != string::npos);
	return s.substr (0, pos);
}

string ConfigMap::Iterator::GetKey () const
{
	assert (!AtEnd ());
	string const & s = (*_iter).first;
	int pos = s.find (".");
	assert (pos != string::npos);
	return s.substr (pos+1, s.length () - pos - 1);
}

string const & ConfigMap::Iterator::GetValue () const
{
	assert (!AtEnd ());
	return (*_iter).second;
}
