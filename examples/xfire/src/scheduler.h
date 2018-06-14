#ifndef SCHEDULER_H
#define SCHEDULER_H


enum SchedulingMode
{
	smStatic			= 0,	// static scheduling (divide work by nº workers equally) => (N/P)
	smFixedChunks		= 1,	// Fixed Size Chunking FSC (param S) => (N/(S*P)
	smFactoring			= 2,	// Factoring Scheduling (FS), params: "f" and "r"
	smDynamicFactoring	= 3,		// Dynamic factoring
	smDynamicFactoringTunable	= 4		// Dynamic factoring (tunable)
};

#define MAX_TUPLES_LIMIT	1024*10

typedef struct _SCHEDULE
{
	// input
	int numSlaves;
	int workSize;		// total work to do in the iteration
	int workSizeUnit;	// bytes per work unit
	// output
	int totalNumTuples;	 // total number of tuples per iteration
	int lastTupleIndex;	 // index of last returned tuple
	int tuples [MAX_TUPLES_LIMIT]; // precalculated tuples 
	// tuning vars
	float df_uc;	 // average exec time of work unit (used in DynFactoring)
	float df_sigma;	 // std-dev for exec time of work unit (used in DynFactoring)
	float df_x_0;		 // last X factor for lote 0	
	float df_x_n;		 // last X factor for lote n
} SCHEDULE;
	
// pointer to a scheduling function
typedef void (*schedule) (int nIter, SCHEDULE * sched);

void Schedule_Init (SCHEDULE * sched);
// read total number of tuples per iteration
int Schedule_GetTotalNumTuples (SCHEDULE * sched);
// returns next tuple to be processed
int Schedule_GetNextTuple (SCHEDULE * sched);
// return non-zero value if there are more tuples left
int Schedule_HasMoreTuples (SCHEDULE * sched);

SchedulingMode GetSchedModeByName (const char * schedName);
schedule GetScheduler (SchedulingMode mode);

// different scheduling policies
void sched_static (int nIter, SCHEDULE * s);
void sched_fixed_chunks (int nIter, SCHEDULE * s);
void sched_factoring (int nIter, SCHEDULE * s);
void sched_dyn_factoring (int nIter, SCHEDULE * s);
void sched_dyn_factoring_tunable (int nIter, SCHEDULE * s);

// utils
void sched_print (SCHEDULE * s);

#endif
