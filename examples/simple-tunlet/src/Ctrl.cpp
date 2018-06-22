//----------------------------------------------------------------------
//
// Ctrl.cpp
// 
// Analyzer Ctrl - controls message receiving
//
// Alberto Olmo, UAB, 2018
//
//----------------------------------------------------------------------
#include "Ctrl.h"
#include "Config.h"
#include "DTAPI.h"
#include "Syslog.h"
#include "MyTunlet.h"
#include <unistd.h>

Controller::Controller(CommandLine & cmdLine, std::string const & cfgFile)
	: _cmdLine(cmdLine)
{	
	// Read configuration from file
	_cfg = ConfigHelper::ReadFromFile(cfgFile);

	// Configure log
	Syslog::Configure(_cfg);
}

void Controller::Run (ShutDownManager *sdm) 
{
	DTLibrary * lib = DTLibraryFactory::CreateLibrary (_cfg);
	MyTunlet mt;
	
    // Create the application model
    Model::Application & app = lib->CreateApplication(
		_cmdLine.GetAppPath(),
		_cmdLine.GetAppArgc(),
		_cmdLine.GetAppArgv());
        
	// Set the reference to the app in the sdm so it can stop the ACs
	sdm->setApp(app);
    
	// Initialize the tunlet
	mt.Initialize(app);

	// Start app
	mt.BeforeAppStart();
	app.Start();

		while (app.GetStatus() == stStarting && !sdm->isFinished()) {
        int nEvents = app.ProcessEvents (true);// Blocking
		if (nEvents > 0)
			Syslog::Debug("[Ctrl] Processed %d events", nEvents);
		sleep (1);
	}

	// Do nothing if the user stopped the application
	if (!sdm->isFinished()) {
		Syslog::Debug("[Ctrl] App is running...");
		Syslog::Debug("[Ctrl] Waiting for events");
	}
	
	while (app.GetStatus () == stRunning && !sdm->isFinished()) {
		int nEvents = app.ProcessEvents (true); // Blocking
		if (nEvents > 0)
			Syslog::Debug("[Ctrl] Processed %d events", nEvents);
		sleep (1);
	}

	
	Syslog::Debug ("[Ctrl] Application has finished");
	
	// Destroy the tunlet
	mt.Destroy ();
	
	// Cleanup
	DTLibraryFactory::DestroyLibrary(lib);
}
