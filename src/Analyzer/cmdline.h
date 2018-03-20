
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

#if !defined (CMDLINE_H)
#define CMDLINE_H
// ----------------------------------------------
// cmdline.h
// Command Line of Analyzer
// (c) Anna Sikora, UAB, 2001-2010
// ----------------------------------------------

#include <string.h>
#include <iostream>
#include <stdlib.h>

//#include <limits.h> for the constant variable PATH_MAX

using namespace std;

/**
 * @brief Encapsulates methods to interact with the user of analyzer.
 * Basically reads the arguments of the analyzer and parses them to get
 * the configuration file and the objective application with its parameters.
 * Once read, encapsulates the information and provides accessors to it.
 * There are two formats Analyzer can be called:
 * <ul>
 * <li> Analyzer <AppPath> [<AppArgs>] </li>
 * <li> Analyzer -config file.ini <App> [<AppArgs>] </li>
 * </ul>
 */
class CommandLine
{	
	public:
		/**
		 * @brief Constructor, parses the arguments provided to analyzer.
		 * @param argc	number of arguments for analyzer
		 * @param argv	arguments for analyzer
		 */
		CommandLine(int argc, char ** argv)
		  : _argc(argc), _argv(argv), _isOk(false), _configFile(0)
		{
			Parse (_argc, _argv);
		}

		/**
		 * @brief Status of the arguments getter.
		 * @return if the arguments provided to analyzer are correct or not.
		 */
		bool IsOk() const { return _isOk; }

		/**
		 * @brief Number of arguments getter.
		 * @return The number of arguments provided to analyzer.
		 */
		int GetArgc() const { return _argc; }

		/**
		 * @brief Arguments getter.
		 * @return The arguments provided to analyzer.
		 */
		char ** GetArgv() const { return _argv; }

		/**
		 * @brief Determines if the user has chosen his own configuration file.
		 * @return If the user provided a specific configuration file.
		 */
		bool HasConfig() const { return _configFile != 0; }

		/**
		 * @brief Configuration file getter.
		 * @return The configuration file of Analyzer.
		 */
		char * GetConfigFile() const { return _configFile; }

		/**
		 * @brief Application path getter.
		 * @return The path of the target application.
		 */
		char const * GetAppPath() const { return _appPath; }

		/**
		 * @brief Application number of arguments getter.
		 * @return The number of arguments of the target application.
		 */
		int GetAppArgc() const { return _appArgc; }
	
		/**
		 * @brief Application arguments getter.
		 * @return The arguments of the target application.
		 */
		char const ** GetAppArgv() const { return (char const* *)_appArgv; }

		/**
		 * @brief Explains the user which arguments can be provided to analyzer.
		 */
		void DisplayHelp() const {
			cerr << "Usage: " << endl
				 << "\t" << _argv [0] << " <App> [<AppArgs>]" << endl
				 << "\t   runs analyzer for a given application" << endl
				 << "\t" << _argv [0] << " [-config file.ini] <App> [<AppArgs>]" << endl
				 << "\t   runs analyzer for the application App with a given configuration file" << endl;
		}

	private:

		/**
		 * @brief Parses the arguments of analyzer to get the configuration file
		 * and the information of the objective application.
		 * Distinguishes if the user provides or not a configuration file and controls
		 * the correctness in the number of arguments.
		 */
		void Parse (int argc, char ** argv) {
			if (argc < 2) { //Analyzer
				_isOk = false;
			} else { // if (argc > 1)
				if (strcmp (argv [1], "-config") == 0) { // analyzer -config file.ini App
					_isOk = (argc >= 4);
					if (_isOk) {
						_configFile = argv [2];
						_appPath = argv [3];
						_appArgc = argc - 3;
						_appArgv = &argv [3];
					}
				} else { // analyzer <AppPath> [<AppArgs>]
					_isOk = true;
					_appPath = argv [1];
					_appArgc = argc - 1;
					_appArgv = &argv [1];
				}
			}
		}

	private:
		int 		_argc;
		char ** 	_argv;
		bool		_isOk;
		char *  	_configFile;
		// app params
		char *		_appPath;
		int			_appArgc;
		char ** 	_appArgv;
};

#endif
