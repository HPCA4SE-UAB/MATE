
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
// Command Line of AC program
// (c) Anna Sikora, UAB, 2001-2010
// ----------------------------------------------

#include <string.h>
#include <iostream>
#include <stdlib.h>

//#include <limits.h> for the constant variable PATH_MAX

using namespace std;

/**
 * @class CommandLine
 * @brief Checks for the necessary data in the arguments passed to main and parses them.
 *
 * Notes
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class CommandLine
{	
public:
	/**
	 * @brief Constructor.
	 */
	CommandLine (int argc, char ** argv)
	  : _argc (argc), _argv (argv), _isOk (false), _configFile ("default")
	{
		Parse (_argc, _argv);
	}
	
	/**
	 * @brief Returns the value of _isOk.
	 * @return Boolean variable that is true if the configuration has been parsed correctly.
	 */
	bool IsOk () const { return _isOk; }

	/**
	 * @brief Getter for the _argc variable.
	 * @return Size of the vector of arguments.
	 */
	int GetArgc () const { return _argc; }

	/**
	 * @brief Getter for the _argv variable.
	 * @return Vector of arguments.
	 */
	char ** GetArgv () const { return _argv; }
	
	/**
	 * @brief Getter for the _appArgc variable.
	 * @return Size of the arguments vector for the app.
	 */
	int GetAppArgc () const { return _appArgc; }

	/**
	 * @brief Getter for the _appPath variable.
	 * @return Path to the executable of the app.
	 */
	char * GetAppPath () const { return _appPath; }

	/**
	 * @brief Getter for the _appArgv variable.
	 * @return Vector that contains the app's arguments.
	 */
	char ** GetAppArgv () const { return _appArgv; }

	/**
	 * @brief Checks if there's a path for the configuration file, if not returns 0.
	 * @return Path for the configuration file.
	 */
	bool HasConfig () const { return strcmp (_configFile, "default") != 0; }

	/**
	 * @brief Getter for the _configFile variable.
	 * @return Path for the configuration file.
	 */
	char * GetConfigFileName () const { return _configFile; }
	
	/**
	 * @brief Prints help message on the terminal.
	 * Tells the user how to introduce the necessary arguments.
	 */
	void DisplayHelp () const
	{
		cerr << "Usage: " << endl
			 << "	" << _argv [0] << " [-config file.ini] app_exec [app_args]" << endl
			 << "		runs AC" << endl
			 << "Type AC to print help message" << endl;
	}
	
private:
	/**
	 * @brief Parses the content of the arguments passed to the AC and initializes the fields with the
	 * data extracted.
	 *
	 * @param argc Size of the vector of arguments passed.
	 * @param argv Vector that contains the arguments passed.
	 */
	void Parse (int argc, char * argv [])
	{
		cerr << "argc: " << argc << endl;
		_isOk = false;	
		if (argc > 1)
		{
			int index = 1;			
			if (strcmp (argv [1], "-config") == 0) // AC [-config file.ini] app_exec [app_args]
			{
				_isOk = (argc >= 4);
				if (!_isOk)
					return;
				_configFile = argv [2];
				index = 3;
			}else{
				_configFile = "AC.ini";
			}
			_appPath = argv [index];
			_appArgc = argc - index - 1;
			_appArgv = &argv [index]; // argv[0] must point to app_exec
			_isOk = true;
		}	
	}


private:
	int 		_argc;
	char ** 	_argv;
   	bool		_isOk;
   	char *  	_configFile;
   	char *		_appPath;
  	int			_appArgc;
  	char ** 	_appArgv;
};

#endif
