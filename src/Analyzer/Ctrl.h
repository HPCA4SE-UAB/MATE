
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
// Ctrl.h
// 
// Analyzer Ctrl - controls message receiving
//
// Anna Sikora, UAB, 2002-2010
//
//----------------------------------------------------------------------

#ifndef CTRL_H
#define CTRL_H

#include "Config.h"
#include "cmdline.h"
#include <stdio.h>
#include <string>
#include "ShutDownManager.h"

using namespace Common;

/**
 * @brief Provides the logic and controls the execution flow of the application.
 */
class Controller {
	public:
		/**
		 * @brief Constructor, sets the command line for the user, determines
		 * the configuration for the application and prepares the system log.
		 */
		Controller (CommandLine & cmdLine, std::string const & cfgFile);

		/**
		 * @brief Manages the execution flow of the application.
		 * The execution flow of analyzer is:
		 * <ul>
		 * <li>create DTAPI, initialize collector, etc.</li>
		 * <li>create application model</li>
		 * <li> initialize all tunlets</li>
		 * <li> start application</li>
		 * <li> handle events</li>
		 * <li> destroy tunlets</li>
		 * <li> destroy app model</li>
		 * </ul>
		 */
		void Run (ShutDownManager *sdm);

	private:
		CommandLine & _cmdLine;
		Config 		  _cfg;
};

#endif

