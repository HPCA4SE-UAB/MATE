/*
** ----------------------------------------------------------------------
**
** FIRESLAVE:			A PVM SLAVE FOR XFIRE PROGRAM
**
**  (C-) Josep Jorba, 1997		C.A.O.S. - UAB
** 
** ----------------------------------------------------------------------
**
** Date			Who			What
** 19-07-98		Josep Jorba		Creation
**
** ----------------------------------------------------------------------
*/

#ifndef _FIRESLAVEH_
#define _FIRESLAVEH_

#include "gisfuncs.h"
#include "factor1.h"
#include "comm_utils.h"

/* Info for a point */
typedef struct{
  double windspeed;
  double winddirection;
  double m;
  double mangle;
  int vegetmodel;
} pointsInfo;

typedef pointsInfo* ppointsInfo;

/* Slave Init & End */
void slaveInit(comm_info_slave* p, pvegetModels* v,int* dtime);
void slaveEnd(comm_info_slave* p, pvegetModels* v);

/* Receive arc info */
void getArcInfo(pFIRELINE* f,int* arcinit,int* arcend,ppointsInfo* pInf, int * batchIdx);

/* Get local parameters of a point */
void getLocalParams(pvegetModels* veget, int numberpoint, ppointsInfo *ptsInf,
		    pointLine* p, plocalParams plocal);

//***************
void cargarmatrices(int f, int c);

//***************

int slaveMain(int, char**);

/* kernel of one simulation step */
void arcStepKernel(int iterIdx, int batchIdx, pFIRELINE *f,pvegetModels *v,ppointsInfo *ptsInf,int dtime, int titerIdx, int tbatchIdx);


/* Task Output -> arc points */
void taskOutput(comm_info_slave* p,pFIRELINE *f,int arcinit,int arcend);

/* reply to master with the arc result */
void resultstoMaster(comm_info_slave* p,pFIRELINE *f,int arcinit,int arcend, int batchIdx);

/* one step of simulation */
int simulationStep(comm_info_slave* p, pvegetModels* veget,int dtime);
#endif
