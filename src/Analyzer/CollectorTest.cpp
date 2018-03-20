
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
// Collector.cpp
// Event Collector Test
// Anna Sikora, UAB, 2003
//
//----------------------------------------------------------------------

#include <iostream>
#include <string>
#include <stdio.h>
#include "SysException.h"
#include "Syslog.h"
#include "config.h"
#include "Reactor.h"
#include "ECPAcceptor.h"


int main (int argc, char* argv[])
{
	try
	{
		cerr << "**************************************************" << endl
		     << "***  COLLECTOR TEST, (c) Ania, UAB, 2002-2003  ***" << endl
		     << "**************************************************" << endl;
		// initialize dynlib log file
		Config cfg;
		cfg.AddEntry ("Syslog", "MasterSwitch", "true");
		cfg.AddEntry ("Syslog", "LogLevel", "0");
		//cfg.AddEntry ("Syslog", "StdErr", "true");
		cfg.AddEntry ("Syslog", "StdErr", "false");
		cfg.AddEntry ("Syslog", "LogFile", "collector.log");
		cfg.AddEntry ("Syslog", "AppendFile", "false");
		Syslog::Configure (cfg);
		Syslog::Info ("[Collector] Starting...");
		
		Reactor reactor;
		ECPAcceptor acceptor (reactor);
		while (true)
		{
			TimeValue waitTime (5); 
			Syslog::Info ("[Collector] Waiting for events...");
			reactor.HandleEvents (&waitTime);
		}			
	  	Syslog::Info ("[Collector] Done...");
	  	return 0;
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

