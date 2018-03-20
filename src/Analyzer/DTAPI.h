
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
// DTAPI.h
// 
// Dynamic Tuning API Implementation
//
// Anna Sikora, UAB, 2004-2010
//
//----------------------------------------------------------------------

#if !defined DTAPI_H
#define DTAPI_H

#include "Config.h"
#include "EventCollector.h"
#include "AppModel.h"

class DTLibraryFactory;

/**
 * @brief Dynamic Tuning Library that offers DT API.
 * Encapsulates information about the application model and the event collector.
 * Provides methods to create application models.
 */
class DTLibrary {
	friend class DTLibraryFactory;
	public:
		/**
		 * @brief Creates a new application model, a new event collector and
		 * associates them.
		 * @param appPath 	path to the target application.
		 * @param argc		number of arguments of the target application.
		 * @param argv		list of arguments of the target application.
		 *
		 * @return Reference to the application model object.
		 */
		Model::Application & CreateApplication (
			char const * appPath,
			int argc,
			char const ** argv);
		/**
		 * @brief Application getter.
		 * @return Reference to the application model.
		 */
		Model::Application & GetApplication ();

	private:
		/**
		 * @brief Constructor.
		 * It is private in order to fit with a singleton pattern
		 * (see DTLibraryFactory for further information).
		 */
		DTLibrary (Config const & cfg);

		/**
		 * Destructor, it will only destroy the object if there is just one last reference
		 * to the library.
		 */
		~DTLibrary ();

		Config 		  const & _cfg;
		Model::Application  * _app;       //Maintains application model
		EventCollector 		* _collector; //Event collector (receives events
										  //from DMLibs)
	};

	/**
	 * @brief Handles the creation and destruction of DT Libraries.
	 */
	class DTLibraryFactory {
		public:
			/**
			 * @brief Creates and initializes the DT Library.
			 * Implements the singleton design pattern, so if the library is
			 * already created it returns a reference to it.
			 * @param cfg Reference to the configuration object.
			 *
			 * @return Reference to the library.
			 */
			static DTLibrary * CreateLibrary (Config const & cfg);

			/**
			 * @brief Destroys the library if there is only one last reference
			 * to the object.
			 * @param lib Reference to the library.
			 */
			static void DestroyLibrary (DTLibrary * lib);
};
#endif

