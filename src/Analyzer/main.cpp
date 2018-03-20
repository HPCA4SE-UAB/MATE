
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
// Analyzer	
//
// main.cpp
//
// Anna Sikora, UAB, 2000-2010
//
//----------------------------------------------------------------------

#include <iostream>
#include <string>
#include <stdio.h>
#include "Ctrl.h"
#include "SysException.h"
#include "cmdline.h"
#include "Types.h"
#include "ShutDownManager.h"

static char * DefConfigFile = "Analyzer.ini";

/*
void LONG2INTS2 (long_t l, int_t & high, int_t & low)
{
	high = static_cast<int_t>((l & 0xFFFFFFFF00000000LL) >> 32);
	low = static_cast<int_t> (l & 0x00000000FFFFFFFFLL);
}
void INTS2LONG2 (int_t high, int_t low, long_t & l)
{
	
	//long_t h64 = (long_t)high;
	//long_t l64 = (long_t)low;
	
	//l = (h64 << 32) | l64;
	
	unsigned int H = static_cast <unsigned int> (high);
	unsigned int L = static_cast <unsigned int> (low);

	unsigned long long h64 = static_cast <unsigned long long> (H);
	h64 <<= 32; 

	printf ("high is: %ld, H is %lu, H64 is: %llu\n", high, H, h64);

	unsigned long long l64 = static_cast <unsigned long long> (L);
	printf ("low is: %ld, L is: %lu, L64 is: %llu\n", low, L, l64);

	l = static_cast <long_t> ((h64 | l64));	
}	

void test ()
{
	long_t T1 = 1080993892553;
	int_t hi, low;
	LONG2INTS (T1, hi, low);
	
	long_t T2;
	INTS2LONG (hi, low, T2);

	long_t T3;
	INTS2LONG2 (hi, low, T3);
	
	printf ("T1 is: %lld, high int: %ld, low int: %ld", T1, hi, low);
	printf ("T2 is: %lld, high int: %ld, low int: %ld", T2, hi, low);
	printf ("T3 is: %lld, high int: %ld, low int: %ld", T3, hi, low);
}
*/
	
int main(int argc, char* argv[]) {
	ShutDownManager *sdm = new ShutDownManager();

	try
	{
		cerr << "********************************************" << endl
		     << "***     ANALYZER, (c) UAB, 2002-2004     ***" << endl
		     << "********************************************" << endl;
		CommandLine cmdLine (argc, argv);
   		if (!cmdLine.IsOk ()) {
  			cmdLine.DisplayHelp ();
    		return -1; 
  		}
  		
  		std::string cfgFile;
        if (cmdLine.HasConfig ()) {
            cfgFile = cmdLine.GetConfigFile ();
        } else {
            cfgFile = DefConfigFile;
        }
        //Run
        cerr << "Loading configuration from " << cfgFile << endl;
        Controller ctrl (cmdLine, cfgFile);
        ctrl.Run (sdm);

	  	cerr << endl << "Analyzer has finished." << endl;
	  	return 0;
	} catch (SysException & se) {
		se.Display ();
		return -1;
	} catch (Exception & e) {
		e.Display ();
		return -1;
	} catch (char const* s) {
		cerr << "Exception: " << s << endl;
		return -1;
	}
	catch (...) {
		cerr << "Unexpected exception" << endl;
		return -1;
	}
}

