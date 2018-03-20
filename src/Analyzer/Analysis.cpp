
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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "Tune.h"
#include <vector>
#include <map>

FILE * output;


//tunable params
double TS = 0.5;	//ms
double TW = 0.0025; //ms

/**
 * @class TaskStats
 * @brief Class that deals with the statistics of a certain task e.g. the communication costs,
 * optimal fragment size or the total number of changes
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 1.0b
 */
class TaskStats {
	public:

        /**
         * @brief Constructor
         * @param tid Id to assign to the task
         */
		TaskStats (int tid)
			: _tid (tid), _nMsgs (0), _sumSize (0), _fragSize (4090), _nChanges (0),
			  _lastOptFS (0)
		{}

		/**
		 * @brief Changes the fragment size to the one provided by the parameter and increments _nChanges by 1
		 * @param size Value of the new frag size
		 */
		void ChangeFragSize(int size) {
			_fragSize = size;
			++_nChanges;
		}

        /**
         * @brief Updates the number of messages by _count_ and the sum size by _size_
         * @param count Increments the number of messages by its value
         * @param size Increments the sum size by its value
         */
		void Update(int count, int size) {
			_nMsgs += count;
			_sumSize += size;

			fprintf (output, "%d\t%lld\t%lld\n",  _tid, (longlong_t)_nMsgs,
					(longlong_t)_sumSize);
			fflush (output);
			/*int opt = GetOptimalFragSize ();
			if (_lastOptFS != opt) {
				_lastOptFS = opt;
				fprintf (output, "Task %d stats: NumMsgs=%lld, SumSize=%lld,
						FragSize=%d, OptFragSize=%d\n", _tid, (longlong_t)_nMsgs,
						(longlong_t)_sumSize, _fragSize, opt);
				fflush (output);
			}
			*/
		}

        /**
         * @brief Returns the communication costs by multiplying the number of messages
         * and the sum size by two factors and then adding their results together
         * @return Communication costs in _ms_
         */
		double GetCommCost() const {
			return _nMsgs* TS + _sumSize * TW;
		}

        /**
         * @brief Finds the optimal fragment size based on the current number of messages and
         * the sum size
         * @return Optimal fragment size
         */
		int GetOptimalFragSize() const {
			if (_nMsgs == 0)
				return _fragSize;
			int avg = (int)ceil (_sumSize / _nMsgs);
			int optSize = FindNearestGreaterPow2 (avg);
			if (optSize < 4090)
				optSize = 4090;
			if (optSize > 512*1024)
				optSize = 512*1024;
			return optSize;
		}

        /**
         * @brief Getter of the current fragment size
         * @return _fragSize
         */
		int GetCurrentFragSize () const { return _fragSize; }

        /**
         * @brief Getter of the current number of changes
         * @return _nChanges
         */
        int GetNumChanges () const { return _nChanges; }

		/**
		 * @brief Getter of the Tid
		 * @return _tid
		 */
        int GetTid () const { return _tid; }

	private:
		static int FindNearestGreaterPow2(int number) {
			int x = 2;
			while (x < number) {
				x = x * 2;
			}
			return x;
		}

	private:
		int 	_tid;
		int		_fragSize; 	// current frag size
		double	_nMsgs; 	// number of messages send/received
		double	_sumSize;	// total msg. size
		int		_nChanges;	// num. fragSize changes applied by tuning
		int 	_lastOptFS;
};

std::map<int, TaskStats *> StatsMap;

//vector of master & slave tids
std::vector<int> tids;
int FragSizeTuning = 0;
hrtime_t FragSizeTuningStart;

void SaveFragSizeFile (int tid, int optFragSize)
{
	char buf[256];
	sprintf (buf, "/users/projects/ania/fs.%d", tid);
	
	// PROTEZA
	optFragSize = 65535;

	fprintf (output, "Saving optimal frag size %d to file %s\n", optFragSize, buf);	
	FILE * f = fopen (buf, "w+");
	char val[20];
	sprintf (val, "%d", optFragSize);
	fprintf (f, "%s\n", val);
	fflush (f);
	fclose (f);
}

void TuneRoutingEncoding ()
{
	fprintf (output, "Tuning encoding and/or routing...\n");
	for (int i = 0; i < tids.size(); ++i) {
		int tid = tids [i];
		pvm_sendsig (tid, SIG_TUNE_ROUTING_ENCODING);
	}
	fprintf (output, "Tuning actions sent\n");	
	fflush (output);
}

void StartFragSizeTuning ()
{
	fprintf (output, "Starting FRAGSIZE tuning...\n");
	for (int i=0; i<tids.size (); ++i)
	{
		int tid = tids [i];
		pvm_sendsig (tid, SIG_TUNE_TOOGLE_FRAG_SIZE_INSTR);
	}	
	++FragSizeTuning;
	FragSizeTuningStart = gethrtime ();
	fprintf (output, "FRAGSIZE instrumentation request send to tasks...\n");
	fflush (output);
}

void StartAnalysis ()
{
	sleep (BEFORE_ANALYSIS_TUNING_DELAY);
	fprintf (output, "Starting analysis...\n");
	fflush (output);
#if defined ENABLE_PVM_ENCODE_TUNING
	fprintf (output, "ENABLE_PVM_ENCODE_TUNING...\n");
	fflush (output);
#endif
#if defined ENABLE_PVM_ROUTING_TUNING
	fprintf (output, "ENABLE_PVM_ROUTING_TUNING...\n");
	fflush (output);
#endif

#if defined (ENABLE_PVM_ENCODE_TUNING) || defined (ENABLE_PVM_ROUTING_TUNING)	
	TuneRoutingEncoding ();
	
	sleep (AFTER_ROUTE_ENCODE_TUNING_DELAY);
#endif

#if defined (ENABLE_FRAGSIZE_TUNING)
	StartFragSizeTuning ();
#endif
			
}

void OnSpawnTask (int senderTid)
{
	tids.push_back (senderTid);
	int nSlaves;
	// unpack num. slaves
	pvm_upkint (&nSlaves, 1, 1);
	fprintf (output, "Num slaves = %d\n", nSlaves);
	for (int i=0; i<nSlaves; ++i)
	{
		int slaveTid;
		pvm_upkint (&slaveTid, 1, 1);
		tids.push_back (slaveTid);
		StatsMap [slaveTid] = new TaskStats (slaveTid);
	}
	fprintf (output, "Received SPAWN_TASK event. %d tasks detected\n", tids.size ());
	fflush (output);
	StartAnalysis ();
}

void CheckFragSizeActivation (int tid, TaskStats & stats)
{
	hrtime_t t0 = gethrtime ();
	// what should we do:
	if (stats.GetNumChanges () >= MAX_FRAGSIZE_CHANGES)
	{
		// nop	
		return;
	}	
	int curFragSize = stats.GetCurrentFragSize ();
	int optFragSize = stats.GetOptimalFragSize ();	
	int delta = abs (optFragSize - curFragSize);
	if (delta < 0.25 * curFragSize)
	{
		// too small change
		/*fprintf (output, "CHECK [TID=%d, FS=%d, OptFS=%d, Delta=%d, Too small.\n",
			tid, curFragSize, optFragSize, delta);*/
		return;
	}	
		
	// calculate instr. time in nanos
	double totalCost = (double) ( (t0 - FragSizeTuningStart) / 1000000L );
	double commCost = stats.GetCommCost () / totalCost;
	
	if (totalCost >= MIN_COST_THRESHOLD && commCost >= COMM_TUNING_TRESHOLD)
	{
		fprintf (output, "Tuning task TID=%d from FRAGSIZE=%d to FRAGSIZE=%d\n",
			stats.GetTid (), curFragSize, optFragSize);		
		//	 - tune frag size
		SaveFragSizeFile (tid, optFragSize);
		pvm_sendsig (tid, SIG_TUNE_CHANGE_FRAG_SIZE);		
		fprintf (output, "Tuning signal sent\n");
		stats.ChangeFragSize (optFragSize);
		
		if (stats.GetNumChanges () >= MAX_FRAGSIZE_CHANGES)
		{
			fprintf (output, "Max number of fragSize changes reached (%d) in task TID=%d. Tuning instr. off\n", 
				MAX_FRAGSIZE_CHANGES, stats.GetTid ());
			
			//SaveFragSizeFile (tid, optFragSize);
			
			//	 - stop instrumentation
			pvm_sendsig (tid, SIG_TUNE_TOOGLE_FRAG_SIZE_INSTR);		
			return;
		}
		fflush (output);
	}	
	else
	{
		/*fprintf (output, "CHECK [TID=%d, TotalCost=%f, CommCost=%f, No action\n",
			tid, totalCost, commCost);*/
	}
	
}

void OnSendEvent (int senderTid)
{
	int count, size;
	pvm_upkint (&count, 1, 1);
	pvm_upkint (&size, 1, 1);
#if defined (ENABLE_FRAGSIZE_TUNING)	
	if (FragSizeTuning)
	{
		// FragSizeTuning must be started
		TaskStats * stats = StatsMap [senderTid];
		assert (stats != 0);
		stats->Update (count, size);
		
		// check if we should react
		//CheckFragSizeActivation (senderTid, *stats);
	}
#endif	
}

void HandleEvents (int myTid)
{
	int tid, tag, size;
	
	while (true)
	{
		int bufid = pvm_recv (-1, -1);
		pvm_bufinfo (bufid, &size, &tag, &tid);
		switch (tag)
		{
		case MSG_SPAWN_TASK_EVENT:
			OnSpawnTask (tid);
			break;
		case MSG_SEND_EVENT: // frag_size instr event
			OnSendEvent (tid);
			break;
		default:
			fprintf (output, "Unknown msg tag (%d) received	from TID=%d\n", tag, tid);
			break;
		}
	}
}

// PVM child task that acts as a Performance Analyzer
int main (int argc, char ** argv)
{
	printf ("Starting performance analyzer\n");
	hrtime_t t1 = gethrtime ();
	output = fopen ("/users/projects/ania/pvm3/bin/SUN4SOL2/analyzer.log", "w+");
	if (output == 0)
	{
		fprintf (stderr, "Cannot create analyzer log file\n");
		return -1;
	}
	int tid = pvm_mytid ();
	fprintf (output, "Analyzer TID=%d\n", tid);
	fflush (output);
	// use direct comm.
	pvm_setopt (PvmRoute, PvmRouteDirect);
	
	HandleEvents (tid);
	
	fclose (output);
	hrtime_t t2 = gethrtime ();
	fprintf (output, "Analyzer done\n");
	fflush (output);
}

