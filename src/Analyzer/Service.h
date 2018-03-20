
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
// Service.h
// 
// Service - holds a list of EventCollHandler
// 
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#ifndef SERVICE_H
#define SERVICE_H

//#include "ECPHandler.h"
//#include "EventChannel.h"
#include "Reactor.h"
#include "Syslog.h"
#include <vector>

class ECPHandler;

/**
 * @brief Provides methods to work with EventCollectorHandlers lists.
 * Holds a list of EventCollHandler and a reference to the reactor.
 * Provides methods to add and remove handlers from the list.
 */
class Service {
	typedef vector<ECPHandler *>::iterator Iter;
	
	public:
		/**
		 * @brief Constructor.
		 * @param reactor Reactor of the application
		 */
		Service (Reactor & reactor)
			: _reactor (reactor)
		{}

		/**
		 * @brief Destructor, deletes the handlers and the references to them.
		 */
		~Service()
		{
			Syslog::Debug ("Handler list contains %d elements.", _list.size ());
			//Delete handlers
			for (int i = 0; i <_list.size(); i++)
				delete &_list[i];
			_list.clear ();
			Syslog::Debug ("Handler list contains %d elements.", _list.size ());
		}

		/**
		 * @brief Adds a handler to the list and sets its service to this.
		 * @param handler	ECP Handler.
		 */
		void Add(ECPHandler * handler);
	
		/**
		 * @brief Unregisters the handler from the reactor and removes it from the list.
		 * @param handler ECP Handler
		 * @throws Exception when the handler does not exist in the list.
		 */
		void Remove(ECPHandler * handler);

	private:
		//EventChannel 					_channel;
		std::vector<ECPHandler*>		_list;
		Reactor &						_reactor;
};

#endif

