
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

#if !defined FACTORINGTUNLET_H
#define FACTORINGTUNLET_H

//----------------------------------------------------------------------
//
// FactoringTunlet.h
// 
//
// Anna Sikora, UAB, 2004
//
//----------------------------------------------------------------------

#include "AppModel.h" 
#include "FactoringStats_nw.h" 
//#include "FactoringOptimizer.h" 
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
//	 fin

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
		 * @brief Sets tl=0.3 and lambda=0.5
		 */
		virtual void AppStarted () {}

		/**
		 * @brief Sets _app = 0
		 */
		virtual void Destroy () = 0;
};

enum FactoringEvent
{ 
	idIterStart = 1, 
	idIterEnd = 2,
	idTupleStart = 3,
	idTupleEnd = 4,
	idCalcStart = 5,
	idCalcEnd = 6,
	idNewBatch = 7
};

/**
 * @struct Stats
 * @brief Struct that stores the mean and standard deviation values
 */
struct Stats
{
	/**
	 * @brief Standard deviation attribute
	 */
    double desv;

	/**
	 * @brief Mean attribute
	 */
    double mean;       
};

/**
 * @brief Window that will store statistics of the workers
 */
struct Ventana
{
    /**
     * @brief Size of the window
     */
    int TAM;

    /**
     * @brief Pointer to a set of stats like the mean, std deviation etc.
     */
    Stats * historico;       
};

/**
 * @brief Factoring optimization tunlet for m/w apps.
 */
class FactoringTunlet : public Tunlet, public Model::EventHandler,
						public Model::TaskHandler {
	// maps iteration index to iteration data
	typedef std::map<int, IterData *> IterMap;
	
	public:
		/**
		 * @brief Constructor
		 */
		FactoringTunlet();

		/**
		 * @brief Destructor
		 */
		~FactoringTunlet();

		/**
		 * @param app
		 */
		void Initialize(Model::Application & app);

		/**
		 * @brief Asserts that _app != 0 and sets the task handler of the app to the current one
		 */
		void BeforeAppStart();

		/**
		 * @brief Sets _app = 0
		 */
		void Destroy();

		/**
		 * @brief Handles all incoming events
		 * @param r EventRecord where its ID can be found
		 */
		void HandleEvent(Model::EventRecord const & r);

		/**
		 * @brief Increments the number of workers by 1 and inserts an event to the worker.
		 * @param t Task
		 */
		void TaskStarted(Model::Task & t);

		/**
		 * @brief Decrements the number of workers by 1
		 * @param t Task
		 */
		void TaskTerminated(Model::Task & t);

		/**
		 * @brief Creates events by using the configuration specified in the file tunlet.ini
		 */
		void CreateEvent();

		/**
		 * @brief Creates events by using the configuration specified in the file tunlet.ini and a given task
		 * @param t Task
		 */
		void CreateEvent(Model::Task & t);

	private:

		void TestEvent (Event e);

		/**
		 * @param t
		 */
		void InsertWorkerEvents(Model::Task & t);

		/**
		 * @param t
		 */
		void InsertMasterEvents(Model::Task & t);

		/**
		 * @param iterIdx
		 */
		IterData * FindIterData(int iterIdx);

		/**
		 * @param numworker
		 */
		float CalculateFactorZero(int numworker);

		/**
		 * @param numworker
		 */
		float CalculateFactorRest(int numworker);
		//int GetNW(){ return _nw;};

		/**
		 * @param nw
		 * @param V
		 * @param sendW
		 * @param alfa
		 * @param Tc
		 */
		double CalculatePi(int nw, int V, int sendW, double alfa, double Tc);

		/**
		 * @param data
		 */
		void Tuning_NW(IterData * data);

		/**
		 * @param iterIdx
		 * @param numworkers
		 */
		void Tuning_LB(int iterIdx, int numworkers);

		/**
		 * @param iterIdx
		 */
		void TryTuning(int iterIdx);

		/**
		 *
		 * @param iterIdx
		 * @param batchIdx
		 */
		void TryTuning(int iterIdx, int batchIdx);

	private:
		Model::Application * _app;
		int                  _nw; //Number of workers
		double               _M0; //Latency of the network
		double               _LAMBDA; //Costs of sending 1 byte
		IterMap			 	 _iterMap;
		int					 _lastIterIdx; // indicates current iteration
										   //index on master
		Ventana              _ventana;
		//FactoringOptimizer	 _optimizer; //????
		//float				 _toleranceF; //?????
		Config				 _cfg;
};
#endif // of FACTORINGTUNLET_H
