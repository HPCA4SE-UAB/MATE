
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
// configreader.cpp
//
// Anna Sikora, UAB, 2000
//
//----------------------------------------------------------------------

// note: get rid of warning C4786: identifier was truncated 
// to '255' characters...
#pragma warning (disable: 4786)

#include "ConfigReader.h"
#include "Config.h"

using namespace std;
using namespace Common;

Config FileConfigReader::Read ()
{
	char buf[MaxLineLen];
	Config conf;
	while (!_ifs.eof ())
	{
		_ifs.getline (buf, sizeof (buf));
		AnalyzeLine (conf, buf);
	}

	return conf;
}

void ConfigReader::AnalyzeLine (Config& config, string const & line)
{
	// ignore empty lines
	if (line.size() == 0)
		return;
	// skip white chars
	int idx = 0;
	int len = line.size();
	while (isspace (line[idx]) && idx < len)
		idx++;
	// check if line is all white
	if (idx == len)
		return;
	switch (line[idx])
	{
		case '#': 
		{
			// comment - ignore
			break;
		}
		case '[': 
		{
			// section
			ReadSection (config, &line[idx]);
			break;
		}
		default:
		{
			// key/value or error
			ReadKeyValue (config, &line[idx]);
			break;
		}
	}
}

void ConfigReader::ReadSection (Config& config, string const & line)
{
	int idx = 0;
	int len = line.size();
	while (line[idx] != ']' && idx < len)
		idx++;
	if (idx == len)
	{
		string str;
		str.append("line: ", 6);
		str.append(line);
		throw ConfigException ("Syntax error in config file. Expected ']'", str);
	}
	_section.assign (&line[1], idx-1);
}

void ConfigReader::ReadKeyValue (Config& config, string const & line)
{
	size_t idxEqual = line.find('=');
	if (idxEqual != string::npos)
	{
		if (idxEqual == 0)
		{
			string str;
			str.append("line: ", 6);
			str.append(line);
			throw ConfigException("Syntax error in config file. Unexpected '='", str);
		}
		// key=value or key=
		_key.assign (line.substr(0,idxEqual));
		// rtrim the key
		_key.assign (TrimStr(_key));
		size_t len = line.size();
		if (len == idxEqual+1)
		{
			// key=, value is empty
			_value.assign ("");
		}
		else
		{
			// value is not empty
			// skip white chars
			size_t idx = idxEqual+1;
			while (isspace (line[idx]) && idx < len)
				idx++;
			// check if value is all white
			if (idx == len)
			{
				// key=            , value is empty
				_value.assign ("");
			}
			else
			{
				_value.assign (line.substr(idx, len - idx));
				// rtrim the value
				_value.assign (TrimStr(_value));
			}
		}
	}
	else
	{
		// key, value is empty
		_key.assign (TrimStr(line));
		_value.assign ("");
	}
	// add entry to config
	config.AddEntry (_section, _key, _value);
}

// return length of a given string after right trim
string ConfigReader::TrimStr (string str)
{
	string whitespaces (" \t\f\v\n\r");
	size_t found = str.find_last_not_of(whitespaces);

	if (found != string::npos)
	    str.erase(found+1);
	else
	    str.clear();

	return str;
}
