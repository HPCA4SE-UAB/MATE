/*
** ----------------------------------------------------------------------
**
** FILE:			Main File
**
**  (C-) Josep Jorba, 1997		C.A.O.S. - UAB
** 
** ----------------------------------------------------------------------
**
** Date			Who		What
** 23-11-97		JJ		Creation
**
** ----------------------------------------------------------------------
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "gisfuncs.h"
#include "vegetmod.h"
#include "windmod.h"
#include "simulator.h"
#include "main.h"
#include "globmod.h"
#include "fireslave.h"

#include "comm_utils.h"
#include "utils.h"
#include "tune.h"


#ifdef DEBUG
FILE *tasklog; /* Fitxer de log de les tasques */
#endif

// global
int myRank = -1;
/*static int    TheTotalWork = 0;
static int    TheWorkSizeUnitBytes = 0;
static int    TheIterationIndex = -1;
static int    NW = 0; //Variable a sintonizar*/


/*
void start_load_gen (int nTasks, int startParamIndex, char ** argv)
{
	int loadTids [128];
	char * loadArgs [10] = 
	{
			"0", // start generating load inmediately
			"0", // infinit generation (until master ends)
	};
	int i = 2; 
	while (argv [startParamIndex] != 0)
	{
		loadArgs [i++] = argv [startParamIndex++];
	}
	loadArgs [i++] = 0;
	
  // spawn slaves 
  // TO IMPLEMENT IN MPI
  int num=pvm_spawn("pvmloadgen",(char **)loadArgs, PvmTaskDefault,"",nTasks,loadTids);
  printf("Loadgen mode = %s, Num load generators started= %d\n", loadArgs [2], num);	  
}
*/

void save_results (char const * statsFile, char const * problemName, int nslaves, 
								   char const * loadFile, double appExec)
{
	 FILE * f = fopen (statsFile, "a");
	 if (f == 0)
	 {
	 	   perror ("result file could not be opened");
	 	   return;
	 }
	 fprintf (f, "xfire.mpi;%s;%d;%d;%s;%.4f\n", problemName, 
	 		nslaves, nslaves, loadFile, appExec);
	 fclose (f);
}								   



/*
** Proces de Setup dels models existents
*/
void models_setup(pMAP* map, pvegetModels* veget,
		  pFIRELINE* front, pwindPoints* wind, ptimeSim* stime,
		  char* landFileName, char* vegetFileName,
		  char* frontFileName, char* windFileName,char* timeFileName)
{
  gis_setup(map,front,landFileName,frontFileName);
  gis_statistics(map,front);
  veget_setup(veget,vegetFileName);
  veget_statistics(veget);
  wind_setup(wind,windFileName);
  wind_statistics(wind);
  simulator_setup(stime,timeFileName);
  simulator_statistics(stime);
}

/* Alliberar memoria associada als models */
void models_unsetup(pMAP* map, pvegetModels* veget,
		    pFIRELINE* front, pwindPoints* wind, ptimeSim* stime)
{
  gis_unsetup(map,front);
  veget_unsetup(veget);
  wind_unsetup(wind);
  simulator_unsetup(stime);
}


/* 
** takeParams: 
** Agafa els parametres de la linia de comandes, i/o teclat
** torna FALSE per parametres dolents, i TRUE parametres valids 
*/
TBOOL takeParams(int numParams, char** Params,
		 char* landname, char* vegetname,
		 char* frontname, char* windname, char* timename, 
		 char * stats_file,
		 int * useLoad
		 )
{
	if (numParams >= 2)
	{
		char buf [512];
		sprintf (buf, "data/land/%s.dat", Params [1]);
		strcpy(landname, buf);
		sprintf (buf, "data/vegetation/%s.dat", Params [1]);
		strcpy(vegetname, buf);
		sprintf (buf, "data/front/%s.dat", Params [1]);
		strcpy(frontname, buf);
	  	sprintf (buf, "data/wind/%s.dat", Params [1]);
	  	strcpy(windname, buf);
	  	sprintf (buf, "data/time/%s.dat", Params [1]);
	  	strcpy(timename, buf);	  	
	  	(numParams >= 3) ? strcpy (stats_file , Params [2]) : strcpy (stats_file, "");
	  	(numParams >= 4) ? *useLoad=5 : *useLoad = 0;
 		  return TTRUE;
	}
	// else wrong params
	// display help & exit
	fprintf (stderr, "Invalid syntax. Try: %s <problem_name> [<exec_stats_output>] [<scheduling>] [<load>]\n",
		Params [0]);
	fprintf (stderr, "where:\n"); 
	fprintf (stderr, " <problem_name>       - set of /data/*/<problem_name>.dat files\n");
	fprintf (stderr, " [<exec_stats_output>]- output file path for execution statistics\n");
	fprintf (stderr, " [<load_funtion [args]>] - filename with config for external load(optional)\n");
	return TFALSE;
}
		
/* Init master, sync slaves */
void masterInit(comm_info_master* p)
{
	int n;
	int numHosts;
	int num;
	MPI_Status status;
	
	/* check number of hosts */
	MPI_Comm_size (MPI_COMM_WORLD, &numHosts);
	if (numHosts < 2)
	{
		printf ("Application requires at least 2 processors\n");
		MPI_Abort (MPI_COMM_WORLD, -2);
		exit (-2);
	} 
	p->mytid=myRank;
	p->nslaves= numHosts -1;
	p->nutilslaves= numHosts -1;
	p->stids = (int *)malloc(p->nslaves*sizeof(int));
	
	printf("Num slaves= %d\n",p->nslaves);
	for (n=0;n<p->nslaves;n++)
	{
		p->stids [n] = n+1;
	} 
	/* Sincronization with slaves */
	for (n=0;n<p->nslaves;n++)
	{
		MPI_Send (NULL, 0, MPI_INT, p->stids[n], PACKET_INIT, MPI_COMM_WORLD);
	}	
	for (n=0;n<p->nslaves;n++)
	{
		MPI_Recv (NULL, 0, MPI_INT, p->stids[n], PACKET_INITOK, MPI_COMM_WORLD,
			&status);
	  	printf("Slave %d -> syncronized\n", p->stids[n]);
	}
}


/* End master, terminate slaves */
void masterEnd(comm_info_master* p)
{
	int numslave;
	int message=PACKET_ENDT;
	int n;
	MPI_Status status;

	/* Sincronization with slaves */
	for (n=0;n<p->nslaves;n++)
	{
		MPI_Send (NULL, 0, MPI_INT, p->stids[n], PACKET_ENDT, MPI_COMM_WORLD);
		//printf("Slave %d -> asked to finalize\n", p->stids[n]);
	}	
	for (n=0;n<p->nslaves;n++)
	{
		//printf("waiting for Slave %d to finalize...\n", p->stids[n]);
		MPI_Recv (NULL, 0, MPI_INT, p->stids[n], PACKET_ENDTOK, MPI_COMM_WORLD,
			&status);
	  	//printf("Slave %d -> finalized\n", p->stids[n]);
	}
	printf("Slaves finalized\n");
#ifdef DEBUG
  fclose(tasklog);
#endif
}

void send_params(comm_info_master *p,pvegetModels *v,ptimeSim* stime)
{
	int n,i;
	pvegetModels pv;
	ptimeSim ptime;
	int inctime;	
	// int numSubmodels
	// { x numSubmodels
	// 	  int sumbmodelCode;
	// 	  double params [MAX_PARAMS]
	// }
	// int inctime
		
	pv=*v;
  	ptime=*stime;

	mpi_pack_init ();
	mpi_pkint (&pv->nSubmodels, 1, 1);
	
  	for (i=0;i<pv->nSubmodels;i++)
	{
	  /* submodel code */
	  mpi_pkint (&(pv->submodels[i].submodelCode), 1, 1);
	  /* submodel params */
	  mpi_pkdouble (pv->submodels[i].params, MAX_PARAMS, 1);
	}
  	inctime=ptime->hinc*60+ptime->minc;
	mpi_pkint (&inctime, 1, 1);
  
 	/* Send vegetation models to slaves */
  	for (n=0;n<p->nslaves;n++)
    {
      	// data
      	MPI_Send (mpi_pack_buf (), 
      		mpi_pack_buf_pos (), 
      		MPI_PACKED, p->stids[n],PACKET_VEGT,
      		MPI_COMM_WORLD);
    }
}

int main(int argc,char** argv)
{
	int st;
	st = MPI_Init (&argc, &argv);
	if (st != MPI_SUCCESS)
	{
		printf ("cannot init mpi\n");
		MPI_Abort (MPI_COMM_WORLD, -1);
		return -1;	
	}  
	MPI_Comm_rank (MPI_COMM_WORLD, &myRank);
	if (myRank == 0) // master
	{
	  	fprintf (stdout, "Forrest Fire Propagation Simulator (MPI, v1.0), "\
	  		"MASTER task, rank %d\n", myRank);
		st = masterMain (argc, argv);
	}
	else
	{
	  	fprintf (stdout, "Forrest Fire Propagation Simulator  (MPI, v1.0), "\
	  		"SLAVE task, rank %d\n", myRank);
		st = slaveMain (argc, argv);
	}
	MPI_Finalize ();
	return st;
}

int masterMain(int argc,char** argv)
{
	pMAP         land=NULL;
	pvegetModels veget=NULL;
	pFIRELINE    front=NULL;
	pwindPoints  wind=NULL;   /* Estructures principals */
	ptimeSim     stime=NULL;
	char landFileName[256];
	char vegetFileName[256];
	char frontFileName[256];
	char windFileName[256];  
	char timeFileName[256]; /* Noms dels fitxers d'entrada */
	char statsFile[256]; /* Nombre de fichero de salida con datos estadisticos de ejecuccion*/
	int useLoad = 0;
	int nslaves;
	comm_info_master p;
	hrtime_t t1, t2, t3, t4, appExec;
	
	t1 = GetCurrentTimeMS ();
	
	if (takeParams(argc,argv,landFileName,vegetFileName, frontFileName,windFileName,timeFileName,
	 		statsFile, &useLoad)==TTRUE)
	{
		InitMasterLogFiles (myRank);
		printf ("Master task, argc=%d, ", argc);
		/* master init */
		masterInit(&p);
		
		/*
			FOR THE MOMENT WE DO NOT GENERATE EXTERNAL LOAD.
		if (useLoad)
		{
			 start_load_gen (p.nslaves == 1 ? 1 : (int)ceil((p.nslaves / 2.0)), useLoad, argv);
		}
		*/
		
		/* models initialization */
		models_setup(&land,&veget,&front,&wind,&stime,
		   landFileName,vegetFileName,frontFileName,
		   windFileName,timeFileName);
		
		/* Send vegetation models & increment time */
		send_params(&p,&veget,&stime);
		sleep (2);
		t2 = GetCurrentTimeMS ();
		simulator_loop(&p,&stime,&land,&veget,&front,&wind);
		t3 = GetCurrentTimeMS ();		
	
		models_unsetup(&land,&veget,&front,&wind,&stime);
		
		/* master end -> end of slaves */
		masterEnd(&p);
		
		// calculate time difference
		t4 = GetCurrentTimeMS ();
		appExec = t4-t1;
		
		if (statsFile != 0 && strlen (statsFile) > 0)
		{
			save_results (statsFile, argv [1], p.nslaves, useLoad == 0 ? "noload" : argv [useLoad], appExec); 
		}
		Info (masterLog, "TOTAL TIMES [ms]: AppExec: %lld, AppRun: %lld (%.2fp)", 
				appExec, t3-t2, Proc (t3-t2, appExec));
		
		CloseMasterLogFiles ();
		fprintf (stderr, "END\n");
		return 0;
	}
	else
	{
		printf ("Wrong params\n");
		return -1;		
	}
}
