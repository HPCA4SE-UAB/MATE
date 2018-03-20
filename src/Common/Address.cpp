
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
// Address.cpp
// ------------------------------------------------------------------

#include "Address.h"
#include <netdb.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <string.h>

using namespace std;
using namespace Common;

// Helper:
//  struct hostent 
//  {
//         char    *h_name;         /* canonical name of host */
//         char    **h_aliases;     /* alias list */
//         int     h_addrtype;      /* host address type */
//         int     h_length;        /* length of address */
//         char    **h_addr_list;   /* list of addresses */
//  };
     
// for example host = "192.9.199.59" or host = "aows10.uab.es"
Address::Address (string const & host, int port)
{

	::memset (&_addr, 0, sizeof (_addr));
	if (IsIPAddress (host))
	{		
		_addr.sin_family = AF_INET;
		_addr.sin_port = htons ((unsigned short)port);
		unsigned long address =  ::inet_addr (host.c_str ());
		if (address == -1)
			throw SysException ("Invalid IP address", host);
		_addr.sin_addr.s_addr = address;
	}
	else
	{
		struct hostent * pHostEntry = ::gethostbyname (host.c_str ());


		//std::cout << "IP addresses: "  << std::endl;
		struct in_addr **addr_list;
        addr_list = (struct in_addr **)pHostEntry->h_addr_list;
        /*for(int i = 0; addr_list[i] != NULL; i++) {
            std::cout << inet_ntoa(*addr_list[i]) << std::endl;
        }*/
        _addr.sin_family = AF_INET;
		_addr.sin_port = htons ((unsigned short)port);
		unsigned long address =  ::inet_addr (inet_ntoa(*addr_list[0]));

		if (address == -1)
			throw SysException ("Invalid IP address", host);
		_addr.sin_addr.s_addr = address;



        /*
		if (pHostEntry == 0)
			throw SysException ("gethostbyname failed", host);
		_addr.sin_family = AF_INET;
		_addr.sin_port = htons ((unsigned short)port);
		bcopy ((char*)&_addr.sin_addr.s_addr, (char *) pHostEntry->h_addr, pHostEntry->h_length);
		*/

	}
}

Address::Address ()
{
	::memset (&_addr, 0, sizeof (_addr));
}

Address::Address (int port)
{
	::memset (&_addr, 0, sizeof (_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons ((unsigned short)port);
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
}

bool Address::IsIPAddress (string const & str)
{
	if (str.length() == 0)
			return false;
	string::const_iterator iter = str.begin ();
	while (iter != str.end ())
	{
		int c = (*iter);
		if (!((isdigit (c)) || (c == '.')))
			return false;
		iter++;
	}
	return true;
}

string Address::GetHostName () const
{
	struct hostent * pHostEntry = ::gethostent ();
	if (pHostEntry == 0)
			throw SysException ("gethostent failed");
	string str(pHostEntry->h_name);
	return str;
}
