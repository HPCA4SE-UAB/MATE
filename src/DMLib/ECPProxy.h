
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

#ifndef __ECPROXY_H__
#define __ECPROXY_H__

//Local includes
#include "Socket.h"
#include "ECPMsg.h"

// C++includes
#include <stdarg.h>

namespace DMLib {
	/**
	 * @class EventCollectorProxy
	 * @brief Connects to the analyzer host and sends requests.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class EventCollectorProxy
	{

	public:

		/**
		 * @brief Constructor.
		 */
		EventCollectorProxy (std::string const & host, int port)
		  : _socket (host, port)
		{
			// disable Nagle's algorithm
			_socket.SetTCPNoDelay (true);
		}

		/**
		 * @brief Destructor.
		 */
		~EventCollectorProxy () {}

		/**
		 * @brief Sends a request to the Analyzer to register a
		 * new worker.
		 */
		void RegisterLib (int pid, int mpiRank,
				std::string host, std::string taskName, int ACport);

		/**
		 * @brief Sends a message to the analyzer.
		 */
		void SendEvent (EventMsg const & event);

		/**
		 * @brief Sends a request to the Analyzer to unregister a
		 * worker.
		 */
		void UnregisterLibrary (int pid);

	private:
		void WriteMessage (ECPMessage const & msg);

	private:
		Socket 	_socket; 						// analyzer socket
		char 	_sendBuffer [1024 * 64];		// buffer for data to be sent
	};
}

#endif
