
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

// MATE
// Copyright (C) 2002-2008 Ania Sikora, UAB.

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef __ADDRESS_H__
#define __ADDRESS_H__

// Local includes
#include "SysException.h"

// C++ includes
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <cctype>
#include <netinet/in.h>
#include <signal.h>
#include <netdb.h>

namespace Common {
	/**
	 * @class Address
	 * @brief Encapsulates a socket address of the AF_INET family.
	 *
	 * This class contains methods to initialize the address of a socket.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */
	
	// ------------------------------------------------------------
	class Address
	{
	public:
		/**
		 * @brief Constructor.
		 *
		 * @param host Host where the socket will be located.
		 * @param port Port used.
		 *
		 * @throws SysException
		 */
		Address (std::string const & host, int port);

		/**
		 * @brief Constructor.
		 *
		 * Uses the INADDR_ANY address.
		 *
		 * @param port Port used.
		 */
		Address (int port);

		/**
		 * @brief Constructor.
		 *
		 * Initializes an empty address, setting the memory
		 * of the object to 0.
		 *
		 */
		Address ();

		/**
		 * @brief Returns a pointer to the sockaddr intern structure.
		 */
		operator struct sockaddr * () {	return (sockaddr*)&_addr; }

		/**
		 * @brief Returns a pointer to the sockaddr_in intern structure.
		 */
		operator struct sockaddr_in * () { return &_addr; }

		/**
		 * @brief Returns size of current address.
		 * Number of bytes the address uses in memory.
		 */
		socklen_t GetSize () const { return sizeof (_addr);	}

		/**
		 * @brief Returns name of host.
		 * @throws SysException
		 */
		std::string GetHostName () const;

	private:
		//Analyzes a string and indicates if it has an IP Address
		static bool IsIPAddress (std::string const & str);

		struct sockaddr_in 	_addr;
	};
}

#endif
