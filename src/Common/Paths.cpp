
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
// Paths.cpp
//
// Anna Sikora, UAB, 2006
//
//----------------------------------------------------------------------

#include "Paths.h"
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string>

using namespace std;

// recursive mkdir
// returns 0 on success and -1 on error. errno indicates the error.
int Common::MakeDirectory (string const & path)
{
    string dir;
    char buffer [MAXPATHLEN+1];
    strcpy (buffer, path.c_str());
    char * pTok = strtok (buffer, "/");
    while (pTok != 0)
    {
		if (strlen (pTok) == 0)
		{
			dir += "/";
			continue; // skip ""
		}
		dir = dir + "/" + pTok;
		int status = mkdir (dir.c_str(), S_IRWXU | S_IRWXG);
		if (status == -1 && errno != EEXIST)
			return -1;
		pTok = strtok (NULL, "/");
    }
    return 0; // path created.
}

