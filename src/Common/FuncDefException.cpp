
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

// ----------------------------------------------
// EventException.cpp
// ----------------------------------------------

#include "FuncDefException.h"
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace Common;

void FuncDefException::Display (std::ostream & os) const
{
	os << "FuncDef exception: " << _msg << endl
	   << "Error number: " << _err << endl
	   << "Reason: " << GetReason () << endl
	   << "Object: " << _objName << endl;
	os.flush ();
}

void FuncDefException::Display () const
{
	cerr << "FuncDef exception: " << _msg << endl
	   << "Error number: " << _err << endl
	   << "Reason: " << GetReason () << endl
	   << "Object: " << _objName << endl;
	cerr.flush ();
}

string FuncDefException::GetReason () const
{
	char * reason = strerror (_err);
	if (reason == 0)
		return "";
	return reason;
}
