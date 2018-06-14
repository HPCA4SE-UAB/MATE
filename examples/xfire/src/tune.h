#ifndef TUNE_H
#define TUNE_H

// FACTORING TUNING
#include "utils.h"
#include <assert.h>
#include <stdio.h>

#define MAX_NUM_TUPLES 10000
#define MAX_NUM_PROCS  10
#define MAX_NUM_BATCHES 10000

struct batch{
       float factor;
       int numTuples;
       int tuples[MAX_NUM_TUPLES];       
};

class Factoring 
{
public:
	Factoring () 
		: _numBatches (0)
	{}
	
	Factoring (Factoring const & f)
		:  _numBatches (f.GetNumBatches ())
	{
        for (int j= 0; j<_numBatches; j++){
            _batch[j].factor = f._batch[j].factor;
            _batch[j].numTuples = f._batch[j].numTuples;            
		    for (int i=0; i<_batch[j].numTuples; i++)
			    _batch[j].tuples [i] = f._batch[j].tuples [i];
       }
	}
	Factoring const & operator= (Factoring const & f)
	{
		_numBatches = f.GetNumBatches ();
		for (int j= 0; j<_numBatches; j++){
            _batch[j].factor = f._batch[j].factor;
            _batch[j].numTuples = f._batch[j].numTuples;            
		    for (int i=0; i<_batch[j].numTuples; i++)
			    _batch[j].tuples [i] = f._batch[j].tuples [i];
       }
	   return *this;
	}
	double GetFactor (int idBatch) const {return _batch[idBatch].factor;}
	int GetNumTuples (int idBatch) const {return _batch[idBatch].numTuples;}
	int * GetTuples (int idBatch) {return _batch[idBatch].tuples;}
	int GetTuple (int idBatch, int idTuple) const {return _batch[idBatch].tuples [idTuple];}
	
	int GetNumBatches () const {return _numBatches;}
	
	void Reset ()
	{				
		for(int j=0; j<MAX_NUM_BATCHES;j++){ 
            _batch[j].factor = 0.0; 
            _batch[j].numTuples = 0;            
    		for (int i=0; i<MAX_NUM_TUPLES; i++)
	    		_batch[j].tuples [i] = 0;		
        }
	}
	
	void SetNumBatches(int numBatches)
	{
         _numBatches = numBatches;
    }
    
	void SetFactor (float factor, int idBatch) 
	{
		assert (factor > 0.0);
		assert (factor <= 1.0);
		assert (idBatch >=0 && idBatch < MAX_NUM_BATCHES);
		_batch[idBatch].factor = factor;
	}
	void SetNumTuples (int idBatch, int numTuples) 
	{
		assert (numTuples <= MAX_NUM_TUPLES);
		assert (idBatch >=0 && idBatch < MAX_NUM_BATCHES); 
		_batch[idBatch].numTuples = numTuples;
	}
	void SetTuple (int idBatch, int idTuple, int value) 
	{
    	printf ("Set tuple");		
        assert (idTuple >=0 && idTuple < MAX_NUM_TUPLES);
        assert (idBatch >=0 && idBatch < MAX_NUM_BATCHES);        
		_batch[idBatch].tuples [idTuple] = value;
    	printf ("Tupla[%d] = %d \n", idTuple, _batch[idBatch].tuples[idTuple]);				
	}
	void AddTuple (int idBatch, int idTuple, int value) 
	{
        assert (idTuple >=0 && idTuple < MAX_NUM_TUPLES);
        assert (idBatch >=0 && idBatch < MAX_NUM_BATCHES);        
		_batch[idBatch].tuples [idTuple] = _batch[idBatch].tuples [idTuple] + value;    	

	}

	int GetSumTuples (int idBatch) const
	{
		int sum = 0;
		int numTuples = _batch[idBatch].numTuples;
		for (int i=0; i<numTuples; i++)
			sum +=_batch[idBatch].tuples [i];
		return sum;
	}

private:
	int     _numBatches;	
	batch 	_batch [MAX_NUM_BATCHES]; 
};

// factoring functions
int Factoring_CalcTuples (int numProcs, int totalWork, int workUnitSizeBytes, float factor, int idBatch);
int Factoring_GetTuple (int idBatch, int index);
int Factoring_GetNextTuple (int idBatch, int idTuple);
int Factoring_GetTotalNumTuples (int idBatch);
int Factoring_HasMoreTuples (int idBatch, int idTuple);
int Factoring_HasMoreBatch (int idBatch);
int Factoring_HasSufTuples(int idBatch, int idTuple);


#endif
