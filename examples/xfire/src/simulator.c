/*
** ----------------------------------------------------------------------
**
** FILE:	Simulator Routines 
**
**  (C-) Josep Jorba, 1997		C.A.O.S. - UAB
** 
** ----------------------------------------------------------------------
**
** Date			Who		What
** 04-02-98		JJ		Creation
**
** ----------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <mpi.h>
#include "gisfuncs.h"
#include "vegetmod.h"
#include "windmod.h"
#include "globmod.h"
#include "sysio.h"
#include "comm_utils.h"
#include "simulator.h"

// tuning
#include "tune.h"

void simulator_setup(ptimeSim* t, char* timeFile)
{
  if (*t != NULL) free(*t);
  *t=(timeSim *) malloc(sizeof(timeSim));
  (*t)->hinit=(*t)->minit=0;
  (*t)->hend=(*t)->mend=0;
  (*t)->hinc=(*t)->minc=0;

  simulator_readTimeFile(t,timeFile);    /* Llegir fitxer de temps */
}

void simulator_unsetup(ptimeSim* t)
{
  if (*t != NULL) free(*t);
}

void simulator_statistics(ptimeSim* t)
{
  ptimeSim pt=NULL;

  pt=*t;
  fprintf(stderr, "Time statistics:\n");
  fprintf(stderr, "Simulation starts at %d:%d to %d:%d in increment %d:%d\n",
	  pt->hinit,pt->minit,pt->hend,pt->mend,pt->hinc,pt->minc);
}

void simulator_readTimeFile(ptimeSim* t, char* name)
{
  FILE *fp;
  ptimeSim pt=NULL;

   if ((fp=fopen(name,"r"))==NULL)
    { printf("Time File error. exit...\n");exit(1);}
  else
    {
      pt=*t;
      fscanf(fp,"%d:%d",&(pt->hinit),&(pt->minit));
      fscanf(fp,"%d:%d",&(pt->hend),&(pt->mend));
      fscanf(fp,"%d:%d",&(pt->hinc),&(pt->minc));
      fclose(fp);
    }
}

void send_initParams(int tid, comm_info_master *p,pvegetModels *v,ptimeSim* stime)
{
	int n,i;
	pvegetModels pv;
	ptimeSim ptime;
	int inctime;

	pv=*v;
	ptime=*stime;
  
    mpi_pack_init ();
    mpi_pkint(&(pv->nSubmodels),1,1);
    for (i=0;i<pv->nSubmodels;i++)
	{
		/* submodel code */
	  	mpi_pkint(&(pv->submodels[i].submodelCode),1,1);
	  	/* submodel params */
	  	mpi_pkdouble(pv->submodels[i].params,MAX_PARAMS,1);
	}
    inctime=ptime->hinc*60+ptime->minc;
    mpi_pkint(&inctime,1,1);
	MPI_Send (
		mpi_pack_buf (),
		mpi_pack_buf_pos (),
		MPI_PACKED,
		tid, 
		PACKET_VEGT, 
		MPI_COMM_WORLD); 
}

void simulator_loop(comm_info_master *p, ptimeSim* t, pMAP* map, 
		    pvegetModels* veget, pFIRELINE* front, pwindPoints* wind)
{  
	ptimeSim ptime=NULL;
	int simulTime,mininit,minend,mininc;
	double mpit1, mpit2;
	unsigned long n; /* comptador numero de fitxer */
	TBOOL nextSimulation; /* If the front out of land, stop the simulation */
	pFIRELINE pfront = *front;
	
	mpit1 = MPI_Wtime ();
	
	/* Agafar punters als diferents objectes */
	ptime=*t;
	
	/* Temps de simulacio, passar els temps a minuts */
	/* CHANGE: no es tenen en compte dates, tot es dintra del mateix dia */
	simulTime=0;
	mininit=ptime->hinit*60L+ptime->minit;
	minend=ptime->hend*60L+ptime->mend;
	mininc=ptime->hinc*60L+ptime->minc;
	n=0L;
	
	printf("Validating fireline...\n");
	check_fireline ("Before simulation", *front);
	
	printf("Simulating...\n");
	for (simulTime=mininit;simulTime<=minend;simulTime=simulTime+mininc)
	{
  		printf("Iteration %d/%d (%d points), ", (1+n), (int)(minend/mininc)+1, pfront->nPoints);
  	  	fflush (stdout);

        /* Generar sortida de simulacio per aquest interval */
      	/* Crida al model global */
      	//nextSimulation=global_model(n,p,map,veget,front,wind,simulTime,mininc, t, &schedData, theScheduler);
      	nextSimulation=global_model(n,p,map,veget,front,wind,simulTime,mininc, t);
      	n++;

      	/* Are the front in land? */
      	if (nextSimulation==TFALSE)
  		{
  			printf("Front out in land.\n");
  			printf("Simulated from %d to %d by inc %d\n", mininit,simulTime,mininc);
    		break;
  		}
	}

	mpit2 = MPI_Wtime ();
	printf ("\nTOTAL TIME=%.2f [sec]\n", (mpit2-mpit1));
}
