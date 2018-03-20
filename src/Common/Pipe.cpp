
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
// Pipe.cpp
//
//----------------------------------------------------------------------

#include "Pipe.h"
#include "SysException.h"
#include <assert.h>

using namespace Common;

Pipe::Pipe ()
{
	// create pipe
	int status = ::pipe (_fds);
	if (status == -1)
	{
		cout << "Pipe creation failed" << endl;
		throw SysException ("Pipe creation failed");
	}
}

Pipe::~Pipe ()
{
	CloseRead ();
	CloseWrite ();
}

int Pipe::Read (char * buf, int bufSize)
{
	assert (_fds [fdRead] != -1);
	for (int i=0; i<bufSize; i++)
		buf [i] = 0;
	int bytesRead = ::read (_fds [fdRead], buf, bufSize);
	if (bytesRead < 0)
	{
		cout << "Pipe read failed" << endl;
		throw SysException ("Pipe read failed");
	}
	return bytesRead;
}

int Pipe::Write (char const * buf, int bufSize)
{
	assert (_fds [fdWrite] != -1);
	//cout << buf << endl;
	int bytesWritten = ::write (_fds [fdWrite], buf, bufSize);
	if (bytesWritten < 0)
	{
		cout << "Pipe write failed" << endl;
		throw SysException ("Pipe write failed");
	}
	return bytesWritten;
}

void Pipe::Close (Descriptor d)
{
	if (_fds [d] != -1)
	{
		::close (_fds [d]);
		_fds [d] = -1;
	}
}
