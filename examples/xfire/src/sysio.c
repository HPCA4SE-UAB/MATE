/*
** ----------------------------------------------------------------------
**
** FILE:			A 
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
#include "gisfuncs.h"

#include "sysio.h"

void sysio_front(pFIRELINE* front, char* s)
{
  pFIRELINE pfront;
  FILE* fp;
  int n;

  pfront=*front;

  if ((fp=fopen(s,"w"))==NULL)
    {
      fprintf(stderr,"ERROR: Front Out not created\n");
      exit(-1);
    }
  else
    {
      for (n=0;n<pfront->nPoints;n++)
	fprintf(fp,"%f %f\n",(pfront->points[n].x),(pfront->points[n].y));
      if (pfront->tline==FL_CLOSEDCURVE)
	fprintf(fp,"%f %f\n",(pfront->points[0].x),(pfront->points[0].y));	
      fclose(fp);
    }
}
void sysio_fireArea(pMAP* map, char* s)
{
  pMAP pmap;
  FILE* fp;
  int i,j;

  pmap=*map;

  if ((fp=fopen(s,"w"))==NULL)
    {
      fprintf(stderr,"ERROR: Area Out not created\n");
      exit(-1);
    }
  else
    {
      for (i=0;i<pmap->xdim;i++)
	for (j=0;j<pmap->ydim;j++)
	  {
	    if (pmap->mapMatrix[i][j].model<0)
	      {
		fprintf(fp,"%f %f\n",(pmap->mapMatrix[i][j].x),
			(pmap->mapMatrix[i][j].y));
	      }
	  }
      fclose(fp);
    }
}
