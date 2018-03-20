
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


#include "FactoringStats_nw.h"

WorkerData::WorkerData()
 :  _nNumTaskReceived(0),
	_StartCalcTimeMs(0),
	_EndCalcTimeMs(0),
	_ComputingTimeMs(0),
	_taken(0),
	_initialized(0)
{
}

void WorkerData::OnCalcStart(long_t time) {
	_StartCalcTimeMs = time;
    //Syslog::Debug ("[WorkerData::OnCalcStart] Start Time %f", _StartCalcTimeMs); 
}	

void WorkerData::OnCalcEnd(long_t time) {
	//Ignore end event until start is received
	if (_StartCalcTimeMs == 0)
		return; //Ignore
	assert(time >= _StartCalcTimeMs);
	_EndCalcTimeMs = time;
	_ComputingTimeMs = (time - _StartCalcTimeMs);
    _taken = 1;	
    //Syslog::Debug ("[WorkerData::OnCalcEnd] Computing time  %f-%f=%f", _EndCalcTimeMs, _StartCalcTimeMs,_ComputingTimeMs ); 	
}

void WorkerData::OnTupleStart(int nTuples, int sizeBytes) {
	_nNumTaskReceived = nTuples;
	_sizeTaskBytes = sizeBytes;
	_initialized = 1;
}

BatchData::BatchData (int batchIdx)
: _batchIdx(batchIdx),
  _numChunks(0),
  _TotalTaskReceived(0),
  _flagActualize(0)
{
}

BatchData::~BatchData() {
	MMap::iterator it = _mapW.begin ();
	while (it != _mapW.end()) {
		WorkerData * wd = it->second;
		delete wd;
		it++;
	}
}

WorkerData & BatchData::NewWorkerData(int workerTid) {
	MMap::iterator it = _mapW.find (workerTid);
	
	if (it != _mapW.end()) {
		pair<MMap::iterator,MMap::iterator> ret = _mapW.equal_range(workerTid);
		//Syslog::Debug ("[BatchData::GetWorkerData] double iterator definition!!");
        for (it = ret.first; it != ret.second; ++it) {
        	//Syslog::Debug ("[BatchData::GetWorkerData] Inside the for!!");
        	WorkerData * wd = (*it).second;
        	assert(wd !=0);
        	if (!wd ->IsInitialized()) {
        		//Syslog::Debug ("[BatchData::NewWorkerData] return worker exist!!");
        		return *wd;
        	}
        }
        
       // Syslog::Debug ("[BatchData::newWorkerData] DOES NOT EXIST, CREATE!!");
        WorkerData * wd = new WorkerData();
		_mapW.insert(pair<int, WorkerData *>(workerTid, wd)); //_mapW.insert ( pair<int,* WorkerData>(workerTid, wd) ); no entiendo el error
		return *wd;
    } else {
		WorkerData * wd = new WorkerData ();
		_mapW.insert(pair<int, WorkerData *>(workerTid, wd)); //_mapW.insert ( pair<int,* WorkerData>(workerTid, wd) ); no entiendo el error
		return *wd;
	}
}


WorkerData & BatchData::GetWorkerData (int workerTid) {
    MMap::iterator it = _mapW.find (workerTid);
	
	if (it != _mapW.end()) {
		pair<MMap::iterator,MMap::iterator> ret = _mapW.equal_range(workerTid);
		//Syslog::Debug ("[BatchData::GetWorkerData] double iterator!!");
	
        for (it = ret.first; it != ret.second; ++it) {
        	//Syslog::Debug ("[BatchData::GetWorkerData] Inside for!!");
        	WorkerData * wd = (*it).second;
        	assert(wd != 0);
        	if (!wd ->IsTaken()) {
        		//Syslog::Debug ("[BatchData::GetWorkerData] IS NOT TAKEN!!");
        		return *wd;
        	}
        }
        
        //Syslog::Debug ("[BatchData::GetWorkerData] DOES NOT EXIST, CREATE!!");
        WorkerData * wd = new WorkerData();
		_mapW.insert(pair<int, WorkerData *>(workerTid, wd));
		//_mapW.insert( pair<int,* WorkerData>(workerTid, wd) ); no entiendo el error
		return *wd;
    } else {
		WorkerData * wd = new WorkerData();
		_mapW.insert(pair<int, WorkerData *>(workerTid, wd));
		//_mapW.insert ( pair<int,* WorkerData>(workerTid, wd) ); no entiendo el error
		return *wd;
	}
    /*
	MMap::iterator it;
    pair<MMap::iterator,MMap::iterator> ret = _mapW.equal_range(workerTid);
	Syslog::Debug ("[BatchData::GetWorkerData] definicion iterador doble!!"); 
	//if(it != _mapW.end()){
        for (it=ret.first; it!=ret.second; ++it){
       	  Syslog::Debug ("[BatchData::GetWorkerData] dentro del for!!");   
          WorkerData * wd = (*it).second;
          assert(wd !=0);
          if(!wd ->IsTaken())
             return *wd;
          else
           Syslog::Debug ("[BatchData::GetWorkerData] PROBLEM !!");        
        }
    }
    else
	{
		WorkerData * wd = new WorkerData ();
		_mapW.insert(pair<int, WorkerData *>(workerTid, wd)); //_mapW.insert ( pair<int,* WorkerData>(workerTid, wd) ); no entiendo el error
		return *wd;
	}
	*/
	
}

/*WorkerData & BatchData::GetWorkerData (int workerTid)
{	
	MMap::iterator it = _mapW.find (workerTid);
	
	if (it != _mapW.end ())
	{
		WorkerData * wd = it->second;
		assert (wd != 0);
		return *wd;
	}
	else
	{
		WorkerData * wd = new WorkerData ();
		_mapW.insert(pair<int, WorkerData *>(workerTid, wd)); //_mapW.insert ( pair<int,* WorkerData>(workerTid, wd) ); no entiendo el error
		return *wd;
	}
}*/

bool BatchData::AreWorkersComplete() const {
	MMap::const_iterator it = _mapW.begin();
	while (it != _mapW.end()) {
		WorkerData * wd = it->second;
		if (!wd->IsComplete ())
			return false;
		it++;
	}
	return true;
}


WorkerData ** BatchData::AllocWorkersArray() {
	int numWorkers = _mapW.size();
	WorkerData ** arr = new WorkerData *[numWorkers];
	
	int i = 0;
	MMap::iterator it = _mapW.begin();
	while (it != _mapW.end()) {
		WorkerData * wd = it->second;
		arr[i] = wd;
		it++;
		i++;
	}
	return arr;
}

double BatchData:: GetMeanStats() {
   	double mean = 0.0;
   	int numChunks = _mapW.size();
    MMap::iterator it = _mapW.begin ();
	while (it != _mapW.end()) {
		WorkerData * wd = it->second;
		//Syslog::Debug ("[STAT] Mean each worker total_time/NTask
        // --> %f/%d", wd->GetTotalCalcTime(),wd->GetNumProcessedTuples());
		mean += wd->GetTotalCalcTime()/wd->GetNumProcessedTuples();
		//Syslog::Debug ("[STAT] Mean acumulada %f", mean);
		it++;		
	}	
	_meanTaskProcessTime = mean/numChunks;
	Syslog::Debug ("[STAT] Mean Task Process time %f numchunks %d",
					_meanTaskProcessTime, numChunks);
	return _meanTaskProcessTime;	       
}

double BatchData:: GetStdStats()
{
    double variance = 0.0;
   	int numChunks = _mapW.size();   	
    MMap::iterator it = _mapW.begin ();
	while (it != _mapW.end ())
	{
		WorkerData * wd = it->second;
		//Syslog::Debug ("[STAT] Varianza each worker (total_time/N�Task)^2 -->(%f/%d)^2", wd->GetTotalCalcTime(),wd->GetNumProcessedTuples());
		variance += ((wd->GetTotalCalcTime()/wd->GetNumProcessedTuples())- _meanTaskProcessTime )* ((wd->GetTotalCalcTime()/wd->GetNumProcessedTuples())-_meanTaskProcessTime);
		//Syslog::Debug ("[STAT] Varianza acumulada = %f", varianza );
		it++;		
	}
    
    variance = variance/numChunks;
    //Syslog::Debug ("[STAT] Varianza total varianza/numchunk= varianza/%d = %f",numChunks,varianza);
	_stdTaskProcessTime = sqrt(variance);
    Syslog::Debug ("[STAT] Standar deviation %f",_stdTaskProcessTime);
	
	return _stdTaskProcessTime;  
       
}

ModelParam BatchData:: GetModelParam() {
    ModelParam MP;
    MMap::iterator it = _mapW.begin ();
    int tasksend = 0;
    double sumtci = 0.0;    
	while (it != _mapW.end()) {
		WorkerData * wd = it->second;
		tasksend += wd->GetSizeProcessedTuples();
		sumtci += wd->GetTotalCalcTime();
		it++;		
	}	

	MP.TotalDataVolume = tasksend + tasksend; //do we call it with 'this.'???
	MP.TotalDataSendW = tasksend;
	MP.TotalCompTime = sumtci;
        
	//Syslog::Debug ("[BatchData::GetModelParam] TotalDataVolume=%d, TotakDataSendW=%d, TotalCompTime=%f",MP.TotalDataVolume, MP.TotalDataSendW,  MP.TotalCompTime);
	return MP; 				              
}

IterData::IterData (int iterIdx)
: _iterIdx (iterIdx),
  _iterStartTimeMs(0),
  _iterTimeMs(0),
  _numTasks (0),
  _sizeBytes (0),
  _nbatchs(0),
  _flagIterStart(0),
  _flagIterEnd(0),
  _numWorkers (0)
{
}

IterData::~IterData ()
{
	Map::iterator it = _mapB.begin ();
	while (it != _mapB.end ()) {
		BatchData * bd = it->second;
		delete bd;
		it++;
	}
}

void IterData::OnIterStart (long_t time, int numTasks, int sizeBytes, int nw) {
    _flagIterStart = 1;
	_iterStartTimeMs = time;
	_numTasks = numTasks;
	_sizeBytes = sizeBytes;
	_numWorkers = nw;
	Syslog::Debug ("[IterData::OnIterStart] numTuples %d, sizeBytes %d, NW %d", 
				_numTasks, _sizeBytes, _numWorkers);
}

void IterData::OnIterEnd (long_t time) {
    _flagIterEnd = 1;     
	assert (time >= _iterStartTimeMs);
	_iterTimeMs = time - _iterStartTimeMs;	
}

BatchData & IterData::GetBatchData(int IdxBatch) {
	Map::iterator it = _mapB.find(IdxBatch);
	if (it != _mapB.end()) {
		BatchData * bd = it->second;
		assert(bd != 0);
		return *bd;
	} else {
		BatchData * bd = new BatchData (IdxBatch);
		_mapB [IdxBatch] = bd;
		return *bd;
	}
}

bool IterData::AreBatchsComplete() const {
	Map::const_iterator it = _mapB.begin ();
	while (it != _mapB.end()) {
		BatchData * bd = it->second;
		if (!bd->IsComplete())
			return false;
		it++;
	}
	return true;
}

/*bool IterData::PreviousBatchComplete(int IdxBatch) {
     Map::cons_iterator it = _mapB.begin();
     while ((it != _mapB.end()) && (it->first < IdxBatch)) {
           BatchData *bd = it->second;
           if (!bd->IsComplete())
              return false;
           it++;
     }
     return true;
}*/

BatchData ** IterData::AllocBatchsArray() {
	int numBatchs = _mapB.size();
	BatchData ** arr = new BatchData *[numBatchs];
	
	int i = 0;
	Map::iterator it = _mapB.begin();
	while (it != _mapB.end()) {
		BatchData * bd = it->second;
		arr[i] = bd;
		it++;
		i++;
	}
	return arr;
}
