
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
// Ctrl.cpp
// 
// Analyzer Ctrl - controls message receiving
//
// Anna Sikora, UAB, 2002-2010
//
//----------------------------------------------------------------------

#include "Ctrl.h"
#include "Config.h"
#include "DTAPI.h"
#include "Syslog.h"
//#include "FactoringTunlet.h"
//#include "AdjustingNWTunlet.h"
#include "PAPITunlet.h"
#include <unistd.h>

Controller::Controller(CommandLine & cmdLine, std::string const & cfgFile)
	: _cmdLine(cmdLine)
{	
	_cfg = ConfigHelper::ReadFromFile(cfgFile);

	// configure log
	Syslog::Configure(_cfg);
}
	
void Controller::Run (ShutDownManager *sdm) {
	DTLibrary * lib = DTLibraryFactory::CreateLibrary (_cfg);
	//FactoringTunlet ft;
    //AdjustingNWTunlet anwt;
    FactoringTunlet anwt;

	//Now we create the application model
	Model::Application & app = lib->CreateApplication(
		_cmdLine.GetAppPath(),
		_cmdLine.GetAppArgc(),
		_cmdLine.GetAppArgv());

	//Set the reference to the app in the sdm, so it can stop the AC's
	sdm->setApp(app);

	//Initialize tunlet
    anwt.Initialize (app);
	Syslog::Info("[Ctrl] anwt.Initialize (app);...");

	//Start app
	anwt.BeforeAppStart ();
	Syslog::Info("[Ctrl] anwt.BeforeAppStart ();...");
	
	app.Start();
	Syslog::Debug ("[Ctrl] Waiting for app to start");
	while (app.GetStatus() == stStarting && !sdm->isFinished()) {
		int nEvents = app.ProcessEvents (true);//blocking
		if (nEvents > 0)
			Syslog::Debug("[Ctrl] Processed %d events", nEvents);
		sleep (1);
		Syslog::Info("[Ctrl] Final del bucle!");
	}
	//Do nothing if the user stopped the application
	if (!sdm->isFinished()) {
		Syslog::Info ("[Ctrl] App started");
		//ft.AppStarted ();
		anwt.AppStarted ();
	
		Syslog::Info("[Ctrl-] App is running...");
		Syslog::Debug("[Ctrl-] Waiting for events");
	}
	while (app.GetStatus () == stRunning && !sdm->isFinished()) {
		Syslog::Debug("[Ctrl-] while...");
		int nEvents = app.ProcessEvents (true);//true --> blocking
			if (nEvents > 0)
				Syslog::Debug("[Ctrl-] Processed %d events", nEvents);
		Syslog::Debug("[Ctrl-] sleeping 1 second");
		sleep (1);
	}
	Syslog::Info ("[Ctrl] Application has finished");
	
	//Destroy tunlet
	//ft.Destroy ();
	anwt.Destroy ();
	
	// cleanup
	DTLibraryFactory::DestroyLibrary (lib);
}
