#if !defined MYTUNLET_H
#define MYTUNLET_H

//----------------------------------------------------------------------
//
// MyTunlet.h
// 
//
// Alberto Olmo, UAB, 2018
//
//----------------------------------------------------------------------

#include "AppModel.h" 
#include <map>
#include "Config.h"

using namespace Model;

// Analyzer process:
//	 create DTAPI, initialize collector, etc.
//	 create application model
//	 initialize all tunlets
//	 start application
//	 handle events....
//	 destroy tunlets
//	 destroy app model
//	 end

static char * DefConfigFile = "Tunlet.ini";

class Tunlet {
	public:

		/**
		 * @brief Initializes the tunlet
		 * @param app App associated to the tunlet
		 */
		virtual void Initialize (Model::Application & app) = 0;

		/**
		 * @brief Asserts that _app != 0 and sets the task handler of the app to the current one
		 */

		virtual void BeforeAppStart () {}
		/**
		 * @brief Sets _app = 0
		 */
		virtual void Destroy () = 0; 
};

enum EventsEnum
{ 
	idSetVariableValue,
	idReplaceFunction,
};

class MyTunlet : public Tunlet, public Model::EventHandler, public Model::TaskHandler {

	public:
		
		MyTunlet();
		
		~MyTunlet();

		void Initialize(Model::Application & app);

		void BeforeAppStart();

		void Destroy();

		void HandleEvent(Model::EventRecord const & r);

		void CreateEvent();

		void CreateEvent(Model::Task & t);

		void TaskStarted(Model::Task & t);

		void TaskTerminated(Model::Task & t);

	private:

		void TestEvent (Event e);

		
	private:
		Model::Application * _app;
		Config				 _cfg;
};

#endif // of MYTUNLET_H
