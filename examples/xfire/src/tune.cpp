#include "tune.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

const int MIN_T = 16;

static Factoring TheFactoring;
//static int TheCounter = 0;

void Factoring_SetNumTuples(int idBatch, int tupleIdx)
{
     TheFactoring.SetNumTuples(idBatch, tupleIdx);
}

//Función modificada para que el procesamiento tenga en cuenta los batch (sirve para la primera iteracion) 
int CalcWorkTuples (int numProcs, int totalWork, float factor, Factoring & factoring, int batchIdx)
{
	
	int i, tupleIdx = 0, tupleSize = 0, workLeft;
	int T = MIN_T;
	workLeft = totalWork;
	
	assert (numProcs > 0);
	assert (totalWork > 0);
	
	printf("Dentro de CalcWorkTuples Factor=%f\n", factor);
	assert (factor > 0);
	
	//factoring.Reset ();
		
    // Calculate the chunk's size based on model performance
	tupleSize = (int)floor ( (double)workLeft / (double)numProcs)* factor;
	
    printf ("Tareas totales %d, tupleSize %d\n", workLeft, tupleSize);
	
	if (tupleSize < T)	
		tupleSize = T;
	for (i=0; i<numProcs && workLeft >= tupleSize; i++)
	{
		factoring.SetTuple (batchIdx, tupleIdx, tupleSize);
		workLeft -= tupleSize;
		printf ("for i=%d workLeft %d\n", i, workLeft);
		if (workLeft < T)
		{
            printf ("Addtuple\n");
			factoring.AddTuple (batchIdx, tupleIdx, workLeft);
			workLeft = 0;
		} 
		++tupleIdx;
	}

	//factoring.SetNumTuples (batchIdx, tupleIdx);
	Factoring_SetNumTuples(batchIdx, tupleIdx);
    factoring.SetFactor (factor, batchIdx);
    factoring.SetNumBatches(batchIdx+1);
    
    return workLeft;

}
// API
int Factoring_CalcTuples (int numProcs, int totalWork, int workUnitSizeBytes, float factor, int idBatch)
{
	int workLeft;
    workLeft = CalcWorkTuples (numProcs, totalWork, factor, TheFactoring, idBatch); 
	return workLeft;
}

int Factoring_GetTuple (int idBatch, int idTuple)
{
	return TheFactoring.GetTuple (idBatch, idTuple);
}

int Factoring_GetTotalNumTuples (int idBatch)
{
	return TheFactoring.GetNumTuples (idBatch);
}

int Factoring_HasMoreTuples (int idBatch, int idTuple)
{
    //fprintf(stderr, "INSIDE HASMORETUPLES \n");
    //fprintf(stderr, "N.Tuplas=%d, del batch=%d, tuplaActual=%d \n", TheFactoring.GetNumTuples (idBatch), idBatch, idTuple);
	return TheFactoring.GetNumTuples (idBatch) - idTuple;
}

int Factoring_GetNextTuple (int idBatch, int idTuple)
{
	return TheFactoring.GetTuple (idBatch, idTuple);
}

int Factoring_HasMoreBatch (int idBatch)
{
    //fprintf(stderr, "INSIDE HASMOREBATCH \n");
    //fprintf(stderr, "Numero de batchs=%d, idBatch=%d \n", TheFactoring.GetNumBatches(), idBatch);
	return TheFactoring.GetNumBatches() - idBatch -1;
}

//Hay chunk suficientes todavia si el número de chunk que quedan por enviar es superior a la mitad del número total de chunk de ese batch
int Factoring_HasSufTuples(int idBatch, int idTuple)
{
    return (TheFactoring.GetNumTuples(idBatch)- idTuple > (TheFactoring.GetNumTuples(idBatch)/2));      
    
}







	
