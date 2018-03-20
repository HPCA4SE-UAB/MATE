
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

/*
 * ShutDownManager.h
 *
 *  Created on: Jul 5, 2011
 *      Author: rodrigo
 */

#ifndef SHUTDOWNMANAGER_H_
#define SHUTDOWNMANAGER_H_

#include "ActiveObject.h"
#include "AppModel.h"

using namespace Common;
/**
 * @brief Handles the shut down of MATE (Analyzer and AC's)
 * The data structure consists basically in a reference to the application model
 * (to know the hosts where the AC's are running in real time) and a boolean to
 * determine if MATE is finished (to let the main process know, and make it stop).
 * Provides a method to set the application model from outside (when it is ready,
 * the main process of the Analyzer will set it).
 * On the other hand, this class inherits from ActiveObject, so its objects are
 * execution threads, this is done to wait for the user to stop MATE without
 * stopping its own execution.
 */
class ShutDownManager : public ActiveObject {
	public:

		/**
		 * @brief Constructor. Sets the finished member to false and starts the thread.
		 */
		ShutDownManager();

		/**
		 * @brief Destructor.
		 */
		virtual ~ShutDownManager();

		/**
		 * @brief Function that is executed by the thread. Waits for the user
		 * to stop MATE, when receives the commandment sends a stop signal
		 * to AC's and sets the variable finished to true in order to stop
		 * the Analyzer itself.
		 */
		void Run();

		/**
		 * @brief Not implemented (Here for compatibility reasons).
		 */
		void InitThread (){}

		/**
		 * @brief Not implemented (Here for compatibility reasons).
		 */
		void FlushThread (){}

		/**
		 * @brief Getter of finished boolean.
		 * @return True if the user stopped MATE, false otherwise.
		 */
		bool isFinished (){ return _finished; }

		/**
		 * @brief Application model reference setter.
		 * @param app	Reference to the application model
		 */
		void setApp(Model::Application & app){_app = &app;}
	private:
		char endExecution;
		Model::Application * _app;//reference to the objective application
					  	  	  	   //so we can get the AC hosts.
		bool _finished;
		static const int SIGNALS_PORT = 8080;
};

#endif /* SHUTDOWNMANAGER_H_ */
