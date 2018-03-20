
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
// ECPProtocol.h
// 
// ECPProtocol - 
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#ifndef ECPPROTOCOL_H
#define ECPPROTOCOL_H

#include "ECPMsg.h"
#include "NetSer.h"
#include "Socket.h"
#include <strstream>

/**
 * @brief Encapsulates methods to read and handle incoming network messages.
 */
class ECPProtocol {
	public:
		//static void WriteMessageEx (ECPMessage const & msg, Socket & sock);

		/**
		 * @brief Reads a message header from the socket, deserializes it and
		 * creates a message header object.
		 *
		 * @param sock	Reference to the socket.
		 *
		 * @return Reference to the message header object created
		 */
		static ECPMsgHeader ReadMessageHeader (Socket & sock);

		/**
		 * @brief Reads a message from the socket, deserializes it and creates
		 * different kind of message objects depending on their type.
		 *
		 * @param sock	Reference to the socket.
		 *
		 * @return Reference to the message object created.
		 */
		static ECPMessage * ReadMessageEx (Socket & sock);
};

#endif

