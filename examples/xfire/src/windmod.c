/*
** ----------------------------------------------------------------------
**
** FILE:	WIND MODEL
**
**  (C-) Josep Jorba, 1997		C.A.O.S. - UAB
** 
** ----------------------------------------------------------------------
**
** Date			Who		What
** 24-01-98		JJ		Creation
**
** ----------------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>

#include "windmod.h"

/* Obtenir el vent en un determinat punt */
void wind_getWind(pwindPoints* w,pointLine* p,double *speed,double *direction)
{
  windStation *station;

  /* Agafar primera estacio */
  station=&((*w)->stations[0]);

  *speed=station->speed;
  *direction=station->direction;
}

/* Setup de les estructures del vent*/
void wind_setup(pwindPoints* w, char* windFile)
{
  if (*w != NULL) { free((*w)->stations); free(*w); }
  *w=(windPoints *) malloc(sizeof(windPoints));
  (*w)->nStations=0;
  (*w)->stations=NULL;

  wind_readWindFile(w,windFile);     /* Llegir fitxer de mesures de vent */
}

/* Alliberacio de les estructures */
void wind_unsetup(pwindPoints* w)
{
  if ((*w) != NULL)
  {
    if ((*w)->stations != NULL) free((*w)->stations);
    free(*w);
  }
}

/* Estadisticas del terreny i fronts actuals */
void wind_statistics(pwindPoints* w)
{
  pwindPoints pw;
  windStation *p;
  int i;

  pw=*w;
  fprintf(stderr, "Wind statistics:\n");
  fprintf(stderr, "%d windstations\n",pw->nStations);

  p=pw->stations;
  for (i=1;i<=pw->nStations;i++)
  {
    fprintf(stderr, "Station %d.\n",p->stationCode);
    fprintf(stderr, "Position at (%8.2f,%8.2f)\n",p->x,p->y);
    fprintf(stderr, "wind %f Km/h at %f degrees.\n",p->speed,p->direction);
    fprintf(stderr,"\n");
    p++;
  }
  fprintf(stderr,"\n");
}

/* Lectura d'un fitxer de definicio d'estacions de mesura de vents */
void wind_readWindFile(pwindPoints* w, char* name)
{
  FILE* fp;
  windStation* p=NULL;
  pwindPoints pw=NULL; /* Punter al front actual */
  int iStations=0;

  if ((fp=fopen(name,"r"))==NULL)
    { printf("Wind File error. exit...");exit(1);}
  else
    {
      pw=*w;
      /* Quants stacions tenim? */
      fscanf(fp,"%d",&(pw->nStations));
      pw->stations= (windStation*) malloc(pw->nStations*sizeof(windStation));
      /* Per facilitar la lectura del codi, pw == (*w) */
      p=pw->stations; /* Estacio actual */
      /* Per cada estacio */
      for (iStations=1;iStations<=pw->nStations;iStations++)
	{
	  fscanf(fp,"%d",&(p->stationCode));
	  fscanf(fp,"%lf %lf",&(p->x),&(p->y));
	  fscanf(fp,"%lf",&(p->direction));
	  fscanf(fp,"%lf",&(p->speed));

	  ++p;
	}
      fclose(fp);
    }
}
