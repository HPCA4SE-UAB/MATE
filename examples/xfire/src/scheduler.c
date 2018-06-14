
#include "scheduler.h"
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

// tunable variables
static int 	 FCS_ChunkSize 		= 5;		// FSC S factor
static float Factoring_X 		= 2.0; 		// MATE tunable
static int   Factoring_R 		= 1000;		// min tuple size	
static float DynFactoring_X0 	= 2.0;	 	// MATE tunable
static int   DynFactoring_R  	= 1000;		// min tuple size		

static float Def_DynFactoring_UC 	= 5.0; // def ms avg
static float Def_DynFactoring_SIGMA = 2.0; // def stddev

int Schedule_GetTotalNumTuples (SCHEDULE * sched)
{
	assert (sched != 0);
	return sched->totalNumTuples;
}

// returns next tuple to be processed
int Schedule_GetNextTuple (SCHEDULE * sched)
{
	assert (sched != 0);
	assert (sched->lastTupleIndex >=0 && sched->lastTupleIndex < MAX_TUPLES_LIMIT);
	return sched->tuples [sched->lastTupleIndex++];
}

// return non-zero value if there are more tuples left
int Schedule_HasMoreTuples (SCHEDULE * sched)
{
	assert (sched != 0);
	return sched->lastTupleIndex < sched->totalNumTuples;
}
	
SchedulingMode GetSchedModeByName (const char * schedName)
{
	if (schedName == 0 || strlen (schedName) == 0)
	{
		printf ("Scheduling mode: default\n");		
		return smStatic;
	}
	else
	{
		printf ("Scheduling mode: %s\n", schedName);
		if (strcmp (schedName, "ss") == 0)
			return smStatic;
		else if (strcmp (schedName, "fsc") == 0)
			return smFixedChunks;
		else if (strcmp (schedName, "f") == 0)
			return smFactoring;
		else if (strcmp (schedName, "df") == 0)
			return smDynamicFactoring;
		else if (strcmp (schedName, "dft") == 0)
			return smDynamicFactoringTunable;
		else
			assert (!"Unknown scheduling mode");
	}
}

schedule SCHEDULERS [] =
{
	sched_static,
	sched_fixed_chunks,
	sched_factoring,
	sched_dyn_factoring	, // auto-tunable
	sched_dyn_factoring_tunable //MATE-tunable
};
	
schedule GetScheduler (SchedulingMode mode)
{
	return SCHEDULERS [mode];
}

// different scheduling policies
void sched_static (int nIter, SCHEDULE * s)
{
	assert (s != 0);
	s->lastTupleIndex = 0;
	s->totalNumTuples = s->numSlaves; // one tuple per each worker
	int i;
	int sum = 0;
	int tupleSize = (int)ceil ((double)s->workSize / (double)s->totalNumTuples);
	for (i=0; i<s->numSlaves; ++i)
	{		
		s->tuples [i] = tupleSize;
		sum += tupleSize;
	}
	// assign the difference to the first worker
	s->tuples [0] += (s->workSize-sum); 	
}
	
// fixed-size scheduling
void sched_fixed_chunks (int nIter, SCHEDULE * s)
{
	assert (s != 0);
	s->lastTupleIndex = 0;
	s->totalNumTuples = 0;
	// N/(S*P)	
	int tupleSize = s->workSize / (s->numSlaves * FCS_ChunkSize);
	int workLeft = s->workSize;
	while (workLeft > 0)
	{
		if (tupleSize <= workLeft)
		{
			s->tuples [s->totalNumTuples++] = tupleSize;
			workLeft -= tupleSize;
		}
		else
		{
			// last tuple
			s->tuples [s->totalNumTuples++] = workLeft;
			workLeft = 0;
		}
	}
}
	
// classical factoring
void sched_factoring (int nIter, SCHEDULE * s)
{
	assert (s != 0);
	s->lastTupleIndex = 0;
	s->totalNumTuples = 0;	
	// R=N
	// Repetir
	// Preparar P tuplas de F=R/(X*P)
	// R=R-R/X
	// Hasta terminar datos

	assert (s->workSize > 0);
	assert (Factoring_X > 0);
	
	int workLeft = s->workSize;
	do
	{
		int tupleSize = (int)ceil ( (double)workLeft / (Factoring_X * (double)s->numSlaves));
		if (tupleSize < Factoring_R)
			tupleSize =  Factoring_R;
		for (int i=0; i<s->numSlaves && workLeft >= tupleSize; ++i)
		{
			s->tuples [s->totalNumTuples++] = tupleSize;
			workLeft -= tupleSize;
		}
	} while (workLeft > Factoring_R);
	assert (workLeft >= 0);
	if (workLeft > 0)
	{
		s->tuples [s->totalNumTuples++] = workLeft;
	} 
}
	

void sched_dyn_factoring (int nIter, SCHEDULE * s)
{
	assert (s != 0);
	s->lastTupleIndex = 0;
	s->totalNumTuples = 0;	
	assert (s->workSize > 0);
		
	double x_lote_0, x_lote_n;
	int workLeft = s->workSize;
	if (nIter == 0) // first iteration
	{
		// x0
		x_lote_0 = DynFactoring_X0;
		x_lote_n = DynFactoring_X0;
	}
	else
	{
		// uc = avg work unit calc time
		double uc = s->df_uc;
		double sigma = s->df_sigma;
		// sigma - stddev of work unit calc time
		if (uc == 0)
		{
			x_lote_0 = 0;
			x_lote_n = 0;
		}
		else
		{
			x_lote_0 = (uc + sigma * sqrt ((double)s->numSlaves / 2.0)) / uc;
			x_lote_n = (2.0*uc + sigma * sqrt ((double)s->numSlaves / 2.0)) / uc;
		}
	}
	s->df_x_0 = x_lote_0;
	s->df_x_n = x_lote_n;
	int f;
	int numLote = 0;
	do
	{
		double x = (numLote == 0 ? x_lote_0 : x_lote_n);
		// we must at least create as many tuples as workers
		if (x == 0) // x = 1.0;
			f = 0;
		else
			f = (int)ceil ((double)workLeft / (x * (double)s->numSlaves));
		if (f==0)
			break;
		if (f < DynFactoring_R)
			f = DynFactoring_R;	
		for (int i=0; i<s->numSlaves && workLeft >= f; ++i)
		{
			s->tuples [s->totalNumTuples++] = f;
			workLeft -= f;
		}	
		++numLote;
	}		
	while (workLeft > DynFactoring_R);
	if (workLeft > 0)
	{
		s->tuples [s->totalNumTuples++] = workLeft;
	} 	
}

// pozwala na tuning DynFactoring_X0
void sched_dyn_factoring_tunable (int nIter, SCHEDULE * s)
{
	assert (s != 0);
	s->lastTupleIndex = 0;
	s->totalNumTuples = 0;	
	assert (s->workSize > 0);
		
	double x;
	int workLeft = s->workSize;
	int f;
	// x0
	x = DynFactoring_X0;
	do
	{
		// we must at least create as many tuples as workers
		if (x == 0) // x = 1.0;
			f = 0;
		else
			f = (int)ceil ((double)workLeft / (x * (double)s->numSlaves));
		if (f==0)
			break;
		if (f < DynFactoring_R)
			f = DynFactoring_R;				
		for (int i=0; i<s->numSlaves && workLeft >= f; ++i)
		{
			s->tuples [s->totalNumTuples++] = f;
			workLeft -= f;
		}	
	}		
	while (workLeft > 0);
	if (workLeft > 0)
	{
		s->tuples [s->totalNumTuples++] = workLeft;
	} 
	s->df_x_0 = x;
	s->df_x_n = x;
	//fflush (stdout);
}
	
// utils
void sched_print (SCHEDULE * s)
{
	FILE * f = stdout;
	//fprintf (f, "Scheduling %d units on %d slaves. ", s->workSize, s->numSlaves);
	fprintf (f, "[Tuples: ");
	for (int i=0; i<s->totalNumTuples; ++i)
	{
		if (i==0)
			fprintf (f, "%d", s->tuples [i]);
		else
			fprintf (f, ", %d", s->tuples [i]);
	}
	fprintf (f, "]\n");	
}

void Schedule_Init (SCHEDULE * sched)
{
	memset (sched, 0, sizeof (SCHEDULE));
	sched->df_uc = Def_DynFactoring_UC;
	sched->df_sigma = Def_DynFactoring_SIGMA; 
	sched->df_x_0 = DynFactoring_X0;
	sched->df_x_n = DynFactoring_X0;
}


// Tuning DF nalazy:
// uruchom Xfirep w trybie dft
// wtedy program uzywa X0=const chyba ze MATE go zmieni.
// (mimo tego ze sobie liczy uc i sigma)

// DFTunlet: 
//		events: num slaves, work unit calc time per each slave in each iteration
//	  model: calc uc, calc sigma, calc x
//	  tuning actions: change x0 na x
