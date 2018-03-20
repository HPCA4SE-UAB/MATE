
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
// AC - application controller
//
// main.cpp
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#include <iostream>
#include <string>
#include <stdio.h>
#include "cmdline.h"
#include "Ctrl.h"   
#include "di.h"
#include "Config.h"
#include "SysException.h"
#include <sys/types.h>
#include <unistd.h>

// Throws exceptions
int main (int argc, char* argv[])
{
	try
	{
		cerr << "****************************************" << endl
			 << "*****    APPLICATION CONTROLLER    *****" << endl
			 << "*****    (c) 2001-2010, UAB        *****" << endl
			 << "****************************************" << endl;
		//cerr << "void * = " << sizeof (void *) << endl
		//	<< "float = " << sizeof (float) << endl
		//	<< "double = " << sizeof (double) << endl
		//	<< "int = " << sizeof (int) << endl;*/

  		CommandLine cmdLine (argc, argv);

   		if (!cmdLine.IsOk ())
 		{
  			cmdLine.DisplayHelp ();
   			return -1;
  		}
   		Config cfg;
 		// run
		Controller ctrl (cmdLine);
		ctrl.Run ();
	  	cerr << endl << "Tuning job has finished." << endl;
	  	return 0;
	}
	catch (DiEx e)
	{
	  	cerr << "DynInst Exception: " << e.GetMessage ()
			    << ", Object name: " << e.GetObjectName ()
	            << endl;
	  	return -1;
	}
	catch (SysException & e)
	{
		e.Display ();
		return -1;
	}
	catch (char const* s)
	{
		cerr << "Exception: " << s << endl;
	  	return -1;
	}
	catch (...)
	{
		cerr << "Unexpected exception" << endl;
	  	return -1;
	}
}

