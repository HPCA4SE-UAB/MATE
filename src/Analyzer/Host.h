
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
// Host.h
// 
// Host stuff
//
// Anna Sikora, UAB, 2004
//
//----------------------------------------------------------------------

#if !defined HOST_H
#define HOST_H

#include "auto_vector.h"//ok
//#include "Utils.h"
//#include "sync.h"
//#include "EventCollector.h"
//#include "Syslog.h"
//#include "Queue.h"
//#include "ECPMsg.h"
//#include "AppEvent.h"
//#include "ACProxy.h"
#include <vector>
//#include <map>
#include <string>

namespace Model {
	/**
	 * @brief Encapsulates host information.
	 * Basically consists in a string with the name of the host
	 * and a method to access it.
	 */
	class Host {
		friend class Application;
		public:
			/**
			 * @return Name of the host
			 */
			string GetName() const { return _name; }
			//Tasks & GetTasks () { return _tasks; }
		protected:
			/**
			 * @brief Constructor.
			 */
			Host(string const & name) : _name(name) {}
		private:
			string 	_name;
	};

	typedef auto_vector<Host> Hosts;

	/**
	 * @brief Provides mechanisms to handle the addition and removing of
	 * hosts.
	 */
	class HostHandler {
		public:
			/**
			 * @brief Called when a new host is added to the virtual machine.
			 * @param h Added host.
			 */
			virtual void HostAdded(Host & h) = 0;
			/**
			 * @brief Called when a host is removed from the virtual machine.
			 * @param h	Removed host.
			 */
			virtual void HostRemoved(Host & h) = 0;
	};

	typedef std::vector <HostHandler *> HostHandlers;
}//End namespace Model
#endif /* HOST_H_ */
