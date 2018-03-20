
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
// DTAPI.cpp
// 
// Dynamic Tuning API Implementation
//
// Anna Sikora, UAB, 2004-2010
//
//----------------------------------------------------------------------

#include "DTAPI.h"
#include <assert.h>

static DTLibrary * TheLibrary = 0;
static int DTCount = 0;//Reference counter

DTLibrary * DTLibraryFactory::CreateLibrary(Config const & cfg) {
	if (TheLibrary == 0) {
		TheLibrary = new DTLibrary (cfg);		
	}
	++DTCount;
	return TheLibrary;
}

void DTLibraryFactory::DestroyLibrary(DTLibrary * lib) {
	assert(lib == TheLibrary);
	assert(DTCount > 0);
	--DTCount;
	if (DTCount == 0) {
		delete TheLibrary;
		TheLibrary = 0;
	}
}

// Dynamic Tuning Library that offers DT API ==================================
DTLibrary::DTLibrary(Config const & cfg)
   : _app (0),
   	 _cfg (cfg),
     _collector (0)
{
}

DTLibrary::~DTLibrary() {
	delete _collector;
	delete _app;
}
	
Model::Application & DTLibrary::CreateApplication(char const * appPath,
										           int argc, char const ** argv) {
	assert(_app == 0);
	//Start collector
	_collector = new EventCollector(_cfg.GetIntValue ("EventCollector", "Port",
									EventCollector::DefaultPort));
	Syslog::Info("[DTLibrary::CreateApplication] on port %d", _cfg.GetIntValue ("EventCollector", "Port", EventCollector::DefaultPort));
	//Create application model
	_app = new Model::Application (appPath, argc, argv);
	_collector->SetListener (_app);
	return *_app;
}

Model::Application & DTLibrary::GetApplication() {
	assert(_app!=0);
	return *_app;
}
