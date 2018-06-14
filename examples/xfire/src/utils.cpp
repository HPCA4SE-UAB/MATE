#include <stdarg.h>
#include <cassert>
#include "utils.h"
#include <sys/time.h>
#include <unistd.h>

static char const * masterLogFileName = "xmaster";
static char const * workerLogFileName = "xworker";

FILE * masterLog = 0;
FILE * workerLog = 0;

void Info (FILE * file, char * formatStr, ...)
{
	char message [512];
	va_list argp;
	va_start (argp, formatStr);
	vsprintf (message, formatStr, argp);
	fprintf (file, message);
	fflush (file);
}

void InitMasterLogFiles (int tid)
{
	masterLog = OpenFile (masterLogFileName, tid, 0);
	assert (masterLog != 0);
}

void InitWorkerLogFiles (int tid)
{
	workerLog = OpenFile (workerLogFileName, tid, 0);
	assert (workerLog != 0);
}

void CloseMasterLogFiles ()
{
	CloseFile (masterLog);
}

void CloseWorkerLogFiles ()
{
	CloseFile (workerLog);
}

FILE * OpenFile (char const * fileName, int tid, int append)
{
	char path [256];
	FILE * f;
	sprintf (path, "%s_%d.log", fileName, tid);
	if (append == 1)
		f = fopen (path, "a");
	else 
		f = fopen (path, "w");
	if (f == NULL)
	{
		fprintf (stderr, "Cannot open file: %s", path);
		exit (-1);
	}
	fprintf (stderr, "File %s open\n", path);
	fflush (stderr);
	return f;
}

void CloseFile (FILE * f)
{
	int err = fclose (f);
	if (err)
		fprintf (stderr, "Cannot close file");
}

double GetAverageTime (double sumTime, int iter)
{
	return (sumTime / (double)iter);
}

double GetError (double sumTime, int iter, double sumError)
{
	double sum2 = (double)sumTime * (double)sumTime;
	double count = (double)iter * (double)iter;
	double diff = sumError/(double)iter - (sum2/count); 
	if (diff > 0)
		return sqrt (diff);
	else
	{
		fprintf (stderr, "Negative value in getError %f\n", diff);
		return 0.0;
	}
}

long long GetCurrentTimeMS ()
{
//	 return gethrtime () / 1000000;

 struct timeval t;
   gettimeofday (&t, 0);
     //time in miliseconds, (from 1.1.1970 UTC)
//   return (t.tv_sec) * 1000 + (t.tv_usec/1000); 
//PAOLA: CAMBIADO 08.04.04
    return ((long long)t.tv_sec)*1000 + ((long long)t.tv_usec)/1000;

}

float Proc (float a, float b)
{
	assert (b != 0);
	return (a / b) * 100;
}



