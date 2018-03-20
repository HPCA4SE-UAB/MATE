
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

#ifndef FACTORINGSTATS_H
#define FACTORINGSTATS_H
using namespace std;

// FACTORING STATISTICS

#include <cassert>
#include <stdio.h>
#include <sys/time.h>
#include "Utils.h"
#include <map>
#include <set>
#include "math.h"

/**
 * @struct ModelParam
 * @brief Stores the total volume of data, the total
 * amount of data sent by workers and the total computed time
 */
struct ModelParam
{
	int TotalDataVolume;

	int TotalDataSendW;

	double TotalCompTime;

};

/**
 * @brief Worker task statistics for a single batch
 */
class WorkerData {
	public:

		/**
		 * @brief Constructor
		 */
		WorkerData ();

		/**
		 * @brief Sets the calculation start time
		 * @param time Time to start in ms
		 */
		void OnCalcStart (long_t time);

		/**
		 * @brief Asserts that the final time is greater than the starting one, computes
		 * the elapsed time in milliseconds and sets the flag _taken to 1
		 * @param time Final time of computation
		 */
		void OnCalcEnd (long_t time);

		/**
		 * @brief Checks if the WorkerData is complete
		 * @return Returns true if the start and end calc times are greater than 0
		 * and if the worker was initialized
		 */
		bool IsComplete () const {
						Syslog::Debug ("[WorkerData::IsComplete] Stime=%f Etime=%f INIT=%d", _StartCalcTimeMs, _EndCalcTimeMs,  _initialized);
						return _StartCalcTimeMs > 0 && _EndCalcTimeMs > 0 && _initialized; }

		// ? o mas bien tupleEnds=calcEnds?
		/**
		 * @brief Getter of the total number of tasks received
		 * @return _nNumTaskReceived
		 */
		int GetNumProcessedTuples () const { return _nNumTaskReceived; }

		/**
		 * @brief Getter of the task size in bytes
		 * @return _sizeTaskBytes
		 */
		int GetSizeProcessedTuples() const { return _sizeTaskBytes;}

		/**
		 * @brief Getter of the total computing time in ms
		 * @return _ComputingTimeMs
		 */
		double GetTotalCalcTime () const { return _ComputingTimeMs; }

		/**
		 * @brief Initializes the tuple by setting the number of tasks received to
		 * nTuples and the size of the tasks to sizeBytes. It also sets the flag _initialized to 1
		 * @param nTuples Number of tasks received
		 * @param sizeBytes Size of the task in bytes
		 */
		void OnTupleStart (int nTuples, int sizeBytes);

		/**
		 * @brief Returns the value of the _taken flag
		 * @return _taken
		 */
		bool IsTaken(){ return _taken;};

		/**
		 * @brief Returns if the task has been initialized or not
		 * @return _initialized
		 */
		bool IsInitialized(){ return _initialized;};
		//void Dump () const;

	private:
		//int _IdxWorker; // es el ID del multiset de batch data
		int _nNumTaskReceived;
		int _sizeTaskBytes;

		bool _taken;
		bool _initialized;

		double _StartCalcTimeMs;
		double _EndCalcTimeMs;
		double _ComputingTimeMs;
};

/**
 * @brief  Statistics of a single batch
 */
class BatchData {
	typedef std::multimap<int, WorkerData *> MMap;

	public:
        /**
         * @brief Constructor
         * @param batchIdx
         */
		BatchData (int batchIdx);

		/**
		 * @brief Destructor
		 */
		~BatchData ();

		/**
		 * @brief Sets the number of chunks to *numChunks*
		 * @param numChunks Number of chunks
		 */
		void OnNewBatch(int numChunks) {
			_numChunks = numChunks;
			//Syslog::Debug ("[BatchData::OnNewBatch] numChunks=%d", _numChunks);
		}

        /**
         * @brief Getter of the data in worker given by workerTid. If it does not exist,
         * creates a new one and returns it.
         * @param workerTid Worker ID
         * @return WorkerData object
         */
		WorkerData & GetWorkerData (int workerTid);

        /**
         * @brief Creates a new worker data in workerTid and returns it. If it already exists, returns it.
         * @param workerTid Worker ID
         * @return WorkerData Object
         */
        WorkerData & NewWorkerData (int workerTid);

        /**
         * @brief Checks if the current BatchData is complete.
         * @return boolean
         */
        bool IsComplete() const {
			Syslog::Debug ("[BatchData::IsComplete] numChunks=%d NW=%d", _numChunks, _mapW.size());
			return AreWorkersComplete() && (_numChunks == _mapW.size());
		}

        /**
         * @brief Getter of the _flagActualize private var
         * @return returns _flagActualize
         */
		bool IsActualize () const {
			 return _flagActualize;
		}

        /**
         * @brief Sets _flagActualize to 1
         */
		void SetActualize() { _flagActualize = 1; }

        /**
         * @brief Checks if all the workers **in BatchData** are complete.
         * @return boolean
         */
        bool AreWorkersComplete () const;

		/**
		 * @brief Allocates a dynamic array of WorkerData and returns it
		 * @return WorkerData array
		 */
		WorkerData ** AllocWorkersArray ();

        /**
         * @brief Not implemented
         * @return
         */
        double DeviationComputingTime();

        /**
         * @brief Not implemented
         * @return
         */
        double MeanComputingTime();

		/**
		 * @brief Getter of _numChunks
		 * @return _numChunks
		 */
		int GetNumChunks() const { return _numChunks; }
		//int GetTotalTasks() const { return _numTotalTasks; }

		/**
		 * @brief Calculates and returns the mean task processing time
		 * @return mean time in **seconds?**
		 */
		double GetMeanStats();

		/**
		 * @brief Calculates and returns the standard deviation of the tasks
		 * @return standard deviation
		 */
		double GetStdStats();

		/**
		 * @brief Adds *sizeTasks* to _TotalTaskReceived
		 * @param sizeTasks
		 */
		void SizeTaskReceived(int sizeTasks){
			 _TotalTaskReceived += sizeTasks;
			 //Syslog::Debug ("[BatchData::SizeTaskReceived] total=%d", _TotalTaskReceived);}
		}

		/**
		 * @brief _TotalTaskReceived getter
		 * @return _TotalTaskReceived
		 */
		int GetSizeTaskReceived() const { return _TotalTaskReceived; }

		/**
		 * @brief Returns the ModelParam object with TotalDataVolume, TotalDataSendW and TotalCompTime updated
		 * @return ModelParam object
		 */
		ModelParam GetModelParam();

		//void Dump () const;
	

	private:
		int     _batchIdx;
		int 	_numChunks; 			//Total number of chunks
		int     _TotalTaskReceived;
		double  _meanTaskProcessTime;
		double  _stdTaskProcessTime;
		bool    _flagActualize;
		MMap     _mapW;	 				//WorkerTid -> worker data
		//long_t _TotalComputingTimeMs; //I don't know if it's necessary
		//int 	_numTotalTasks; 		//total number of task including in this batch (????)
};

/**
 * @brief Statistics for a single iteration
 */
class IterData {
	typedef std::map<int, BatchData *> Map;
	
	public:
		/**
		 * @brief Constructor
		 * @param iterIdx ID of the iterator
		 */
		IterData (int iterIdx);

		/**
		 * @brief Destructor
		 */
		~IterData ();

		/**
		 * @brief Sets the flag of the iteration started to 1, the iteration starting time to _time_,
		 * the number of tasks as _numTuples_, the number of workers to _nw_ and the size in Bytes to _sizeBytes_.
		 * @param time Starting time of the iteration in milliseconds
		 * @param numTuples Number of tasks
		 * @param sizeBytes Task size in Bytes
		 * @param nw Number of workers
		 */
		void OnIterStart (long_t time, int numTuples,
				 	 	 int sizeBytes, int nw); //sizeBytes of a single tuple

		/**
		 * @brief Sets the flag of the finishing iteration to 1 and makes sure that the final time stated in _time_
		 * is greater than the starting one. Finally, it computes the iteration's elapsed time.
		 * @param time Ending time of the iteration.
		 */
		void OnIterEnd (long_t time);

		/**
		 * @brief Increments the number of batches by 1
		 */
		void OnNewBatch(){_nbatchs++;}

		/**
		 * @brief Gets the data in a batch by specifying the batch ID.
		 * If it's the last node from the iterator, adds a new BatchData object and returns it.
		 * @param IdxBatch
		 * @return Data of the batch in a BatchData object
		 */
		BatchData & GetBatchData (int IdxBatch);

		/**
		 * @brief Checks if the current iteration has a start and an end and whether
		 * all batches are complete.
		 * @return True if the current iteration is complete or False if not
		 */
		bool IsComplete () const
		{
			Syslog::Debug ("[IterData::IsComplete] flagEnd=%d flagStart=%d",
							_flagIterEnd, _flagIterStart);
			return (_flagIterEnd == 1) &&
				   (_flagIterStart == 1) &&
				   AreBatchsComplete();
		}

		/**
		 * @brief Checks if all batches are complete.
		 * @return True if all batches are complete and False if not.
		 */
		bool AreBatchsComplete() const;
		//bool IterData::PreviousBatchComplete(int IdxBatch);

		/**
		 * @brief Getter of the tuple size in bytes
		 * @return _sizeBytes
		 */
		int GetTupleSizeInBytes() const { return _sizeBytes; };

		/**
		 * @brief Allocates a new batch data for each element in the array _mapB
		 * @return Array with all batches allocated
		 */
		BatchData ** AllocBatchsArray();

		/**
		 * @brief Getter of the number of workers
		 * @return _numWorkers
		 */
		int GetNumWorkers() { return _numWorkers; };

		/**
		 * @brief Getter of the number of tasks
		 * @return _numTasks
		 */
		int GetTotalTasks() const { return _numTasks; };

		/**
		 * @brief Getter of the number of batches
		 * @return _nbatchs
		 */
		int GetNumBatchs() const { return _nbatchs;};

		//void Dump () const;

	private:
		int     _iterIdx;
		int 	_numTasks;            // total number of tasks to process during this iteration
		int 	_sizeBytes;           // size in bytes of a single task
		int     _numWorkers;          //number of workers in a iteration
		double  _iterStartTimeMs;     //star timestap iteration
		double  _iterTimeMs;          //iteration time
		int     _flagIterStart;
		int     _flagIterEnd;
		int     _nbatchs;
		Map     _mapB;	              // batchTid -> batch data
};

#endif
