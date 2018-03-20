
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
// Env.cpp
//
// Anna Sikora, UAB, 2006
//
//----------------------------------------------------------------------

#include <sys/param.h>
#include "Env.h"

using namespace std;

string Common::ExpandPath (string const & path)
{
    // first substitute $XXXX envirnment variables with their values
    string output;
    size_t len = path.length ();
    size_t i = 0;
    while (i<len)
    {
	int c=path [i];
	if (c == '$')
	{
	    // extract ennviroment variable name
	    string var;
	    ++i; // skip $
	    while (i<len)
	    {
		int d=path [i];
		if (d=='/')
		    break;
		else
		    var.append (1, d);
		++i;    
	    }
	    char * envVal = getenv (var.c_str());
	    if (envVal != 0)
		output.append (envVal);
	}
	else
	{
	    output.append (1, c);
	    ++i;
	}
    }
    // path might be a filename, relative path or absolute path
    //char buffer [MAXPATHLEN+1];
    //char * result = realpath (output.c_str(), buffer);
    //if (result!=0)
//	return result;
//    else
//	return "";
    return output;
}


