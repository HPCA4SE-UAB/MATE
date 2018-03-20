
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
// Terminator.h
//
// AC shut down manager
//
//----------------------------------------------------------------------

#ifndef SHUTDOWNSLAVE_H_
#define SHUTDOWNSLAVE_H_

#include "ActiveObject.h"
#include "Ctrl.h"
#include "Socket.h"
#include "Config.h"

using namespace Common;

/**
 * @class ShutDownSlave
 * @brief Receives terminating message from Analyzer.
 *
 * Runs a thread that waits blocked for a message from the analyzer.
 * When it receives this message the main loop in the controller is stopped,
 * and subsequently the AC is terminated.
 *
 * Note: the tasks should be deleted at this point.
 *
 * @version 1.1
 * @since 1.1
 */
class ShutDownSlave : public ActiveObject {
public:
	/**
	 * @brief Constructor
	 *
	 * @param analyzerHost Host in which the Analyzer is running.
	 * @param analyzerPort Port through which the connection will happen.
	 * @param ctrl Controller object.
	 */
	ShutDownSlave(string analyzerHost, int analyzerPort, Controller ctrl);

	/**
	 * @brief Destructor
	 */
	virtual ~ShutDownSlave();

	/**
	 * @brief Contains the main function to be run by the thread.
	 */
	void Run ();
protected:
	/**
	 * @brief Not implemented
	 */
	void InitThread ();

    /**
     * @brief Not implemented
     */
    void FlushThread ();
private:
	ServerSocket _SSock;
	string _analyzerHost;
	int _analyzerPort;
	Controller _ctrl;
};

#endif /* SHUTDOWNSLAVE_H_ */
