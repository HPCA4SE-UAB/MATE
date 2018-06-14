/*
** ----------------------------------------------------------------------
**
** FIRESLAVE:			A MPI SLAVE FOR XFIRE PROGRAM
**
**  (C-) Josep Jorba, 1998		C.A.O.S. - UAB
** 
** ----------------------------------------------------------------------
**
** Date			Who			What
** 19-07-98		Josep Jorba		Creation
**
** ----------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/stat.h>
//SOLO PARA SOLARIS
//#include <ieeefp.h>

//Jordi
#include <unistd.h>

#include <mpi.h>
#include "comm_utils.h"
#include "factor1.h"
#include "factor2.h"
#include "gisfuncs.h"
#include "vegetmod.h"
#include "fireslave.h"
// tuning
#include "utils.h"

// tuning global vars
hrtime_t idleTime = 0;
hrtime_t calcTime = 0;


extern void check_fireline (char const * region, pFIRELINE front);
/*

{
	int n;
	pointLine * point;
	
	assert (front != 0);
	for (n=0; n<front->nPoints; ++n)
	{
		point = &front->points [n];
		if (point != NULL)
		{
			if (isnan (point->x))
			{
				fprintf (stderr, "%s. NAN detected. Point [%d].x\n", region, n);
				break;
			}
			if (isnan (point->y))
			{
				fprintf (stderr, "%s. NAN detected. Point [%d].y\n", region, n);
				break;
			}
		}
		else
		{
			fprintf (stderr, "%s. Point [%d] is null\n", region, n);
		}
    }	
}
*/

/* Init MPI slave */
void slaveInit(comm_info_slave* p, pvegetModels* v,int* dtime)
{
	int numslave;
	pvegetModels pv=NULL;
	int n;
	int myRank;
	MPI_Status status;
	
	MPI_Comm_rank (MPI_COMM_WORLD, &myRank);
	
	p->mytid=myRank;
	p->mastertid=0;
	InitWorkerLogFiles (p->mytid);
	
	Info (workerLog, "Waiting for init from master\n");
	MPI_Recv (NULL, 0, MPI_INT, p->mastertid, PACKET_INIT, MPI_COMM_WORLD,
		&status);
	Info (workerLog, "Init from master. \n");
	
	MPI_Send (NULL, 0, MPI_INT, p->mastertid, PACKET_INITOK, MPI_COMM_WORLD);

	/* master send parameters of vegetation */
	*v=(vegetModels *) malloc(sizeof(vegetModels));
	pv=*v;
	pv->submodels=NULL;
	pv->nSubmodels=0;

	MPI_Recv (mpi_pack_buf (),
		mpi_pack_buf_size (),
		MPI_PACKED,
		p->mastertid,
		PACKET_VEGT,
		MPI_COMM_WORLD,
		&status);
	mpi_unpack_init ();
	mpi_upkint (&pv->nSubmodels, 1, 1);
    pv->submodels=(vegetSubmodel*) malloc(pv->nSubmodels*sizeof(vegetSubmodel));
    for (n=0;n<pv->nSubmodels;n++)
    {
    	mpi_upkint (&pv->submodels[n].submodelCode, 1, 1);
    	mpi_upkdouble (pv->submodels[n].params, MAX_PARAMS, 1);
    }
    /* master send time increment in each iteration */
	mpi_upkint (dtime, 1, 1);
	
	printf ("Slave %d received input data\n", myRank);
}

void slaveEnd(comm_info_slave* p, pvegetModels* v)
{
	pvegetModels pv=NULL;
	pv=*v;
	
	if (pv!=NULL) {free(pv->submodels);free(pv);}
	
	mpi_pack_init ();
	mpi_pkint(&p->mytid,1,1);
	MPI_Send (NULL, 0, MPI_INT,
		p->mastertid, 
		PACKET_ENDTOK, 
		MPI_COMM_WORLD);
}

//MODIFICADA
void getArcInfo(pFIRELINE* f,int* arcinit,int* arcend,
	ppointsInfo* pInf, int * iterIdx, int * batchIdx) //incluimos el id del batch
{
	pFIRELINE pf=NULL;
	int n;
	ppointsInfo pts=NULL;
	
	/* Create one front */
	*f=(FIRELINE *) malloc(sizeof(FIRELINE));
	
	
	pf=*f;
	pf->points=NULL;
	pf->nPoints=0;
	
	/* unpack information */
	mpi_unpack_init	();
	
	mpi_upkint(batchIdx,1,1); // Incluimos el id del batch
	mpi_upkint(iterIdx,1,1);
	mpi_upkint(arcinit,1,1);
	mpi_upkint(arcend,1,1);
	
	
	pf->nPoints=(*arcend)-(*arcinit)+1;
	pf->points=(pointLine *) malloc((pf->nPoints+2)*sizeof(pointLine));
	
	mpi_upkdouble((double *)&(pf->points[1]),2*pf->nPoints,1);
	mpi_upkdouble((double *)&(pf->points[0]),2,1);
	mpi_upkdouble((double *)&(pf->points[pf->nPoints+1]),2,1);
	
	
	/* Info of interior points */
	*pInf=(pointsInfo *) malloc((pf->nPoints)*sizeof(pointsInfo));
	pts=*pInf;
	/* unpack parameters */
	for (n=0;n<pf->nPoints;n++)
	{
		mpi_upkint(&(pts[n].vegetmodel),1,1);
		mpi_upkdouble(&(pts[n].m),1,1);
		mpi_upkdouble(&(pts[n].mangle),1,1);
		mpi_upkdouble(&(pts[n].windspeed),1,1);
		mpi_upkdouble(&(pts[n].winddirection),1,1);
		
		assert (!isnan (pts[n].m));
		assert (!isnan (pts[n].mangle));
		assert (!isnan (pts[n].windspeed));
		assert (!isnan (pts[n].winddirection));
	}

}


/* get local parameters for a point */
void getLocalParams(pvegetModels* veget, int n, ppointsInfo *ptsInf,
		    pointLine* p, plocalParams plocal)
{
  pvegetModels pveget=NULL;
  ppointsInfo ppoints=NULL;
  double windangle;
  int vegetModel;

  pveget=*veget;
  ppoints=*ptsInf;

  vegetModel=abs(ppoints[n-1].vegetmodel);

  plocal->w0   =pveget->submodels[vegetModel].params[0];
  plocal->beta =pveget->submodels[vegetModel].params[1];
  plocal->sigma=pveget->submodels[vegetModel].params[2];
  plocal->St   =pveget->submodels[vegetModel].params[3];
  plocal->Se   =pveget->submodels[vegetModel].params[4];
  plocal->Mx   =pveget->submodels[vegetModel].params[5];
  plocal->Mf   =pveget->submodels[vegetModel].params[6];
  plocal->h    =pveget->submodels[vegetModel].params[7];
  plocal->rhos =pveget->submodels[vegetModel].params[8];

  /* Obtenir direccio del vent i velocitat */
  /* obtenir pendent en el punt */
  plocal->m=ppoints[n-1].m;
  assert (!isnan (plocal->m));
  assert (!isnan (ppoints[n-1].mangle));
  plocal->mx=plocal->m*cos(ppoints[n-1].mangle);
  assert (!isnan (plocal->mx));
  plocal->my=plocal->m*sin(ppoints[n-1].mangle);
  assert (!isnan (plocal->my));
  gis_normalvect(&(plocal->mx),&(plocal->my));

  plocal->U=ppoints[n-1].windspeed;
  windangle=(ppoints[n-1].winddirection*M_PI)/180.0;
  plocal->Ux=plocal->U*cos(windangle);
  plocal->Uy=plocal->U*sin(windangle);
  gis_normalvect(&(plocal->Ux),&(plocal->Uy));
}

//***************************************************************

void cargarmatrices(int f, int c)
{
     long int matriz1[f][c];
     long int matriz2[f][c];
     long int matrizresultado[f][c];

     int i,j,ir,jr,i1,numcte;
//   printf("cargando");
   for(i=0;i<f;i++)
   {
     for(j=0;j<c;j++)
     {
//        printf("%ld",numcte);
//        if(quematriz==1)
          matriz1[i][j]=numcte;
//        else
          matriz2[i][j]=numcte;
          numcte=numcte + 1;
//        printf("%d",matriz1[i][j]);
//        printf("%d",matriz2[i][j]);
     }
   }
   //MULTIPLICACION
   for(ir=0;ir<f;ir++)
     for(jr=0;jr<c;jr++)
       for(i1=0;i1<f;i1++)
         matrizresultado[ir][jr]+=matriz1[ir][i1]*matriz2[i1][jr];
   //FIN

}


//****************************************************************

//MODIFICADA
void arcStepKernel(int iterIdx, int batchIdx, pFIRELINE *f,pvegetModels *v,ppointsInfo *ptsInf,int dtime, int titerIdx, int tbatchIdx) //incluimos el id del batch necesario para el evento
{
  pvegetModels pveget=NULL;
  pFIRELINE pfront=NULL;
  ppointsInfo ppoints=NULL;

  plocalParams plocal=NULL;
  pvectRmax pRn0=NULL;
  int n;
  int vegetModel;
  double nx,ny;
  double modn,modn0; /* moduls dels vectors normals */
  double angle; /* angle entre n i n0 */
  double fn; /* Segon factor, influencia del angle */
  double Rn0; /* modul de la velocitat */

  double x_d, y_d, sum, tiempo_comienzo, tiempo_fin; // Andrea
  
  pFIRELINE pfront2=NULL;

  pveget=*v;
  pfront=*f;
  ppoints=*ptsInf;


  /* Crear front copia de l'actual */
  pfront2=(FIRELINE*) malloc(sizeof(FIRELINE));
  pfront2->points=NULL;
  pfront2->points= (pointLine*) malloc((pfront->nPoints+2)*sizeof(pointLine));
  pfront2->nPoints=pfront->nPoints;

  plocal=(plocalParams) malloc(sizeof(localParams));
  pRn0=(pvectRmax) malloc(sizeof(vectRmax));
  
  tiempo_comienzo = MPI_Wtime();	

  /* Per cada Punt interior del arc */
  sum = 0;
  for (n=1;n<=pfront->nPoints;n++)
    {
      vegetModel=ppoints[n-1].vegetmodel;

      /* Model valid de vegetacio que permet moviment */
      //cargarmatrices(20,20);
      
      if (vegetModel !=0)
	  {
	    /* Codi OK : Obtenir normal del arc de circunferencia */
	    gis_arcnormal(f,n,&nx,&ny);
	    /* Obtenir parametres locals del punt */
	    getLocalParams(v,n,ptsInf,&(pfront->points[n]),plocal);

	    /* Calcul dels factors */
	    factor1_calculation(plocal,nx,ny,pRn0); 
      
	    /* modul de la velocitat */
	    assert (pow(pRn0->Rmaxx,2.0)+pow(pRn0->Rmaxy,2.0) >= 0);
	    Rn0=sqrt(pow(pRn0->Rmaxx,2.0)+pow(pRn0->Rmaxy,2.0));

	    /* angle entre els vectors n(nx,ny) i n0 */
	    modn=sqrt(pow(nx,2.0)+pow(ny,2.0));
	    modn0=sqrt(pow(pRn0->n0x,2.0)+pow(pRn0->n0y,2.0));
	    assert (modn != 0);
	    assert (modn0 != 0);
	    angle=acos((nx*pRn0->n0x+ny*pRn0->n0y)/(modn*modn0));

	    /* Calcul de l'efecte de l'angle */

	    factor2_calculation(plocal,Rn0,angle,&fn);
        

	    pfront2->points[n].x=(pfront->points[n].x)+pRn0->Rmaxx*fn*dtime;
	    pfront2->points[n].y=(pfront->points[n].y)+pRn0->Rmaxy*fn*dtime;


        //x_d = pfront->points[n].x - pfront2->points[n].x;
        //y_d = pfront->points[n].y - pfront2->points[n].y;
        if(pfront->points[n].x > pfront->points[n].y) sum+=1;
        
        //sum += sqrt((x_d*x_d)+(y_d*y_d));
	  }
       /* else : no hi ha moviment del punt del front */
    }
    
    tiempo_fin = MPI_Wtime();	

    sum /= pfront->nPoints;
    

    
    printf ("Valor de sum %lf, tiempo computo %lf, numero de puntos %d \n", sum, tiempo_fin-tiempo_comienzo, pfront->nPoints);
  /* copy new front , only the interior points */ 
  for(n=1;n<=pfront->nPoints;n++)
    {
      pfront->points[n].x=pfront2->points[n].x;
      pfront->points[n].y=pfront2->points[n].y;
    }
  
  free(pfront2->points);
  free(pfront2);
  
  free(plocal);
  free(pRn0);
  
  sleep(sum*(tiempo_fin-tiempo_comienzo));
}

/* Task Output -> arc points */
void taskOutput(comm_info_slave* p,pFIRELINE *f,int arcinit,int arcend)
{
#if DEBUG	
  pFIRELINE pfront=NULL;
  int n;
  pfront=*f;

  fprintf(stderr, "Data of task %d counter %d \n",p->mytid, COUNTER++);
  fprintf(stderr,"----------------\n");
  fprintf(stderr,"Front with %d points. \n",pfront->nPoints);
  fprintf(stderr,"Arc points indexs -> [%d,%d]\n",arcinit,arcend);
  for (n=0;n<=(pfront->nPoints)+1;n++)
    {
      if (isnan (pfront->points[n].x) || isnan (pfront->points[n].y))
      {
      	fprintf(stderr,"Point %d -> (%f,%f)\n",n,
	    	 pfront->points[n].x,pfront->points[n].y);
	  }  	 
    }
  fprintf(stderr,"\n");
  fflush (stderr);
#endif  
}

/* reply to master with the arc result */
void resultstoMaster(comm_info_slave* p,pFIRELINE *f,int arcinit,int arcend, hrtime_t procesStartMs, int batchIdx)
{
	pFIRELINE pfront=NULL;
	hrtime_t procesEndMs;
	double diff;
	pfront=*f;
	
	mpi_pack_init ();
	mpi_pkint(&batchIdx,1,1);
	mpi_pkint(&arcinit,1,1);
	mpi_pkint(&arcend,1,1);
	

	mpi_pkdouble((double *)&(pfront->points[1]),2*pfront->nPoints,1);
	procesEndMs = GetCurrentTimeMS ();
	
	/* calc and send process time (tuning) */
	diff = double (procesEndMs-procesStartMs); // (double)(pfront->nPoints);
	assert (diff >= 0);
	//Info (workerLog, "process time %.10f ms, nPoints: %d\n", diff, pfront->nPoints);
	
	mpi_pkdouble (&diff, 1, 1);
	MPI_Send (
		mpi_pack_buf (),
		mpi_pack_buf_pos (),
		MPI_PACKED,
		p->mastertid, 
		PACKET_ARCR, 
		MPI_COMM_WORLD); 
}

//MODIFICADA
int simulationStep(comm_info_slave* p, pvegetModels* veget,int dtime)
{
	int actualbuf; /* Buffer actual */
	int nbytes,sendtid;
	int mtype; /* message tag */
	int finalStep;
	pFIRELINE pfront=NULL;
	int arcinit,arcend, iterIdx, batchIdx; //incluimos el batch del chunk que va a procesar
	pvegetModels pv=NULL;
	ppointsInfo ptsInf=NULL;
	hrtime_t t1, t2, t3, t4;
	MPI_Status st;

	finalStep=0;
	pv=*veget;
	Info (workerLog, "simulationStep\n");
	/* receive message and measure ide time */
	t1 = GetCurrentTimeMS ();
	
	//printf ("Slave %d waiting for requests...\n", p->mytid);
	MPI_Recv (mpi_pack_buf (),
    	mpi_pack_buf_size (),
    	MPI_PACKED,
    	p->mastertid,
    	MPI_ANY_TAG,
    	MPI_COMM_WORLD,
    	&st);
	
	t2 = GetCurrentTimeMS ();
	idleTime += (t2-t1);
	
	sendtid = st.MPI_SOURCE;
	mtype = st.MPI_TAG;
	Info (workerLog, "mpi_recv from %d \n", sendtid);
	t3 = GetCurrentTimeMS ();	
	/* What type of packet */
	switch(mtype)
    {
	case PACKET_ARCP:
		Info (workerLog, "PACKET_ARCP\n");
		/* Receive arc front data */
		getArcInfo(&pfront,&arcinit,&arcend,&ptsInf,&iterIdx,&batchIdx); // Incluimos el batch del chunk
		/* debug Output arc points */
		taskOutput(p,&pfront,arcinit,arcend);
		/* Calculation kernel */
		arcStepKernel(iterIdx, batchIdx, &pfront,&pv,&ptsInf,dtime, iterIdx, batchIdx); //Incluimos el id del batch para generar el evento
		/* result send to the master */
		resultstoMaster(p,&pfront,arcinit,arcend, t3, batchIdx);   
		Info (workerLog, "resultsToMaster\n");
		//printf ("Slave %d responded to master\n", p->mytid);
		break;
	
	case PACKET_IDLE: 
		printf ("Slave %d PACKET_IDLE\n", p->mytid);		
		break;
	  
	case PACKET_ENDT: 
		printf ("Slave %d finalizing...\n", p->mytid);		
		finalStep=1;
		break;
		
	default:
		printf ("Slave %d UNKNOWN\n", p->mytid);				
    }

  /* Deallocate temporal dinamic memory */
  if (pfront!=NULL) {free(pfront->points);free(pfront);pfront=NULL;}
  if (ptsInf!=NULL) {free(ptsInf);ptsInf=NULL;}

  t4 = GetCurrentTimeMS ();	
  calcTime += (t4-t3);

  return finalStep; /* last step of simulation */
}


// tuned main
int slaveMain(int, char**)
{
  char buf [256];
  comm_info_slave p;
  pvegetModels veget=NULL;
  int dtime;

  hrtime_t t1, t2, appTime;
  t1 = GetCurrentTimeMS ();
    	
  /* Slave Inicialization */
  slaveInit(&p,&veget,&dtime);
		
  while (simulationStep(&p,&veget,dtime)!=1);

  slaveEnd(&p,&veget);

  
  /** calculate time difference in milisecs and add difference to the accumulateTime */
  t2 = GetCurrentTimeMS ();
  appTime = t2-t1;
  Info (workerLog, "TOTAL TIMES: execution: %lld, work: %lld (%.2fp), idle: %lld (%.2fp)\n", 
			appTime, calcTime, Proc (calcTime, appTime), idleTime, Proc (idleTime, appTime));	
  CloseWorkerLogFiles ();  
  return 0;
}

