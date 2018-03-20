
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
// Config.cpp
// ------------------------------------------------------------------

#include "Config.h"

using namespace std;
using namespace Common;

int Config::GetIntValue (string const & section, string const & key) const
{
	string value = _map.GetValue (section, key);

	//find if number is 0
	string whitespaces (" \t\f\v\n\r");
	int first = value.find_first_not_of(whitespaces);
	if (first == string::npos)	first = 0;
	int last = value.find_last_not_of(whitespaces);
	if (last == string::npos) last = value.size();

	if (value.find_first_not_of("0") == string::npos)
		return 0;

	//if it's not found, the return value 0 will mean failure.
	int intValue = atoi (value.c_str ());
	if (intValue != 0){
		return intValue;
	} else {
		char buffer [255];
		sprintf (buffer, "key %s of section %s doesn't contain a "
				"valid integer value", key.c_str(), section.c_str());
		throw ConfigException(buffer);
	}
}

bool Config::GetBoolValue (string const & section, string const & key) const
{
	string const & value = _map.GetValue (section, key);
	if (value == "true" || value == "TRUE" || value == "1"){
		return true;
	} else if (value == "false" || value == "FALSE" || value == "0") {
		return false;
	} else {
		char buffer [255];
		sprintf (buffer, "key %s of section %s doesn't contain a "
				"valid boolean value", key.c_str(), section.c_str());
		throw ConfigException(buffer);
	}
}

void Config::KeyIterator::Next ()
{
	assert (!AtEnd ());
	while (true)
	{
		_iter.Next ();
		if (_iter.AtEnd ())
			return;
		if (_section.compare (_iter.GetSection ()) == 0)
			return;
	}
}

string Config::KeyIterator::GetKey () const
{
	assert (!AtEnd ());
	return _iter.GetKey ();
}

string const & Config::KeyIterator::GetValue () const
{
	assert (!AtEnd ());
	return _iter.GetValue ();
}

int Config::KeyIterator::GetIntValue () const
{
	assert (!AtEnd ());
	string const & value = _iter.GetValue ();
	return atoi (value.c_str ());
}
