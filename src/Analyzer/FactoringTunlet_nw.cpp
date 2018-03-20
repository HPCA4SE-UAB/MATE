
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
// FactoringTunlet.cpp
// 
// Anna Sikora, UAB, 2004
//
//----------------------------------------------------------------------

#include "FactoringTunlet_nw.h"
//#include "FactoringStats_nw.h"
#include "Syslog.h"
#include "Utils.h"
#include <assert.h>
#include <math.h>
#include "Config.h"
#include <sstream>
#include <boost/assign/list_of.hpp>

#define WINDOW_SIZE 3

//Be careful:
//vector of workers with its current iteration and batch idx
/**
 * @class curState
 * @brief Struct that stores the iteration, batch and number of tuples
 */
typedef struct curState {
	int iter;
	int batch;
	int numTuples;
};

curState workersCurState[128];

std::map<std::string, FactoringEvent> FactoringEventMap = boost::assign::map_list_of("IterStart", idIterStart)("IterEnd", idIterEnd)\
	("TupleStart",idTupleStart)("TupleEnd",idTupleEnd)("CalcStart",idCalcStart)("CalcEnd",idCalcEnd)("NewBatch",idNewBatch);
	
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
// new
//----------------------------------------------------------------------------------------------------
void FactoringTunlet::TestEvent (Event e)
{
	Attribute * testAttr;
	// Test insert event:
	Syslog::Debug("Test event: Begin");
	Syslog::Debug("----------------------------------------------");
	Syslog::Debug("Name of the inserted function: %s",e.GetFunctionName().c_str());
	Syslog::Debug("Entry point: %d",e.GetInstrPlace());
	Syslog::Debug("Event ID: %d",e.GetId());
	Syslog::Debug("Number of attributes in this event: %d",e.GetNumAttributes());
	testAttr = e.GetAttributes();
	for (int idx =0 ; idx<e.GetNumAttributes();idx++)
	{
		Syslog::Debug("Source: %d",testAttr[idx].source);
		Syslog::Debug("Type: %d",testAttr[idx].type);
		Syslog::Debug("id: %s",(testAttr[idx].id).c_str());
	}
	Syslog::Debug("----------------------------------------------");
	Syslog::Debug("Test event: End");
}

void FactoringTunlet::CreateEvent()
{
	Syslog::Debug("[FT---]CreateEvent()");
	// The functions into tunlet.ini file is read
	std::string nameFunc,base = "function",func,option;
	bool exitsFunction =1, exitsAtribute;
	int iter=1,iterA;
	stringstream ss,numA;
	std::vector<Attribute> Attrs;
	Attribute *aux;
	InstrPlace entry;
	FactoringEvent idEvent;

	while (exitsFunction)
	{
		Syslog::Debug("[FT---]CreateEvent---while");
		ss << iter;
		nameFunc = base + ss.str();
		exitsFunction = _cfg.Contains ("Functions",nameFunc);
		if (exitsFunction)
		{
			func = _cfg.GetStringValue("Functions",nameFunc);
			iterA =1;
			exitsAtribute =1;
			while (exitsAtribute)
			{
				numA << iterA;
				exitsAtribute = _cfg.Contains(func,"source"+numA.str());
				if (exitsAtribute)
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
			// Add start event
			entry = (InstrPlace) StringToEnum(_cfg.GetStringValue(func,"entry"),3);
			idEvent = (FactoringEvent) FactoringEventMap[_cfg.GetStringValue(func,"event")];
			Event startEvent(idEvent, func,entry);
			startEvent.SetAttribute(Attrs.size(), &Attrs[0]);
			startEvent.SetEventHandler(*this);
			// Test insert event:
			TestEvent(startEvent);

			//---Execute AddEvent --> Analyzer/AppTask
			//t.AddEvent(startEvent);
			// Add end event
			//Event endEvent(idCalcEnd,func, ipFuncExit);
			//endEvent.SetEventHandler(*this);
			//t.AddEvent(endEvent);
			Attrs.clear();
			Syslog::Debug("[FT---]CreateEvent---DONE");
		}
		iter ++;
		ss.str(std::string());
	}
}

void FactoringTunlet::CreateEvent(Task & t)
{
	Syslog::Debug("[FT---]CreateEvent(Task & t)");
	// The functions into tunlet.ini file is read
	std::string nameFunc,base = "function",func,option;
	bool exitsFunction =1, exitsAtribute;
	int iter=1,iterA;
	stringstream ss,numA;
	std::vector<Attribute> Attrs;
	Attribute *aux;
	InstrPlace entry;
	FactoringEvent idEvent;

	while (exitsFunction)
	{
		Syslog::Debug("[FT---]CreateEvent---while");
		ss << iter;
		nameFunc = base + ss.str();
		exitsFunction = _cfg.Contains ("Functions",nameFunc);
		if (exitsFunction)
		{
			func = _cfg.GetStringValue("Functions",nameFunc);
			iterA =1;
			exitsAtribute =1;
			while (exitsAtribute)
			{
				numA << iterA;
				exitsAtribute = _cfg.Contains(func,"source"+numA.str());
				if (exitsAtribute)
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
			// Add start event
			entry = (InstrPlace) StringToEnum(_cfg.GetStringValue(func,"entry"),3);
			idEvent = (FactoringEvent) FactoringEventMap[_cfg.GetStringValue(func,"event")];
			Event startEvent(idEvent, func,entry);
			startEvent.SetAttribute(Attrs.size(), &Attrs[0]);
			startEvent.SetEventHandler(*this);
			// Test insert event:
			TestEvent(startEvent);
			
			//---Ejecutar AddEvent --> Analyzer/AppTask
			t.AddEvent(startEvent);

			// Add end event
			//Event endEvent(idCalcEnd,func, ipFuncExit);
			switch (idEvent) {
				case idIterStart:
				{
					Event endEvent(idIterEnd,func, ipFuncExit);
					endEvent.SetEventHandler(*this);
					t.AddEvent(endEvent);
					break;
				}
				case idTupleStart:
				{
					Event endEvent(idTupleEnd,func, ipFuncExit);
					endEvent.SetEventHandler(*this);
					t.AddEvent(endEvent);
					break;
				}
				case idCalcStart:
				{
					Event endEvent(idCalcEnd,func, ipFuncExit);
					endEvent.SetEventHandler(*this);
					t.AddEvent(endEvent);
					break;
				}
				default:
					// ignore
					Event endEvent(idCalcEnd,func, ipFuncExit);
					endEvent.SetEventHandler(*this);
					t.AddEvent(endEvent);
			}//End switch




			//endEvent.SetEventHandler(*this);
			//t.AddEvent(endEvent);
			Attrs.clear();
			Syslog::Debug("[FT---]CreateEvent---DONE");
		}
		iter ++;
		ss.str(std::string());
	}
    std::string dummyFunc = "MonitorSignal";
    Event dummyEvent(0,dummyFunc, ipFuncExit);
    dummyEvent.SetEventHandler(*this);
    TestEvent(dummyEvent);
    t.AddEvent(dummyEvent);
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
}

void FactoringTunlet::BeforeAppStart() {
	Syslog::Debug("[FT Event---] BeforeAppStart");
	assert(_app != 0);
	_app->SetTaskHandler(*this);
}

void FactoringTunlet::Destroy() {
	_app = 0;
	Syslog::Info ("[FT] Tunlet destroyed");
}

void FactoringTunlet::TaskStarted(Task & t) {
	Syslog::Debug("[FT Event---] TaskStarted");
	if (!t.IsMaster()) {
		//_optimizer.AddWorker (t.GetId ());
		_nw ++; //The attribute that controls the number of workers is incremented
		Syslog::Info("[FT] Increment the number of workers %d", _nw);
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

//handles all incoming events
void FactoringTunlet::HandleEvent (EventRecord const & r) {
	Syslog::Debug("[FT Event---] HandleEvent START");
	switch (r.GetEventId()) {
		case idIterStart:
		{
			Syslog::Debug("[FT Event---] idIterStart");
			//Sender: master
			//attrs: iterIndex, total work to do, single tuple size in bytes,
			//		 number of workers
			_lastIterIdx = r.GetAttributeValue(0).GetIntValue();
			//Syslog::Info ("[FT Event] start iter %d", _lastIterIdx);
			IterData * data = FindIterData(_lastIterIdx);
			data->OnIterStart (r.GetTimestamp (), 
				r.GetAttributeValue(1).GetIntValue(), // total work
				r.GetAttributeValue(2).GetIntValue(), // size bytes
				r.GetAttributeValue(3).GetIntValue()  // number of workers
				);
			_nw = r.GetAttributeValue(3).GetIntValue();
			Syslog::Debug("[FT Event] start iter %d NW=%d", _lastIterIdx, _nw);
			//Syntonize at the beginning of the iteration:
			//factor & number of workers
			TryTuning(_lastIterIdx);
			break;
		}
		case idIterEnd:
		{
			Syslog::Debug("[FT Event---] idIterEnd");
		//Sender: master
		//attrs: none
			Syslog::Info ("[FT Event] end iter");
			if (_lastIterIdx != -1) {
				IterData * data = FindIterData(_lastIterIdx);
				data->OnIterEnd (r.GetTimestamp());
				//Syntonize at the end of the iteration:
				//factor & number of workers
				TryTuning (_lastIterIdx);
				
			} else
				Syslog::Debug ("[FT Event] end iter ignored");
			break;
		}
		case idTupleStart:
		{
			Syslog::Info ("[FT Event---] idTupleStart");
		//Sender=master
		//attrs: batch index, worker tid, ntask to do
			if (_lastIterIdx != -1) {
				IterData * data = FindIterData(_lastIterIdx);
				int batchIdx = r.GetAttributeValue(0).GetIntValue();
				int workerTid = r.GetAttributeValue(1).GetIntValue();
				int numTuples = r.GetAttributeValue(2).GetIntValue();
				workersCurState[workerTid].iter = _lastIterIdx;
				workersCurState[workerTid].batch = batchIdx;
				workersCurState[workerTid].numTuples = numTuples;

				int sizeInBytes = numTuples * data->GetTupleSizeInBytes();
				Syslog::Info("[FT Event] tuple START iterIdx %d, workerTid %d, "
								"batch %d, numTuples %d, sizeInBytes %d",
								_lastIterIdx, workerTid, batchIdx, numTuples,
								sizeInBytes);
				BatchData & bd = data->GetBatchData(batchIdx);
                //WorkerData & wd = bd.GetWorkerData(workerTid-1);//Data structure starts in 0
                WorkerData & wd = bd.NewWorkerData(workerTid-1);
				wd.OnTupleStart(numTuples, sizeInBytes);
			}
			else 
				Syslog::Debug ("[FT Event] start tuple ignored");
			break;
		}
		case idTupleEnd:
		{
			Syslog::Info ("[FT Event---] idTupleEnd");
		//Sender: master
		//attrs: worker tid, ntask received
			if (_lastIterIdx != -1) {
				IterData * data = FindIterData(_lastIterIdx);
				//int workerTid = r.GetAttributeValue (0).GetIntValue ();
				int workerTid = r.GetTask().GetMpiRank();
				//int numTuples = r.GetAttributeValue (1).GetIntValue ();
				//int batchIdx = r.GetAttributeValue (2).GetIntValue ();
				int batchIdx = workersCurState[workerTid].batch;
				int numTuples = workersCurState[workerTid].numTuples;
				int sizeInBytes = numTuples * data->GetTupleSizeInBytes();
					Syslog::Info ("[FT Event] tuple END iterIdx %d, workerTid %d, batch %d, numTuples %d, sizeInBytes %d",
						_lastIterIdx, workerTid, batchIdx, numTuples, sizeInBytes); 
				BatchData & bd = data->GetBatchData(batchIdx);
				bd.SizeTaskReceived(sizeInBytes);   
                TryTuning(_lastIterIdx, batchIdx);
			}
			else 
				Syslog::Debug("[FT Event] end tuple ignored");
			break;
		}
		case idCalcStart:
		{
			Syslog::Info ("[FT Event---] idCalcStart");
		//Sender: worker
		//attrs: iter index, batch index
			int workerTid = r.GetTask().GetMpiRank();
			int iterIdx = r.GetAttributeValue(0).GetIntValue();
			int batchIdx = r.GetAttributeValue(1).GetIntValue();
			workersCurState[workerTid].iter = iterIdx;
			workersCurState[workerTid].batch = batchIdx;
			Syslog::Debug("[FT Event] calc start iteridx %d, worker %d, batch %d", iterIdx, workerTid, batchIdx);
			IterData * data = FindIterData(iterIdx);
			BatchData & bd = data->GetBatchData(batchIdx);
            WorkerData & wd = bd.GetWorkerData(workerTid-1);  //Data structure starts in 0
			wd.OnCalcStart (r.GetTimestamp());
			break;
		}
		case idCalcEnd:
		{
			Syslog::Info ("[FT Event---] idCalcEnd");
		//Sender: worker
		//attrs: iter index, batch index
			int workerTid = r.GetTask().GetMpiRank();
			//int iterIdx = r.GetAttributeValue (0).GetIntValue ();
			//int batchIdx = r.GetAttributeValue (1).GetIntValue();
			int iterIdx = workersCurState[workerTid].iter;
			int batchIdx = workersCurState[workerTid].batch;
			
			Syslog::Debug("[FT Event] calc end iteridx %d, worker %d, batch %d", iterIdx, workerTid, batchIdx);
			IterData * data = FindIterData(iterIdx);
			BatchData & bd = data->GetBatchData(batchIdx);
            WorkerData & wd = bd.GetWorkerData(workerTid-1); //Data structure starts in 0
			wd.OnCalcEnd (r.GetTimestamp());
			
			/*
				//Syntonize after receiving an end of computation event, because a batch may be already completed
				TryTuning (iterIdx, batchIdx);
			*/
			break;
		}
     	case idNewBatch:
     	{
     		Syslog::Info ("[FT Event---] idNewBatch");
     	// NEW BATCH EVENT
     	//idNewBatch: sender: master
     	//attrs: idBatch, N� chunks
            int batchIdx = r.GetAttributeValue(0).GetIntValue();
			int nchunks = r.GetAttributeValue(1).GetIntValue();
            Syslog::Debug ("[FT Event] New Batch Event batch %d, nchunks %d",
            				batchIdx, nchunks);
			IterData * data = FindIterData(_lastIterIdx);
			BatchData & bd = data->GetBatchData(batchIdx);
			bd.OnNewBatch(nchunks);
			data->OnNewBatch();
			break;
     	}
		default:
			// ignore
			Syslog::Info ("[FT Event] ERROR: unknown event");
	}//End switch
	Syslog::Debug("[FT Event---] HandleEvent END");
}



/*void FactoringTunlet::InsertMasterEvents(Task & t) {
	Syslog::Info ("[FT] Inserting events to master task %d", t.GetMpiRank ());

	CreateEvent();
	//We need to insert: iter start, iter end, new batch
	
    // START ITER EVENT
	// idIterStart:
	//sender: master
	//attrs: iterIndex, total work to do, single tuple size in bytes, workers' number
	// Attribute iterAttrs[4];
	// iterAttrs[0].source = asFuncParamValue;
	// iterAttrs[0].type = avtInteger;
	// iterAttrs[0].id = "0";	// first param: iter index
	// iterAttrs[1].source = asVarValue;
	// iterAttrs[1].type = avtInteger;
	// iterAttrs[1].id = "TheTotalWork";
	// iterAttrs[2].source = asVarValue;
	// iterAttrs[2].type = avtInteger;
	// iterAttrs[2].id = "TheWorkSizeUnitBytes";
	// iterAttrs[3].source = asVarValue;
	// iterAttrs[3].type = avtInteger;
	// iterAttrs[3].id = "NW";
	
	// Event startIterEvent(idIterStart, "global_sendreceive", ipFuncEntry);
	// startIterEvent.SetAttribute(4, iterAttrs);
	// startIterEvent.SetEventHandler(*this);
	// t.AddEvent(startIterEvent);
    	
	// // END ITER EVENT
	// // idIterEnd: sender=master, attrs: none
	// Event endIterEvent(idIterEnd, "global_sendreceive", ipFuncExit);
	// endIterEvent.SetEventHandler(*this);
	// t.AddEvent(endIterEvent);
	
	// // NEW BATCH EVENT
	// // idNewBatch: sender=master, attrs: idBatch, N� chunks
	// Attribute NewBatchAttrs[2];
	// NewBatchAttrs[0].source = asFuncParamValue;
	// NewBatchAttrs[0].type = avtInteger;
	// NewBatchAttrs[0].id = "0"; //idBatch
	// NewBatchAttrs[1].source = asFuncParamValue;
	// NewBatchAttrs[1].type = avtInteger;
	// NewBatchAttrs[1].id = "1"; //idtuple
	// Event newBatchEvent(idNewBatch, "Factoring_SetNumTuples", ipFuncEntry);
	// newBatchEvent.SetAttribute(2, NewBatchAttrs);
	// newBatchEvent.SetEventHandler(*this);
	// t.AddEvent(newBatchEvent);
	
	// // START TUPLE EVENT
	// //sender: master
	// //attrs: batch index, worker tid, ntask to do
	// Attribute tupleAttr[3];
	// tupleAttr[0].source = asFuncParamValue;
	// tupleAttr[0].type = avtInteger;
	// tupleAttr[0].id = "0"; // first function param	: batch index
	// tupleAttr[1].source = asFuncParamValue;
	// tupleAttr[1].type = avtInteger;
	// tupleAttr[1].id = "2"; // third function param	: worker tid
	// tupleAttr[2].source = asFuncParamValue;
	// tupleAttr[2].type = avtInteger;
	// tupleAttr[2].id = "4"; // fifth function param : num tuples
	// Event startTupleEvent(idTupleStart, "global_sendwork", ipFuncEntry);
	// startTupleEvent.SetAttribute(3, tupleAttr);
	// startTupleEvent.SetEventHandler(*this);
	// t.AddEvent(startTupleEvent);
	
	// // END TUPLE EVENT
	// // sender=master, attrs: worker tid, nTask received, batch index
	// /*Attribute endTupleAttr [3];
	// endTupleAttr[0].source = asFuncParamPointerValue;
	// endTupleAttr[0].type = avtInteger;
	// endTupleAttr[0].id = "0"; // first function param : pointer to worker Tid
	// endTupleAttr[1].source = asFuncParamPointerValue;
	// endTupleAttr[1].type = avtInteger;
	// endTupleAttr[1].id = "3"; // first function param : pointer to nTask received
	// endTupleAttr[2].source = asFuncParamPointerValue;
	// endTupleAttr[2].type = avtInteger;
	// endTupleAttr[2].id = "4"; // first function param : pointer to batch's index of tasks received
	// Event endTupleEvent(idTupleEnd, "global_receivework", ipFuncExit);
	// //endTupleEvent.SetAttribute (3, endTupleAttr);
	// endTupleEvent.SetEventHandler(*this);
	// t.AddEvent(endTupleEvent);
		
	// note: must be requested at the end to guarantee that
	// all events that arrive AFTER StartIter are valid and in correct order.
	// all events BEFORE first StartIter event should be ignored.
}*/

void FactoringTunlet::InsertMasterEvents(Task & t) {
	Syslog::Info ("[FT] Inserting events to master task %d", t.GetMpiRank ());
	//CreateEvent();
	CreateEvent(t);
	//We need to insert: iter start, iter end, new batch
	
    // START ITER EVENT
	// idIterStart:
	//sender: master
	//attrs: iterIndex, total work to do, single tuple size in bytes, workers' number
	// Attribute iterAttrs[4];
	// iterAttrs[0].source = asFuncParamValue;
	// iterAttrs[0].type = avtInteger;
	// iterAttrs[0].id = "0";	// first param: iter index
	// iterAttrs[1].source = asVarValue;
	// iterAttrs[1].type = avtInteger;
	// iterAttrs[1].id = "TheTotalWork";
	// iterAttrs[2].source = asVarValue;
	// iterAttrs[2].type = avtInteger;
	// iterAttrs[2].id = "TheWorkSizeUnitBytes";
	// iterAttrs[3].source = asVarValue;
	// iterAttrs[3].type = avtInteger;
	// iterAttrs[3].id = "NW";
	
	// Event startIterEvent(idIterStart, "global_sendreceive", ipFuncEntry);
	// startIterEvent.SetAttribute(4, iterAttrs);
	// startIterEvent.SetEventHandler(*this);
	// t.AddEvent(startIterEvent);
    	
	// // END ITER EVENT
	// // idIterEnd: sender=master, attrs: none
	// Event endIterEvent(idIterEnd, "global_sendreceive", ipFuncExit);
	// endIterEvent.SetEventHandler(*this);
	// t.AddEvent(endIterEvent);
	
	// // NEW BATCH EVENT
	// // idNewBatch: sender=master, attrs: idBatch, N� chunks
	// Attribute NewBatchAttrs[2];
	// NewBatchAttrs[0].source = asFuncParamValue;
	// NewBatchAttrs[0].type = avtInteger;
	// NewBatchAttrs[0].id = "0"; //idBatch
	// NewBatchAttrs[1].source = asFuncParamValue;
	// NewBatchAttrs[1].type = avtInteger;
	// NewBatchAttrs[1].id = "1"; //idtuple
	// Event newBatchEvent(idNewBatch, "Factoring_SetNumTuples", ipFuncEntry);
	// newBatchEvent.SetAttribute(2, NewBatchAttrs);
	// newBatchEvent.SetEventHandler(*this);
	// t.AddEvent(newBatchEvent);
	
	// // START TUPLE EVENT
	// //sender: master
	// //attrs: batch index, worker tid, ntask to do
	// Attribute tupleAttr[3];
	// tupleAttr[0].source = asFuncParamValue;
	// tupleAttr[0].type = avtInteger;
	// tupleAttr[0].id = "0"; // first function param	: batch index
	// tupleAttr[1].source = asFuncParamValue;
	// tupleAttr[1].type = avtInteger;
	// tupleAttr[1].id = "2"; // third function param	: worker tid
	// tupleAttr[2].source = asFuncParamValue;
	// tupleAttr[2].type = avtInteger;
	// tupleAttr[2].id = "4"; // fifth function param : num tuples
	// Event startTupleEvent(idTupleStart, "global_sendwork", ipFuncEntry);
	// startTupleEvent.SetAttribute(3, tupleAttr);
	// startTupleEvent.SetEventHandler(*this);
	// t.AddEvent(startTupleEvent);
	
	// // END TUPLE EVENT
	// // sender=master, attrs: worker tid, nTask received, batch index
	// /*Attribute endTupleAttr [3];
	// endTupleAttr[0].source = asFuncParamPointerValue;
	// endTupleAttr[0].type = avtInteger;
	// endTupleAttr[0].id = "0"; // first function param : pointer to worker Tid
	// endTupleAttr[1].source = asFuncParamPointerValue;
	// endTupleAttr[1].type = avtInteger;
	// endTupleAttr[1].id = "3"; // first function param : pointer to nTask received
	// endTupleAttr[2].source = asFuncParamPointerValue;
	// endTupleAttr[2].type = avtInteger;
	// endTupleAttr[2].id = "4"; // first function param : pointer to batch's index of tasks received	*/
	// Event endTupleEvent(idTupleEnd, "global_receivework", ipFuncExit);
	// //endTupleEvent.SetAttribute (3, endTupleAttr);
	// endTupleEvent.SetEventHandler(*this);
	// t.AddEvent(endTupleEvent);
		
	// note: must be requested at the end to guarantee that
	// all events that arrive AFTER StartIter are valid and in correct order.
	// all events BEFORE first StartIter event should be ignored.
}

void FactoringTunlet::InsertWorkerEvents(Task & t) {
	Syslog::Info ("[FT] Inserting events to worker task %d", t.GetMpiRank());
	//CreateEvent();
	CreateEvent(t);

	//We need to measure WORKER CALC TIME so we monitor function:
	//	 arcStepKernel(&pfront,&pv,&ptsInf,dtime, iterIdx, batchIdx);
	
	// CALC START & CALC END: sender=worker, attrs: iter index, Batch index
	// Attribute attr[2];
	// attr[0].source = asFuncParamValue;
	// attr[0].type = avtInteger;
	// attr[0].id = "0"; // Iter index
	// attr[1].source = asFuncParamValue;
	// attr[1].type = avtInteger;
	// attr[1].id = "1"; // Batch index
	
	// // The functions into tunlet.ini file is read
	// std::string nameFunc,base = "function";
	// bool exits =1;
	// int iter =1;
	// stringstream ss;
	// while (exits)
	// {
	// 	ss << iter;
	// 	nameFunc = base + ss.str();
	// 	Syslog::Debug("leyendo funcion %s",nameFunc.c_str());
	// 	exits = _cfg.Contains ("Functions",nameFunc);
	// 	if (exits)
	// 	{
	// 		// Add start event
	// 		Event startEvent(idCalcStart, _cfg.GetStringValue("Functions",nameFunc), ipFuncEntry);
	// 		startEvent.SetAttribute(2, attr);
	// 		startEvent.SetEventHandler(*this);
	// 		t.AddEvent(startEvent);
	// 		// Add end event
	// 		Event endEvent(idCalcEnd,_cfg.GetStringValue("Functions",nameFunc), ipFuncExit);
	// 		endEvent.SetEventHandler(*this);
	// 		t.AddEvent(endEvent);
	// 	}
	// 	iter ++;
	// 	ss.str(std::string());
	// }

/*	Event startEvent(idCalcStart, "arcStepKernel", ipFuncEntry);
	startEvent.SetAttribute(2, attr);
	startEvent.SetEventHandler(*this);
	t.AddEvent(startEvent);
		
	Event endEvent(idCalcEnd, "arcStepKernel", ipFuncExit);
	//endEvent.SetAttribute (2, attr);
	endEvent.SetEventHandler(*this);
	t.AddEvent(endEvent);*/
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
	Syslog::Debug ("[FT::CalculateFactorZero] NW %f ",  NW);
	Syslog::Debug ("[FT::CalculateFactorZero] inv_factor %f ",  inv_factor);
	Syslog::Debug ("[FT::CalculateFactorZero]  %f ",  1.0/inv_factor);
	return 1.0/inv_factor;
}
 
float FactoringTunlet::CalculateFactorRest(int numworker) {
	int NW = numworker;
	float inv_factor = 2 + ((_ventana.historico[0].desv*sqrt(NW/2.0)) /
							_ventana.historico[0].mean);
	Syslog::Debug ("[FT::CalculateFactorZero] NW %f ",  NW);
	Syslog::Debug ("[FT::CalculateFactorRest] inv_factor %f",  inv_factor);
	Syslog::Debug ("[FT::CalculateFactorRest]  %f ",  1.0/inv_factor);
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

	/*if(Factor_zero == Factor_zero)
	{
		AttributeValue value_f0;
	    value_f0.SetType(avtDouble);
		value_f0.doubleValue = Factor_zero;
		Syslog::Info("!!!!!! Executing tuning action, setting variable F0=%F", Factor_zero);
		_app->GetMasterTask()->SetVariableValue("F0", value_f0, 0);
	}
	

	if(Factor_one == Factor_one)
	{
		AttributeValue value_f1;
	    value_f1.SetType(avtDouble);
		value_f1.doubleValue = Factor_one;
		Syslog::Info("!!!!! Executing tuning action, setting variable F1=%f", Factor_one);
		_app->GetMasterTask()->SetVariableValue("F1", value_f1, 0);
	}*/

    Factor_zero = 0.5;
    AttributeValue value_f0;
    value_f0.SetType(avtDouble);
	value_f0.doubleValue = Factor_zero;
	Syslog::Info("!!!!!! Executing tuning action, setting variable F0=%F", Factor_zero);
	_app->GetMasterTask()->SetVariableValue("F0", value_f0, 0);

	Factor_one = 0.5;
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
