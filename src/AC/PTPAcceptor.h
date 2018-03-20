
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
// PTPAcceptor.h
// 
// PTPAcceptor - waits for and accepts coming solutions from analyzer
// Communicator module
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#ifndef PTPACCEPTOR_H
#define PTPACCEPTOR_H

#include "Socket.h"
#include "EventHandler.h"
#include "Reactor.h"
#include "TaskManager.h"
#include <strstream>

/**
 * @class PTPAcceptor
 * @brief Manages socket connection and handles data input through them.
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class PTPAcceptor : public EventHandler
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param reactor Object of class reactor that manages event handlers.
	 * @param tm Task manager.
	 */
	PTPAcceptor (Reactor & reactor, TaskManager & tm, int  port);
	
	/**
	 * @brief Destructor
	 */
	~PTPAcceptor ()
	{
		_reactor.UnRegister (*this);
	}
	
	/**
	 * @brief Gets the socket for the client and binds it with the task manager.
	 */
	void HandleInput ();

	/**
	 * @brief Getter of a handler for the variable _socket.
	 * @return Handle of the server socket.
	 */
	int GetHandle ()
	{
		return _socket.GetHandle ();
	}
	
private:
	ServerSocket 	_socket;
	Reactor	    &	_reactor;
	TaskManager &	_tm;
};

#endif
