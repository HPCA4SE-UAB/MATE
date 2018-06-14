#if !defined UTILS_H
#define UTILS_H

// TUNING HELPER

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
//#include <rtl.h>

typedef long long hrtime_t;
extern FILE * masterLog;
extern FILE * workerLog;

FILE * OpenFile (char const * fileName, int tid, int append);
void CloseFile (FILE * f);
void InitMasterLogFiles (int tid);
void InitWorkerLogFiles (int tid);
void CloseMasterLogFiles ();
void CloseWorkerLogFiles ();

void Info (FILE * file, char * formatStr, ...);

double GetAverageTime (double sumTime, int iter);
double GetError (double sumTime, int iter, double sumError);
long long GetCurrentTimeMS (); // hrtime_t -> 64bit int

float Proc (float a, float b);


#endif

