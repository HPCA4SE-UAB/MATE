
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

#ifndef __DMLIBAPI_H__
#define __DMLIBAPI_H__

// Local includes
#include "Utils.h"

// C++ includes
#include <unistd.h>
#include <sys/types.h>
#include <string>

extern "C"
{
	namespace DMLib {
		/**
		 * @brief Initializes all the necessary objects to start working.
		 *
		 * This function starts the system log, registers the process and initializes
		 * the proxy to handle events.
		 *
		 * OMPI_COMM_WORLD_RANK environment variable should be set to the MPI rank of
		 * the process.
		 *
		 * @return True on success, false otherwise.
		 *
		 * @param taskName Name assigned to the current task.
		 * @param analyzerHost Name of the analyzer host.
		 * @param analyzerPort Port to communicate with the analyzer.
		 * @param configFile Path of the configuration file.
		 */
		/*bool DMLib_Init (std::string const & taskName,
				std::string const & analyzerHost,
				int analyzerPort, std::string const & configFile = "DMLib.ini");*/
		bool DMLib_Init (const char *taskName,
				const char *analyzerHost,
				int analyzerPort,  const char *configFile);


		void DMLib_ECreate(const char* func_name);

		void DMLib_PAPIAdd(char* EventName, const char* func_name);

		void DMLib_PAPI_Start(const char *func_name);

		void DMLib_PAPI_Stop(const char *func_name);

		void DMLib_PAPI_Destroy_EventSet(const char* func_name);


		/**
		 * @brief Sets size resolution, synchronize time difference.
		 *
		 *
		 */
		void DMLib_SetDiff (int lowDiff, int highDiff);

		/**
		 * @brief Stops to attend the monitoring.
		 *
		 * Unregisters the process from the proxy and destroys it.
		 */
		void DMLib_Done ();

		/**
		 * @brief Starts recording a new event.
		 *
		 * To record a new event it records an event identifier and the
		 * number of attributes.
		 *
		 * @param eventId Id of the event.
		 * @param instrPlace Place where the event is located
		 * {instrUnknown, ipFuncEntry,	ipFuncExit}.
		 * @param paramCount Number of parameters.
		 */
		void DMLib_OpenEvent (int eventId, InstrPlace instrPlace, int paramCount);

		/**
		 * @brief Adds integer parameter to the current event.
		 */
		void DMLib_AddIntParam (int value);

		/**
		 * @brief Adds float parameter to the current event.
		 */
		void DMLib_AddFloatParam (float value);

		/**
		 * @brief Adds double parameter to the current event.
		 */
		void DMLib_AddDoubleParam (double value);

		/**
		 * @brief Adds char parameter to the current event.
		 */
		void DMLib_AddCharParam (char value);

		/**
		 * @brief Adds string parameter to the current event.
		 */
		void DMLib_AddStringParam (std::string const & value);

		/**
		 * @brief Closes the event and sends it through the proxy.
		 */
		void DMLib_CloseEvent ();

		// global variables
		extern int TraceLib_SendSizeAux;
		extern int TraceLib_RcvSizeAux;
		extern int TraceLib_RcvTidAux;
	}
}

#endif
