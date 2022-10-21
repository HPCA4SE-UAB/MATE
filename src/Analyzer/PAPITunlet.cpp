//----------------------------------------------------------------------
//
// FactoringTunlet.cpp
// 
// Anna Morajko, UAB, 2004
//
//----------------------------------------------------------------------

#include "PAPITunlet.h"
#include "FactoringStats_nw.h" 
#include "Syslog.h"
#include "Utils.h"
#include <assert.h>
#include <math.h>
#include "Config.h"
#include <sstream>
#include <boost/assign/list_of.hpp>
#include <time.h>
#define WINDOW_SIZE 3

//Be careful:
//vector of workers with its current iteration and batch idx
typedef struct curState {
	int iter;
	int batch;
	int numTuples;
};

curState workersCurState[128];



//Map with the names of each region to measure
//an entry point and exit point is necessary for
//each region,  the exit point is identified with
//the _e at the end of the name. enum FactoringEvent
//in PAPITunlet.h should also be modified.
std::map<std::string, FactoringEvent> FactoringEventMap = boost::assign::map_list_of("Copy",idCopy)("Copy_E",idCopy_End)\
	("Scale",idScale)("Scale_E",idScale_End)("Add",idAdd)("Add_E",idAdd_End)("Triad",idTriad)("Triad_E",idTriad_End)\
	("Reduction",idReduction)("Reduction_E",idReduction_End)("2PStencil",id2PStencil)("2PStencil_E",id2PStencil_End)\
	("2D4PStencil",id2D4PStencil)("2D4PStencil_E",id2D4PStencil_End)("Gather",idGather)("Gather_E",idGather_End)\
	("Scatter",idScatter)("Scatter_E",idScatter_End)("Stride2",idStride2)("Stride2_E",idStride2_End)("Stride4",idStride4)\
	("Stride4_E",idStride4_End)("Stride16",idStride16)("Stride16_E",idStride16_End)("Stride64",idStride64)("Stride64_E",idStride64_End)\
	("Rows",idRows)("Rows_E",idRows_End)("Test",idTest)("Test_E",idTest_End);


long_t timestamps[40] = {0};
long_t timestamp_total[40] = {0};
	
FactoringTunlet::FactoringTunlet()
: _app (0), _lastIterIdx (-1), _nw(0), _M0(1), _LAMBDA(0.0000001)
{
	_ventana.TAM = WINDOW_SIZE;
	_ventana.historico = new Stats[_ventana.TAM];
	for (int i = 0; i < _ventana.TAM; i++) {
			_ventana.historico[i].desv = 0.0;
			_ventana.historico[i].mean = 0.0;
	}
}

FactoringTunlet::~FactoringTunlet() {
	IterMap::iterator it = _iterMap.begin();
	while (it != _iterMap.end()) {
		IterData * data = it->second;
		delete data;
		it++;
	}
}

//----------------------------------------------------------------------------------------------------
// Test function to check if the event is correct
//----------------------------------------------------------------------------------------------------
void FactoringTunlet::TestEvent (Event e)
{
	//sleep(10);
	Attribute * testAttr;
	std::string * testMetrics;
	// Test insert event:
	Syslog::Info("Test event: Begin");
	Syslog::Info("----------------------------------------------");
	Syslog::Info("Name of the inserted function: %s",e.GetFunctionName().c_str());
	Syslog::Info("Entry point: %d",e.GetInstrPlace());
	Syslog::Info("Event ID: %d",e.GetId());
	Syslog::Info("Number of attributes in this event: %d",e.GetNumAttributes());
	testAttr = e.GetAttributes();
	for (int idx =0 ; idx<e.GetNumAttributes();idx++)
	{
		Syslog::Info("Source: %d",testAttr[idx].source);
		Syslog::Info("Type: %d",testAttr[idx].type);
		Syslog::Info("id: %s",(testAttr[idx].id).c_str());
	}
	Syslog::Info("Number of PAPI metrics in this event: %d",e.GetNumPapiMetrics());
	testMetrics = e.GetMetrics();
	for (int idx =0 ; idx<e.GetNumPapiMetrics();idx++)
	{
		Syslog::Info("id: %s",testMetrics[idx].c_str());
	}
	Syslog::Info("----------------------------------------------");
	Syslog::Info("Test event: End");
}

//----------------------------------------------------------------------------------------------------
// Create Events for the different regions
// The tunlet.ini file is read and for each region and entry/exit 
// point, the appropriate action is taken. In this case, the PAPI events
// are read and initialized in the application. The same events are used
// for all regions.
//----------------------------------------------------------------------------------------------------
void FactoringTunlet::CreateEvent(Task & t)
{
	Syslog::Info("[FT---]CreateEvent(Task & t)");
	// The functions into tunlet.ini file are read
	std::string nameFunc,base = "function",func,option;
	bool existsFunction =1, existsAtribute;
	int iter=1,iterA;
	stringstream ss,numA;
	std::vector<Attribute> Attrs;
	Attribute *aux;
	std::vector<std::string> Metrics;
	std::string *aux_string;
	InstrPlace entry;
	FactoringEvent idEvent;
	int NumberEvents = 2;
	// For each region
	while (existsFunction)
	{
		Syslog::Info("[FT---]CreateEvent---while");
		ss << iter;
		nameFunc = base + ss.str();
		existsFunction = _cfg.Contains ("Functions",nameFunc);
		if (existsFunction)
		{
			func = _cfg.GetStringValue("Functions",nameFunc);
			iterA =1;
			existsAtribute =1;
			//for each region attribute
			while (existsAtribute)
			{
				numA << iterA;
				existsAtribute = _cfg.Contains(func,"source"+numA.str());
				if (existsAtribute)
				{
					aux =new Attribute;
					option = _cfg.GetStringValue(func,"source"+numA.str());
					aux->source = (AttrSource) StringToEnum(option,1);
					option = _cfg.GetStringValue(func,"type"+numA.str());
					aux->type = (AttrValueType) StringToEnum(option,2);
					aux->id = _cfg.GetStringValue(func,"id"+numA.str());
					Attrs.push_back(*aux);
				}
				iterA ++;
				numA.str(std::string());
			}
			iterA =1;
			existsAtribute =1;
			//For each PAPI event
			while (existsAtribute)
			{
				
				numA << iterA;
				existsAtribute = _cfg.Contains(func,"papi_event"+numA.str());
				if (existsAtribute)
				{
					option = _cfg.GetStringValue(func,"papi_event"+numA.str());
					aux_string = new std::string(option);
					if( listPAPI.size()<iterA)
					{
						listPAPI.push_back(*aux_string);
					}
					else
					{
						if(listPAPI.at(iterA-1).compare(*aux_string)!=0)
						{
							Syslog::Debug("[FT---] ERROR PAPI LIST");
						}
					}
					Metrics.push_back(*aux_string);
				}
				iterA ++;
				numA.str(std::string());
			}
			// Add start event
			entry = (InstrPlace) StringToEnum(_cfg.GetStringValue(func,"entry"),3);//ipFuncEntry ipFuncExit
			Syslog::Debug("Event: %s", _cfg.GetStringValue(func,"event").c_str());
			idEvent = (FactoringEvent) FactoringEventMap[_cfg.GetStringValue(func,"event")];
			Event startEvent(idEvent, func,entry);
			startEvent.SetAttribute(Attrs.size(), &Attrs[0]);
			startEvent.SetMetric(Metrics.size(), &Metrics[0]);
			startEvent.SetEventHandler(*this);
			// Test insert event:
			TestEvent(startEvent);
			//---Ejecutar AddEvent --> Analyzer/AppTask
			t.AddEvent(startEvent);

			// Add end event
			idEvent = (FactoringEvent) FactoringEventMap[_cfg.GetStringValue(func,"event")+"_E"];
			Event endEvent(idEvent,func, ipFuncExit);
			endEvent.SetEventHandler(*this);
			endEvent.SetMetric(Metrics.size(), &Metrics[0]);
			TestEvent(endEvent);
			t.AddEvent(endEvent);
			Attrs.clear();
			Metrics.clear();
			Syslog::Info("[FT---]CreateEvent---DONE");
		}
		iter ++;
		ss.str(std::string());
	}

	//Create dummy event
	std::string dummyFunc = "MonitorSignal";
	Event dummyEvent(0,dummyFunc, ipFuncExit);
	dummyEvent.SetEventHandler(*this);
	TestEvent(dummyEvent);
	t.AddEvent(dummyEvent);
	Syslog::Debug("[FT---]MonitorSignal sent, Number of PAPI events to measure %d", listPAPI.size());
	//Create the header for the metrics file, one field for each metrics
	// plus time and region´s label
	for (std::vector<std::string>::iterator it = listPAPI.begin() ; it != listPAPI.end(); ++it)
		fs << *it << ";";
	fs << "time;label;" << std::endl;
}
//----------------------------------------------------------------------------------------------------
// end new
//----------------------------------------------------------------------------------------------------
void FactoringTunlet::Initialize(Model::Application & app) {
	_app = &app;
	_cfg = ConfigHelper::ReadFromFile(DefConfigFile);
	// prueba ------------------------------------------------------------
	//CreateEvent();
	// fin --------------------------------------------------------------------------------
	Syslog::Info ("[FT] Tunlet initialized");
	fs.open(data_file, std::ofstream::out | std::ofstream::app);
	Syslog::Info ("[FT] File open? %d", fs.is_open());
	assert(fs.is_open() != 0);
	
}

void FactoringTunlet::BeforeAppStart() {
	Syslog::Info("[FT Event---] BeforeAppStart");
	assert(_app != 0);
	_app->SetTaskHandler(*this);
}

void FactoringTunlet::Destroy() {
	_app = 0;
	Syslog::Info ("[FT] Tunlet destroyed");
	fs.close();
}

void FactoringTunlet::TaskStarted(Task & t) {
	Syslog::Debug("[FT Event---] TaskStarted");
	if (!t.IsMaster()) {
		//_optimizer.AddWorker (t.GetId ());
		_nw ++; //The attribute that controls the number of workers is incremented
		Syslog::Info("[FT] Incrementamos el n� de workers %d", _nw);
		//Syslog::Info ("[FT] Task %d added to optimizer", t.GetId ()); da error el t.GetId
		InsertWorkerEvents(t);
	} else
		InsertMasterEvents(t);
}

void FactoringTunlet::TaskTerminated (Task & t)
{
	Syslog::Debug("[FT Event---] TaskTerminated");
	if (!t.IsMaster()) {
		//_optimizer.RemoveWorker (t.GetId ());
		_nw--; //The attribute that controls the number of workers is decremented
		//Syslog::Info ("[FT] Task %d terminated", t.GetId ());
	}		
}

std::string FactoringTunlet::GetFuncName(int event_id)
{
	for (std::map<std::string,FactoringEvent>::iterator it=FactoringEventMap.begin(); it!=FactoringEventMap.end(); ++it)
	{
		if( it->second == event_id)
		{
			Syslog::Info("[FT Event---] %s => %d", it->first.c_str(), it->second);
			return it->first;
		}
	}
		
}

//handles all incoming events
void FactoringTunlet::HandleEvent (EventRecord const & r) {
	Syslog::Info("[FT Event---] HandleEvent START");
	switch (r.GetEventId()%2) {
		// For even events, the counters are obtained with a timestamp to measure
		// execution time of the monitored region
		case 0: // End of event/function
		{
			Syslog::Info("[FT Event---0] %s",GetFuncName(r.GetEventId()).c_str());
			int iter;
			int num_iters = listPAPI.size();
			//For each PAPI event
			for(iter=0; iter<num_iters; iter++)
			{
				// read the counter value and write it to the output file (data.csv)
				double PAPI_value_d = r.GetAttributeValue(iter).GetDoubleValue();
				long long int  PAPI_value_ll;
				memcpy(&PAPI_value_ll, &PAPI_value_d, sizeof(PAPI_value_d));
				Syslog::Info ("[FT Event] Papi Metric%d %lld", iter, PAPI_value_ll );
				fs << PAPI_value_ll << ";";
			}
			
			
			if (_lastIterIdx != -1) {
				IterData * data = FindIterData(_lastIterIdx);
				data->OnIterEnd (r.GetTimestamp());
			} else
				Syslog::Info ("[FT Event] end iter ignored");
			//Compare the starting timestamp to the one obtained at the end
			// to obtain execution time and write it to the output file
			long_t calc_time_iter= r.GetTimestamp()- timestamps[r.GetEventId()-1];
			fs << calc_time_iter;
			fs << ";" << GetFuncName(r.GetEventId()).c_str();
			fs << ";" << std::endl;
			if (calc_time_iter < 0)
				Syslog::Info("[FT Event---0] Negative calc_time_iter %ld", calc_time_iter);
			else
			{
				Syslog::Info("[FT Event---0] Calc_time_iter %ld", calc_time_iter);
				timestamp_total[r.GetEventId()] += calc_time_iter;
				Syslog::Info("[FT Event---0] Calc_time_iter %ld", timestamp_total[r.GetEventId()]);
			}
			Syslog::Info("[FT Event---0] end of %s",GetFuncName(r.GetEventId()).c_str());
   
			break;
		}
		// For odd events, the starting time is obtained
		case 1: // Start of event/function
		{
			timestamps[r.GetEventId()] = r.GetTimestamp();
			Syslog::Info("[FT Event---1] %s",GetFuncName(r.GetEventId()).c_str()); 
			_lastIterIdx = r.GetAttributeValue(0).GetIntValue();
			Syslog::Info ("[FT Event] start iter %d", _lastIterIdx);
			  
												 
														  
			//IterData * data = FindIterData(_lastIterIdx);
			Syslog::Info("[FT Event---1] end of %s",GetFuncName(r.GetEventId()).c_str());
			//r.GetTimestamp (); 
			break;
		}
		default:
			// ignore
			Syslog::Info ("[FT Event] ERROR: unknown event");
	}//End switch
	Syslog::Info("[FT Event---] HandleEvent END");
}


void FactoringTunlet::InsertMasterEvents(Task & t) {
	Syslog::Info ("[FT] Inserting events to master task %d", t.GetMpiRank ());

	CreateEvent(t);

}

void FactoringTunlet::InsertWorkerEvents(Task & t) {
	Syslog::Info ("[FT] Inserting events to worker task %d", t.GetMpiRank());

	CreateEvent(t);

}




IterData * FactoringTunlet::FindIterData(int iterIdx) {
	IterMap::iterator it = _iterMap.find(iterIdx);
	if (it != _iterMap.end()) {
		IterData * data = it->second;
		assert (data != 0);
		return data;
	} else {
		IterData * data = new IterData(iterIdx);
		_iterMap[iterIdx] = data;
		return data;
	}
}



float FactoringTunlet::CalculateFactorZero(int numworker) {
     
	int NW = numworker;
	float inv_factor = 1 + ((_ventana.historico[0].desv*sqrt(NW/2.0)) /
							_ventana.historico[0].mean);
	return 1.0/inv_factor;
}
 
float FactoringTunlet::CalculateFactorRest(int numworker) {
	int NW = numworker;
	float inv_factor = 2 + ((_ventana.historico[0].desv*sqrt(NW/2.0)) /
							_ventana.historico[0].mean);
	return 1.0/inv_factor;
}
 
double FactoringTunlet::CalculatePi(int nw, int V, int sendW, double alfa,
									double Tc) {
	double Tt;
	//Syslog::Debug ("[FT]CalculatePi NW=%d",nw);
	assert(nw > 0);
	Tt = 2 * _M0 + (((nw - 1) * alfa + 1) * _LAMBDA * V + Tc/nw);
	return (nw * Tt * Tt)/Tc;
}

void FactoringTunlet::Tuning_LB(int iterIdx, int numworkers)
{
	double Factor_zero, Factor_one;
	int i;
	//LOAD BALANCING SYNTONIZATION ===========================================
	//We distinguish between the first 3 iterations, when the historic
	//is being created and the following ones, in which the historic is
	//already created.
	if( WINDOW_SIZE >iterIdx) { //in our case, the first 3 iterations
		Factor_zero = CalculateFactorZero(numworkers);
		Factor_one = CalculateFactorRest(numworkers);
	} else{ //Following iterations
		//Move historic window
		for (i = 0; i < WINDOW_SIZE; i++){
			if (i != (WINDOW_SIZE - 1)){
				_ventana.historico[i].desv = _ventana.historico[i+1].desv;
				_ventana.historico[i].mean = _ventana.historico[i+1].mean;
				Syslog::Debug ("[FT::Tuning_lb] historico[%d] media =%f, desv =%f",
					  i,_ventana.historico[i].desv, _ventana.historico[i].mean );
			} else {
				_ventana.historico[i].desv = 0.0;
				_ventana.historico[i].mean = 0.0;
				Syslog::Debug ("[FT::Tuning_lb] historico[%d] media =%f, desv =%f", i,_ventana.historico[i].desv, _ventana.historico[i].mean );
			}
		}//End for

        Factor_zero = CalculateFactorZero(numworkers);
        Factor_one = CalculateFactorRest(numworkers);
    } //End if

	//The syntonization is applied after having calculated the factor value
	//For now I've supposed that the name of the variable to syntonize is TheFactorF
	//We should have 3 factor variables: F0,F1 (both syntonized after receiving
	//the start of the iteration event) and F2 (which is syntonized in this function)
    AttributeValue value_f0;
    value_f0.SetType(avtDouble);
	value_f0.doubleValue = Factor_zero;
	Syslog::Info("!!!!!! Executing tuning action, setting variable F0=%F", Factor_zero);
	_app->GetMasterTask()->SetVariableValue("F0", value_f0, 0);
	
    AttributeValue value_f1;
    value_f1.SetType(avtDouble);
	value_f1.doubleValue = Factor_one;
	Syslog::Info("!!!!! Executing tuning action, setting variable F1=%f", Factor_one);
	_app->GetMasterTask()->SetVariableValue("F1", value_f1, 0);
}

void FactoringTunlet::Tuning_NW(IterData * data) {
    //Go over the batch to calculate V, alfa y Tc
	int nBatchs = data->GetNumBatchs();
	int TotalDataVolume = 0; int TotalDataSendW = 0;
	double TotalCompTime = 0.0; double alfa;
    ModelParam MP;
    int NW;

    //Calculate parameters needed to evaluate the performance model
    NW = data -> GetNumWorkers();
   	//Syslog::Debug ("[FT] Tuning_NW NW=%d", NW);
	for(int i = 0; i < nBatchs; i++) {
		//Syslog::Debug ("[FT] Tuning_NW batch %d", i);
		BatchData & bd = data->GetBatchData(i);
		MP = bd.GetModelParam();
		TotalDataVolume += MP.TotalDataVolume;
		TotalDataSendW += MP.TotalDataSendW;
		TotalCompTime += MP.TotalCompTime;
    }
    alfa = (double)TotalDataSendW/(double)TotalDataVolume;
    
    //Syslog::Debug ("[FT::Tuning_NW] Iteracion: NW=%d",NW);
    //Syslog::Debug ("[FT::Tuning_NW] Iteracion: V=%d", TotalDataVolume);
    //Syslog::Debug ("[FT::Tuning_NW] Iteracion: SendW=%d",TotalDataSendW);
    //Syslog::Debug ("[FT::Tuning_NW] Iteracion: alfa=%f",alfa);
    //Syslog::Debug ("[FT::Tuning_NW] Iteracion: Time=%f",TotalCompTime);

    //Performance Index Value for actual workers' number 
    double currentPi = CalculatePi(NW, TotalDataVolume, TotalDataSendW, alfa,
    							 	 TotalCompTime);
  	Syslog::Debug ("[FT] Tuning_NW PiActual=%f --> NW=%d", currentPi,NW);
    
    //Star adding workers
    int NwOpt = NW + 1;
    double PiNwplus = CalculatePi(NwOpt, TotalDataVolume, TotalDataSendW, alfa,
    								TotalCompTime);
   	Syslog::Debug ("[FT] Tuning_NW PiNwPLUS=%f --> NW=%d", PiNwplus,NwOpt);
    if (currentPi > PiNwplus) {
        while (currentPi > PiNwplus) {
            currentPi = PiNwplus;
            NwOpt = NwOpt + 1;
            PiNwplus = CalculatePi(NwOpt, TotalDataVolume, TotalDataSendW,
            						alfa, TotalCompTime);
        }
        NwOpt--;
    } else { //Decrement workers
        NwOpt = NW-1;
        if (NwOpt > 1) {
            PiNwplus = CalculatePi(NwOpt, TotalDataVolume, TotalDataSendW,
            						alfa, TotalCompTime);
            while (currentPi > PiNwplus) {
                currentPi = PiNwplus;
                NwOpt = NwOpt-1;  
                PiNwplus = CalculatePi(NwOpt, TotalDataVolume, TotalDataSendW,
                						alfa, TotalCompTime);
            }
        }
        NwOpt++;
    }            
    
    if(NW != NwOpt) {
		AttributeValue value;
		value.SetType(avtInteger);
		value.intValue = NwOpt;
		Syslog::Info ("!!!!!! Executing tuning action, setting number of workers: "
						"%d. Past number: %d", NwOpt, NW);
		//_nw=NwOpt;
		//_app->GetMasterTask()->SetVariableValue("NW", value ,0); //tengo que situar en el codigo de Xfire una variable que sea NW
    }
    else {
    	Syslog::Info ("[FT] Skipping tuning action.  current n %d , Nopt %d", NW, NwOpt);
    }
}

void FactoringTunlet::TryTuning (int iterIdx) {
	Syslog::Info ("Trying tuning for starting iteration %d", iterIdx);
	
	double Factor_zero, Factor_one;
	int last_iter, numworkers;
	
	if (iterIdx != 0) {
    	last_iter = iterIdx - 1;
    	//GET NECESSARY DATA
    	//Get iteration data
    	IterData * data = FindIterData(last_iter);
    	numworkers = data->GetNumWorkers();
    	    	
    	//Check if the last iteration was completed
    	if (data->IsComplete()) {
             Syslog::Debug ("Iteration is complete %d", iterIdx);                                         
            //LOAD BALANCING SYNTONIZATION
            Tuning_LB(iterIdx, numworkers);                		
    		
    		//NUMBER OF WORKERS SYNTONIZATION
    		Tuning_NW(data);    		                                                                                                                     
        } else{
             Syslog::Debug ("Iteration is not complete %d", iterIdx);
        }
    } else {
        Syslog::Info ("Exit without executing tuning action because iteration "
        				"is %d", iterIdx);
    }	
}

void FactoringTunlet::TryTuning (int iterIdx, int batchIdx) {
	Syslog::Info ("Trying tuning for ending computation worker Iteration: %d", iterIdx);
	
	int index;
	int NW;
	double Factor;
	
	//GET NECESSARY DATA
	//Get iteration data
	IterData * data = FindIterData (iterIdx);
	NW = data->GetNumWorkers();
	//Get batch data
	BatchData & bd = data->GetBatchData (batchIdx);		
	
	//If the current batch is completed, we store historic data
	if(bd.IsComplete() && !bd.IsActualize()){
                            
		Syslog::Info ("Batch %d complete iteracion: %d", batchIdx, iterIdx);
		bd.SetActualize();

		//We distinguish between the first 3 iterations, when the historic
		//is being created and the following ones, in which the historic is
		//already created.
		WINDOW_SIZE-iterIdx > 1 ? index = iterIdx+1 : index = WINDOW_SIZE;

		for (int i = 0; i < index; i++) {
			//Syslog::Debug ("[FT::TryTuning] ANTES historico[%d] media =%f, desv=%f", i, _ventana.historico[i].mean, _ventana.historico[i].desv);
			_ventana.historico[i].mean+=bd.GetMeanStats();
			_ventana.historico[i].desv+=bd.GetStdStats();
			//Syslog::Debug ("[FT::TryTuning] DESPUES historico[%d] media =%f, desv=%f", i, _ventana.historico[i].mean, _ventana.historico[i].desv);
		}

		if (iterIdx != 0){
			/*
				if(data.PreviousBatchComplete(batchIdx)){
				//If the lasts batches are also complete, we can syntonize.
			 */
			Factor = CalculateFactorRest(NW); //Add 1 because it would be the j factor

			//The syntonization is applied after having calculated the factor value
			//For now I've supposed that the name of the variable to syntonize is TheFactorF
			//We should have 3 factor variables: F0,F1 (both syntonized after receiving
			//the start of the iteration event) and F2 (which is syntonized in this function)
			AttributeValue value;
			value.SetType(avtDouble);
			value.doubleValue = Factor;
			Syslog::Info ("!!!!!!  Executing tuning action, setting variable F2=%f",
							Factor );
			_app->GetMasterTask ()->SetVariableValue ("F2", value, 0);
			//} //End if(data.PreviousBatchComplete(batchIdx))
		}
	} else {
		Syslog::Info ("Exit without executing tuning action iteracion: %d", iterIdx);
    } //end if(bd.IsComplete() && !(bd.IsActualize()))
}
