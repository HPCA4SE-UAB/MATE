/*
** ----------------------------------------------------------------------
**
** FILE:	Vegetation Model
**
**  (C-) Josep Jorba, 1997		C.A.O.S. - UAB
** 
** ----------------------------------------------------------------------
**
** Date			Who		What
** 26-01-98		JJ		Creation
**
** ----------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

#include "vegetmod.h"

void veget_setup(pvegetModels* v, char* vegetFile)
{
  veget_unsetup(v); /* Alliberament estructura anterior */
  *v=(vegetModels *) malloc(sizeof(vegetModels));
  (*v)->nSubmodels=0;
  (*v)->submodels=NULL;

  veget_readVegetFile(v,vegetFile);     /* Llegir fitxer de mesures de vent */
}

void veget_unsetup(pvegetModels* v)
{
  if ((*v) != NULL)
  {
    if ((*v)->submodels != NULL) free((*v)->submodels);
    free(*v);
  }
}

void veget_statistics(pvegetModels* v)
{
  pvegetModels pv;
  vegetSubmodel *p;
  int i,j;

  pv=*v;
  fprintf(stderr, "Veget statistics:\n");
  fprintf(stderr, "%d submodels\n",pv->nSubmodels);

  p=pv->submodels;
  for (i=1;i<=pv->nSubmodels;i++)
  {
    fprintf(stderr, "Model %d.\n",p->submodelCode);
    fprintf(stderr, "Params: ");
    for (j=0;j<MAX_PARAMS;j++)
      fprintf(stderr, "%f ,",p->params[j]);
    fprintf(stderr,"\n");
    p++;
  }
  fprintf(stderr,"\n");
}

void veget_readVegetFile(pvegetModels* v, char* name)
{
  FILE* fp;
  vegetSubmodel* p=NULL;
  pvegetModels pv=NULL; /* Punter al model actual */
  int iModels=0,j;

  if ((fp=fopen(name,"r"))==NULL)
    { printf("Vegetation File error. exit...");exit(1);}
  else
    {
      pv=*v;
      /* Quants submodels tenim? */
      fscanf(fp,"%d",&(pv->nSubmodels));
      pv->submodels=(vegetSubmodel*) malloc(pv->nSubmodels*sizeof(vegetSubmodel));
      /* Per facilitar la lectura del codi, pv == (*v) */
      p=pv->submodels; /* Submodel actual */
      /* Per cada submodel */
      for (iModels=1;iModels<=pv->nSubmodels;iModels++)
	{
	  fscanf(fp,"%d",&(p->submodelCode));
	  for (j=0;j<MAX_PARAMS;j++)
	    fscanf(fp,"%lf ",&(p->params[j]));
	  ++p;
	}
      fclose(fp);
    }
}


