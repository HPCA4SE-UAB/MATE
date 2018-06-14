/*
** ----------------------------------------------------------------------
**
** FILE:	GLOBAL MODEL
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
#include <math.h>
#include <assert.h>
//SOLO PARA SOLARIS
//#include <ieeefp.h>

#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>
#include "gisfuncs.h"
#include "numeric.h"
#include "vegetmod.h"
#include "windmod.h"
#include "globmod.h"

#include "globalvar.h"

// tuning
#include "tune.h"

//FILE * ff = 0;

float F0 = 0.5;
float F1 = 0.5;
float F2 = 0.5;
float Factor_0 = 0.5;
float Factor_1 = 0.5;
float Factor_2 = 0.5;
/*static int    TheTotalWork = 0;
static int    TheWorkSizeUnitBytes = 0;
static int    TheIterationIndex = -1;
static int    NW = 0; //Variable a sintonizar*/
int    TheTotalWork = 0;
int    TheWorkSizeUnitBytes = 0;
int    TheIterationIndex = -1;
int    NW = 0; //Variable a sintonizar

/*
extern int TheTotalWork;
extern int TheWorkSizeUnitBytes;
extern int TheIterationIndex;
extern int NW; //Variable a sintonizar */

//temporal for adding machine only once
int done = 0;
	
void check_firelineEx (char const * region, pFIRELINE front, pMAP map)
{
	check_fireline (region, front);
	/*
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
			if (point->x < map->xmin)
			{
				fprintf (stderr, "%s. Point [%d].x=%f < xmin=%f\n", region, n, point->x, map->xmin);
				break;				
			}
			if (point->y < map->ymin)
			{
				fprintf (stderr, "%s. Point [%d].y=%f < ymin=%f\n", region, n, point->y, map->ymin);
				break;								
			}
			if (point->x > map->xmax)
			{
				fprintf (stderr, "%s. Point [%d].x=%f > xmax=%f\n", region, n, point->x, map->xmax);
				break;								
			}
			if (point->y > map->ymax)
			{
				fprintf (stderr, "%s. Point [%d].x=%f > ymax=%f\n", region, n, point->y, map->ymax);
				break;
			}		
		}
		else
		{
			fprintf (stderr, "%s. Point [%d] is null\n", region, n);
		}
    }	
    */
}

void check_fireline (char const * region, pFIRELINE front)
{
	int n;
	pointLine * point;
	
	assert (front != 0);

//        printf("CHECKFIRELINE 1 \n");
	for (n=0; n<front->nPoints; ++n)
	{
		point = &front->points [n];
//                printf("CHECKFIRELINE 2 \n");
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
//        printf("CHECKFIRELINE 3 \n");
}

/* Determinacio de l'area cremada */
void global_fireArea(pMAP*map, pFIRELINE* front)
{
  pMAP pmap;
  pFIRELINE pfront;
  int n;
  int i1,i2,j1;
  double x1,y1,x2,y2; /* segment de front */
  int i,j;
  double temp;
  
  double fxmin,fxmax,fymin,fymax,fx,fy;
  pointLine p,pnext;
  double yline;

  double xcuts[256]; /* Array per guardar els talls de la linia */
  int ncuts;
  double xini,xend;
  double m,b;
  double xnewcut;
  int is_xcut;
  int numsegments; /*number of segments to examine */
  int actmodel;

 /* Agafar punters als diferents objectes */
  pmap=*map;
  pfront=*front;
  
  /* Determinar rectangle que compte el front */
  fxmin=pmap->xmax;
  fxmax=pmap->xmin;
  fymin=pmap->ymax;
  fymax=pmap->ymin;

  for (n=0;n<pfront->nPoints;n++)
    {
      fx=pfront->points[n].x;
      fy=pfront->points[n].y;
      if (fx>fxmax) fxmax=fx;
      if (fx<fxmin) fxmin=fx;
      if (fy>fymax) fymax=fy;
      if (fy<fymin) fymin=fy;
    }

  /* Ajustar coodenades a la reixa del mapa */
  p.x=fxmin;p.y=fymin;
  gis_roundPointSO(map,p,&pnext);
  fxmin=pnext.x;fymin=pnext.y;
  p.x=fxmax;p.y=fymax;
  gis_roundPointNE(map,p,&pnext);
  fxmax=pnext.x;fymax=pnext.y;

  if (pfront->tline==FL_CLOSEDCURVE) numsegments=pfront->nPoints;
  else numsegments=pfront->nPoints-1;
  /* Determinar si el punt pertany a una area cremada */
  for (yline=fymax;yline>=fymin;yline=yline-pmap->ydelta)
    {
      ncuts=0;
      for (n=0;n<numsegments;n++)
	{
	  /* coordinates of the segment */
	  if (n!=(pfront->nPoints-1))
	    {
	      /* segment de front */
	      x1=pfront->points[n].x;
	      y1=pfront->points[n].y;
	      x2=pfront->points[n+1].x;
	      y2=pfront->points[n+1].y;
	    }
	  else   /* last segment of a closed curve */
	    {
	      x1=pfront->points[n].x;
	      y1=pfront->points[n].y;
	      x2=pfront->points[0].x;
	      y2=pfront->points[0].y;
	    }

	  /* intersecta ? */
	  if (((yline<y2) && (yline>y1)) || ((yline>y2) && (yline<y1))
	      || ((yline==y1)||(yline==y2)))
	    {
	      /* Es la aresta horitzontal ? */
	      if (y2-y1!=0.0)
		{
		  /* CHANGE: Vigilar pendent infinita */
		  if ((x2-x1)!=0.0)
		    {
		      assert (x2 != x1);		
		      m=(y2-y1)/(x2-x1);
		      b=y1-m*x1;
		      assert (m != 0);
		      xnewcut=(yline-b)/m;
		    }
		  else xnewcut=x1;

		  is_xcut=0; /* this x is a previous cut? */

		  for (i=0;i<ncuts;i++)
		    {
		      if (xcuts[i]==xnewcut) is_xcut=1;
		    }

		  /* is a new cut? */
		  if (is_xcut==0)
		    {
		      xcuts[ncuts]=xnewcut;
		      ncuts++;
		    }
		}
	    }
	}

      /* simple bublesort for x cut ordering */
      for (i=1;i<ncuts;i++)
	{
	  for (j=ncuts-1;j>=i;j--)
	    {
	      if (xcuts[j-1]>xcuts[j])
		{
		  temp=xcuts[j-1];
		  xcuts[j-1]=xcuts[j];
		  xcuts[j]=temp;
		}
	    }
	}
	assert (pmap->xdelta);
	assert (pmap->ydelta);
	       
      if (ncuts==-1) /* cut in a one point*/
	{
	  xini=xcuts[0];

	  /* Coordinate adjust of raster line */

	  
	  temp=(xini - pmap->xmin) / pmap->xdelta;
	  if((temp-floor(temp))>0) i1=(int)floor(temp)+1;
	  else i1=(int)temp; 
	  
	  j1= (int)floor((yline - pmap->ymin) / pmap->ydelta);
	
	  actmodel=pmap->mapMatrix[i1][j1].model;
	  pmap->mapMatrix[i1][j1].model=-1*abs(actmodel); 
	}

      /* Paint the burned zone */
      for (i=0;i<ncuts-1;i=i+2)
	{
	  xini=xcuts[i];
	  xend=xcuts[i+1];

	  /* Coordinate adjust of raster line */
	  /* i1,i2 x segment to paint */
	  temp=(xini - pmap->xmin) / pmap->xdelta;
	  if((temp-floor(temp))>0) i1=(int)floor(temp)+1;
	  else i1=(int)temp;
	 
	  /* the end point burned */
	  temp= (int)(xend - pmap->xmin) / pmap->xdelta;
	  i2=(int)floor(temp);
	  
	  
	  j1= (int)floor((yline - pmap->ymin) / pmap->ydelta);
	  
	  for (j=i1+1;j<=i2-1;j++)
	    {
	      actmodel=pmap->mapMatrix[j][j1].model;
	      pmap->mapMatrix[j][j1].model=-1*abs(actmodel); 
	      /* Burned zone : -model */
	    }
	}
    }
}

/* Test de les hipotesis de quasi equilibri que s'han de cumplir */
TBOOL global_testeq(pMAP* map, pvegetModels* veget, pFIRELINE* front)
{
  pMAP pmap;
  pvegetModels pveget;
  pFIRELINE pfront;
  int actnPoints;
  int n;
  TBOOL isTest;
  double dist;

  isTest=TTRUE; /* Test equilibri correcte */

 /* Agafar punters als diferents objectes */
  pmap=*map;
  pveget=*veget;
  pfront=*front;

  actnPoints=pfront->nPoints;

  /* Avaluacio de la maxima distancia entre punts, per avaluar si es 
   trenquen les condicions d'equilibri */
  n=0;
  dist=0.0;
  while ((dist<=MAXDIST_TEST) &&(n<(actnPoints-1)))
    {
      dist=sqrt(pow((pfront->points[n].x)-(pfront->points[n+1].x),2)+
	pow((pfront->points[n].y)-(pfront->points[n+1].y),2));
      n++;
    }
  
  if (n!=(actnPoints-1)) isTest=TFALSE;

  return isTest;
}

/* Test del front, esta al territori? */
TBOOL global_testFrontInLand(pMAP* map, pFIRELINE* front)
{
  pMAP pmap;
  pFIRELINE pfront;
  TBOOL result;
  double x,y;
  int n;

  pmap=*map;
  pfront=*front;
  
  result=TTRUE;
  for (n=0;n<pfront->nPoints;n++)
    {
      x=pfront->points[n].x;
      y=pfront->points[n].y;

      if((x<pmap->xmin) || (x>pmap->xmax)) {result=TFALSE;break;}
      if((y<pmap->ymin) || (y>pmap->ymax)) {result=TFALSE;break;}
    }
  return result;
}

/* 
   Interpolacio (per splines) de nous punts del front
   Se li passa el territori per evitar que s'interpolin punts fora del terreny.
 */
void global_newpoints(pMAP* map, pFIRELINE* front)
{
  pMAP pmap;
  pFIRELINE pfront;

  int actnPoints,newnPoints;
  double dist,totaldist,maxdist;
  int n,i;
  int points; /* numero de punts nous per interval */
  
  int nact; /* nombre de punts actuals del front */
  int nnew; /* nombre de punts nous a calcular */
  int innew; /* index dels punts nous */
  int inact; /* index */
  /* 
     tact parametre parametric
     xact,yact (x,y) actual
     tnew parametre dels nous punts
     xnew,ynew (x,y) dels nous punts
  */
  double *tact,*xact,*yact,*tnew,*xnew,*ynew;
  double tini,tend,tinc,ti;
  double testdist;
   
  pmap=*map;
  pfront=*front;

  /* Punts actuals del front */
  actnPoints=pfront->nPoints;

  /* Si es tancada la corba, hi ha un punt mes */
  if (pfront->tline==FL_CLOSEDCURVE) nact=actnPoints+1; 
  else nact=actnPoints;
  
  tact=(double *) malloc(nact*sizeof(double));
  xact=(double *) malloc(nact*sizeof(double));
  yact=(double *) malloc(nact*sizeof(double));

  /* Calcul maxima distancia entre dos punts, i calcul aproximat de la
   distancia total entre el inici i final del front */
  totaldist=0.0;
  maxdist=0.0;

  tact[0]=0.0;
  xact[0]=pfront->points[0].x;
  yact[0]=pfront->points[0].y;

  for (n=0;n<actnPoints-1;n++)
    {
      dist=sqrt(pow((pfront->points[n].x)-(pfront->points[n+1].x),2)+
		pow((pfront->points[n].y)-(pfront->points[n+1].y),2));
      if (dist>maxdist) maxdist=dist;
      totaldist=totaldist+dist;
      tact[n+1]=totaldist; /* ara guardem distancia despres calculem t */
      xact[n+1]=pfront->points[n+1].x;
      yact[n+1]=pfront->points[n+1].y;
    }

  /* Si el front es una corba tancada cal calcular la distancia del ultim
   punt al primer */
  if (pfront->tline==FL_CLOSEDCURVE)
    {
      dist=sqrt(pow((pfront->points[0].x)-(pfront->points[actnPoints-1].x),2)+
		pow((pfront->points[0].y)-(pfront->points[actnPoints-1].y),2));
      if (dist>maxdist) maxdist=dist;
      totaldist=totaldist+dist;
      tact[actnPoints]=1.0; /* 1.0 el maxim de t */
      xact[actnPoints]=pfront->points[0].x; /* mateixes x,y punt inicial */
      yact[actnPoints]=pfront->points[0].y;
    }

  /* calculem ara el parametre t pels punts que tenim */
  for (n=1;n<actnPoints;n++) tact[n]=tact[n]/totaldist;

  /* Punts nous per cada interval */
  testdist=(double)MAXDIST_TEST;
  points = (int)floor(maxdist/testdist);
  
  nnew=(nact-1)*points;

  tnew=(double *) malloc(nnew*sizeof(double));
  xnew=(double *) malloc(nnew*sizeof(double));
  ynew=(double *) malloc(nnew*sizeof(double));

  innew=0;
  for(n=0;n<(nact-1);n++)
    {
      tini=tact[n];
      tend=tact[n+1];
      tinc=(tend-tini)/(double)(points+1);
      ti=tini;
      for (i=0;i<points;i++)
	{
	  tnew[innew++]=ti+tinc;
	  ti=ti+tinc;
	}
    }

  /* Interpolacio de x(t),y(t) */
  spline(tact,xact,nact,tnew,xnew,nnew);
  spline(tact,yact,nact,tnew,ynew,nnew);
  
  /* Fusio dels punts per formar el nou front */
  newnPoints=actnPoints+nnew;
  free(pfront->points);
  pfront->points=(pointLine *) malloc(newnPoints*sizeof(pointLine));

  innew=0;
  inact=0;
  for (n=0;n<actnPoints;n++)
    {
      pfront->points[innew].x=xact[n];
      pfront->points[innew].y=yact[n];
      innew++;
      for (i=0;i<points;i++)
	{
	  pfront->points[innew].x=xnew[inact];
	  pfront->points[innew].y=ynew[inact];
	  innew++;
	  inact++;
	}
    }

  pfront->nPoints=newnPoints;
  
  free(tact);
  free(xact);
  free(yact);

  free(tnew);
  free(xnew);
  free(ynew);
  
}

/* 
   Interpolacio (per splines) de nous punts del front
   Se li passa el territori per evitar que s'interpolin punts fora del terreny.
 */
void global_newpoints2(pMAP* map, pFIRELINE* front)
{
  pMAP pmap;
  pFIRELINE pfront;

  int actnPoints,newnPoints;
  double dist,totaldist,maxdist;
  int n,i;
  int *npoints; /* number of new points for each interval */
  int nact; /* nombre de punts actuals del front */
  int nnew; /* nombre de punts nous a calcular */
  int innew; /* index dels punts nous */
  int inact; /* index */
  /* 
     tact parametre parametric
     xact,yact (x,y) actual
     tnew parametre dels nous punts
     xnew,ynew (x,y) dels nous punts
  */
  double *tact,*xact,*yact,*tnew,*xnew,*ynew;
  double tini,tend,tinc,ti;
  double testdist;
   
  pmap=*map;
  pfront=*front;

  /* Punts actuals del front */
  actnPoints=pfront->nPoints;

  /* Si es tancada la corba, hi ha un punt mes */
  if (pfront->tline==FL_CLOSEDCURVE) nact=actnPoints+1; 
  else nact=actnPoints;
  
  tact=(double *) malloc(nact*sizeof(double));
  xact=(double *) malloc(nact*sizeof(double));
  yact=(double *) malloc(nact*sizeof(double));
  npoints=(int *) malloc(nact*sizeof(double));

  /* Calcul maxima distancia entre dos punts, i calcul aproximat de la
   distancia total entre el inici i final del front */
  totaldist=0.0;
  maxdist=0.0;

  tact[0]=0.0;
  xact[0]=pfront->points[0].x;
  yact[0]=pfront->points[0].y;

  /* Test distance */
  testdist=(double)MAXDIST_TEST;
  nnew=0; /* number of new points */

  for (n=0;n<actnPoints-1;n++)
    {
      dist=sqrt(pow((pfront->points[n].x)-(pfront->points[n+1].x),2)+
		pow((pfront->points[n].y)-(pfront->points[n+1].y),2));
      if (dist>maxdist) maxdist=dist;
      totaldist=totaldist+dist;
      tact[n+1]=totaldist; /* ara guardem distancia despres calculem t */
      xact[n+1]=pfront->points[n+1].x;
      yact[n+1]=pfront->points[n+1].y;
      /* number of points to be interpolated  in this interval */
      npoints[n]=(int)floor(dist/testdist);
      nnew=nnew+npoints[n];
    }

  /* Si el front es una corba tancada cal calcular la distancia del ultim
   punt al primer */
  if (pfront->tline==FL_CLOSEDCURVE)
    {
      dist=sqrt(pow((pfront->points[0].x)-(pfront->points[actnPoints-1].x),2)+
		pow((pfront->points[0].y)-(pfront->points[actnPoints-1].y),2));
      if (dist>maxdist) maxdist=dist;
      totaldist=totaldist+dist;
      tact[actnPoints]=1.0; /* 1.0 el maxim de t */
      xact[actnPoints]=pfront->points[0].x; /* mateixes x,y punt inicial */
      yact[actnPoints]=pfront->points[0].y;
      npoints[actnPoints-1]=(int)floor(dist/testdist);
      nnew=nnew+npoints[actnPoints-1];
    }

  /* calculem ara el parametre t pels punts que tenim */
  for (n=1;n<actnPoints;n++) tact[n]=tact[n]/totaldist;

  tnew=(double *) malloc(nnew*sizeof(double));
  xnew=(double *) malloc(nnew*sizeof(double));
  ynew=(double *) malloc(nnew*sizeof(double));

  innew=0;
  for(n=0;n<(nact-1);n++)
    {
      tini=tact[n];
      tend=tact[n+1];
      tinc=(tend-tini)/(double)(npoints[n]+1);
      ti=tini;
      for (i=0;i<npoints[n];i++)
	{
	  tnew[innew++]=ti+tinc;
	  ti=ti+tinc;
	}
    }

  /* Interpolacio de x(t),y(t) */
  spline(tact,xact,nact,tnew,xnew,nnew);
  spline(tact,yact,nact,tnew,ynew,nnew);
  
  /* Fusio dels punts per formar el nou front */
  newnPoints=actnPoints+nnew;
  free(pfront->points);
  pfront->points=(pointLine *) malloc(newnPoints*sizeof(pointLine));

  innew=0;
  inact=0;
  for (n=0;n<actnPoints;n++)
    {
      pfront->points[innew].x=xact[n];
      pfront->points[innew].y=yact[n];
      innew++;
      for (i=0;i<npoints[n];i++)
	{
	  pfront->points[innew].x=xnew[inact];
	  pfront->points[innew].y=ynew[inact];
	  innew++;
	  inact++;
	}
    }

  pfront->nPoints=newnPoints;
  
  free(tact);
  free(xact);
  free(yact);

  free(tnew);
  free(xnew);
  free(ynew);
  
}


/* Obtenir els parametres local pel punt determinat del front */
void global_getLocalParams( pMAP* map, pvegetModels* veget, pwindPoints* wind,
			    pointLine* p, plocalParams plocal)
{
  pMAP pmap;
  pvegetModels pveget;
  pwindPoints pwind;

  int vegetModel; /* Numero de model de vegetacio */
  double windspeed,winddirection, windangle;
  double m,mangle;

 /* Agafar punters als diferents objectes */
  pmap=*map;
  pveget=*veget;
  pwind=*wind;

  /* Numero de model de vegetacio del punt */
  vegetModel=abs(gis_getVegetModel(map,p));

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
  gis_getOrientation(map,p,&m,&mangle);
  plocal->m=m;
  plocal->mx=m*cos(mangle);
  plocal->my=m*sin(mangle);
  gis_normalvect(&(plocal->mx),&(plocal->my));
      
  wind_getWind(wind,p,&windspeed,&winddirection);
  plocal->U=windspeed;
  windangle=(winddirection*M_PI)/180.0;
  plocal->Ux=windspeed*cos(windangle);
  plocal->Uy=windspeed*sin(windangle);
  gis_normalvect(&(plocal->Ux),&(plocal->Uy));
}


/* kernel del model global */
void global_kernel( pMAP* map, pvegetModels* veget,
		    pFIRELINE* front, pwindPoints* wind,int acttime,int dtime)
{
  pMAP pmap;
  pvegetModels pveget;
  pFIRELINE pfront;
  pwindPoints pwind;

  plocalParams plocal;
  pvectRmax pRn0;
  pFIRELINE pfront2;

  int n;
  int vegetModel;
  double nx,ny; /* Guarden la normal del punt */
  double xnew,ynew;
  double xold,yold;
  double angle; /* angle entre n i n0 */
  double fn; /* Segon factor, influencia del angle */
  double Rn0; /* modul de la velocitat */
  double modn,modn0; /* moduls dels vectors normals */

 /* Agafar punters als diferents objectes */
  pmap=*map;
  pveget=*veget;
  pfront=*front;
  pwind=*wind;

  /* Crear front copia de l'actual */
  pfront2=(FIRELINE*) malloc(sizeof(FIRELINE));
  pfront2->points= (pointLine*) malloc(pfront->nPoints*sizeof(pointLine));
  pfront2->nPoints=pfront->nPoints;

  /* 
  ** Basicament:
  ** a) Proces de disgregacio del front.
  ** b) Avaluacio dels punts i calcul de la normal.
  ** c) Crida al calcul de factors (model local pels punts del front).
  ** d) Traslacio de punts : moviment del front.
  */

  plocal=(plocalParams) malloc(sizeof(localParams));
  pRn0=(pvectRmax) malloc(sizeof(vectRmax));

  check_firelineEx ("global_kernel", pfront, pmap);
  /* Per cada Punt */
  for (n=0;n<pfront->nPoints;n++)
    {
      vegetModel=gis_getVegetModel(map,&(pfront->points[n]));

      /* Model valid de vegetacio que permet moviment */
      if (vegetModel !=0)
	  {
	    /* Codi OK : Obtenir normal del arc de circunferencia */
	    gis_arcnormal(front,n,&nx,&ny);
	    /* Obtenir parametres locals del punt */
	    global_getLocalParams(map,veget,wind,&(pfront->points[n]),plocal);

	    /* Calcul dels factors */
	    factor1_calculation(plocal,nx,ny,pRn0); 
      
	    /* modul de la velocitat */
	    Rn0=sqrt(pow(pRn0->Rmaxx,2.0)+pow(pRn0->Rmaxy,2.0));

	    /* angle entre els vectors n(nx,ny) i n0 */
	    modn=sqrt(pow(nx,2.0)+pow(ny,2.0));
	    modn0=sqrt(pow(pRn0->n0x,2.0)+pow(pRn0->n0y,2.0));
	    angle=acos((nx*pRn0->n0x+ny*pRn0->n0y)/(modn*modn0));

	    /* Calcul de l'efecte de l'angle */

	    factor2_calculation(plocal,Rn0,angle,&fn);
      
	    /* fn=1.0; */
	    /* nou punt calculat */
	    xold=pfront->points[n].x;
	    yold=pfront->points[n].y;

	    xnew=(pfront->points[n].x)+pRn0->Rmaxx*fn*dtime;
	    ynew=(pfront->points[n].y)+pRn0->Rmaxy*fn*dtime;

	    pfront2->points[n].x=xnew;
	    pfront2->points[n].y=ynew;

	    /* if the target area is burned, not move to new position */
	    if (gis_getVegetModel(map,&(pfront2->points[n]))<0)
	      {
		pfront2->points[n].x=xold;
		pfront2->points[n].y=yold;
	      }
	  }
       /* else : no hi ha moviment del punt del front */
    }

  /* copy new front */ 
  for(n=0;n<pfront->nPoints;n++)
    {
      pfront->points[n].x=pfront2->points[n].x;
      pfront->points[n].y=pfront2->points[n].y;
    }

  free(pfront2->points);
  free(pfront2);

  free(plocal);
  free(pRn0);
}

//MODIFICADA
void global_sendwork (int batchIdx, int iterIdx, int tid, int encoding, int tuple, int workIdx, pMAP pmap, 
	pvegetModels pveget, pFIRELINE pfront, pwindPoints pwind)
{
	int i, vegetmodel;
  	double windspeed,winddirection;
  	double m,mangle;	
  	
	int nTotalPoints = pfront->nPoints;
	int nPointsLeft = nTotalPoints - workIdx;
	assert (nPointsLeft >= tuple);
	
	int arcInit = workIdx;
	int arcEnd = arcInit + tuple - 1; // inclusive [arcInit, arcEnd]
	// prev and next points indices
	int pointPrev = arcInit==0 ? nTotalPoints-1 : arcInit - 1;
	int pointNext = arcEnd == nTotalPoints-1 ? 0 : arcEnd + 1;
	
	//fprintf (stderr, "Sending arc (%d=%d) to worker %d\n", arcInit, arcEnd, tid);
		
	mpi_pack_init ();
	mpi_pkint (&batchIdx,1,1); // Incluimos el identificador del batch al que pertenece el chunk q va a procesar el worker
	mpi_pkint (&iterIdx,1,1);
    mpi_pkint (&arcInit,1,1);
    mpi_pkint (&arcEnd,1,1);
    /* pack the arc */
    mpi_pkdouble ((double *)&(pfront->points[arcInit]), 2*(arcEnd-arcInit+1),1);
    /* pack the previous & next points */
    mpi_pkdouble ((double *)&(pfront->points[pointPrev]),2,1);
    mpi_pkdouble ((double *)&(pfront->points[pointNext]),2,1);
    
    /* pack the veget model numbers, orientation & wind of points */
    /* only for interior points, not boundary points */
    for (i=arcInit;i<=arcEnd;++i)
	{
      //fprintf(ff,"gis_getVegetModel i=%d, px=%f, py=%f\n",i, pfront->points[i].x, pfront->points[i].y);
	  vegetmodel = gis_getVegetModel (&pmap,&(pfront->points[i]));
	  mpi_pkint (&vegetmodel,1,1);
	  gis_getOrientation (&pmap,&(pfront->points[i]),&m,&mangle);
	  mpi_pkdouble (&m,1,1);
	  mpi_pkdouble (&mangle,1,1);
	  wind_getWind (&pwind,&(pfront->points[i]),&windspeed,&winddirection);
	  mpi_pkdouble (&windspeed,1,1);
	  mpi_pkdouble (&winddirection,1,1);
	}
    /* Send info to slave */
    MPI_Send (mpi_pack_buf (),
    	mpi_pack_buf_pos (),
    	MPI_PACKED,
    	tid, 
    	PACKET_ARCP, 
    	MPI_COMM_WORLD);
}   
      
pFIRELINE global_copyfront (pFIRELINE pfront)
{
  /* copy of front */
  int n;
  pFIRELINE pfront2;
  
  pfront2 =(FIRELINE*) malloc(sizeof(FIRELINE));
  pfront2->points= (pointLine*) malloc(pfront->nPoints*sizeof(pointLine));
  pfront2->nPoints=pfront->nPoints;
  pfront2->tline=pfront->tline;
  for(n=0;n<pfront->nPoints;n++)
  {
      pfront2->points[n].x=pfront->points[n].x;
      pfront2->points[n].y=pfront->points[n].y;
  }
  return pfront2;
}
        

void global_receivework (int * fromTid, pFIRELINE pfront, double * timeMs, int * nPoints, int * batchIdx) // es posible que tenga que adaptarla para el evento en tunlet número de worker
{   
	MPI_Status st;
	int arcInit, arcEnd;
	double workerCalcTime;
	MPI_Recv (mpi_pack_buf (),
    	mpi_pack_buf_size (),
    	MPI_PACKED,
    	MPI_ANY_SOURCE,
    	PACKET_ARCR,
    	MPI_COMM_WORLD,
    	&st);
    	
	//fprintf (stderr, "*** work received from %d \n", *fromTid);
    mpi_unpack_init ();
    mpi_upkint(batchIdx,1,1);
    mpi_upkint (&arcInit,1,1);
    mpi_upkint (&arcEnd,1,1);
    *nPoints = (arcEnd-arcInit)+1;
    mpi_upkdouble((double *)&((pfront)->points[arcInit]), 2*(arcEnd-arcInit+1),1);
	mpi_upkdouble (timeMs, 1, 1);
	*fromTid = st.MPI_SOURCE;
	//fprintf (stderr, "*** Received arc (%d=%d) from worker %d - %f\n", arcInit, arcEnd, *fromTid, workerCalcTime);
	fflush (stderr);	
		
}
     
void send_paramsToSlave(int slaveTid, 
	comm_info_master *p,
	pvegetModels *v, 
	ptimeSim *stime)
{
	int n,i;
	pvegetModels pv;
	ptimeSim ptime;
	int inctime;
	
	pv=*v;
	ptime=*stime;
	
	/* Send vegetation models to slaves */
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
		slaveTid, 
		PACKET_VEGT, 
		MPI_COMM_WORLD); 
}

/* Dynamically tuned work load distribution */
//MODIFICADA
void global_sendreceive (int iterIdx,
						 comm_info_master* p, 
						 pMAP pmap, 
			       		 pvegetModels pveget,
			       		 pFIRELINE pfront,
			       		 pwindPoints pwind,
			       		 ptimeSim* t)
{
	int i, j, n, tuple, fromTid, workLeft;
	
	int batchIdx = 0; //Batch ID
	int tupleIdx = 0; //Chunk ID
	int nTotalTuples; //Nº chunk in a batch
	int nTotalTask = pfront->nPoints; //Total task in iteration
	int nTuplesReceived = 0;
	int workIdx = 0; //Start's index to send tasks
	pFIRELINE pfront2;
    double compTime;
    int nPoints;
    int nTaskReceived = 0;
    bool batch_ready = 0;
    
    p->nslaves = NW;
		
	fprintf (stderr, "ITERACION %d: Using TheFactorF %f\n", iterIdx, Factor_0);
	
	// Calculate the chunk's size for batch 0
    workLeft = Factoring_CalcTuples (p->nslaves, nTotalTask, sizeof (double) * 2, Factor_0, batchIdx); // workunitsizeinbytes = 1 point (x,y)
	
    //Calculate the chunk's number of batch 0
    nTotalTuples = Factoring_GetTotalNumTuples (batchIdx);
    
	// copy the front making space for results
	pfront2 = global_copyfront (pfront);
	
	// distribute the first batch to all the workers
	for (i=0; i<p->nslaves && i < nTotalTuples; ++i)
	{
		// this functions returns size of the next tuple to be processed
		tuple = Factoring_GetNextTuple (batchIdx, tupleIdx);
		fprintf (stderr, "--->Sending work: %d tuples to worker %d\n", tuple, p->stids [i]);
		global_sendwork (batchIdx, iterIdx, p->stids [i], 0, tuple, workIdx, pmap, pveget, pfront, pwind); //?

		//workIdx lleva el control del número de tareas enviadas
		workIdx += tuple;
		tupleIdx++;
	}

    //Si quedan tareas que enviar (es decir, factor !=1): antes de comenzar a recibir el batch 0, preparamos el batch 1 
    if(workLeft != 0){
        batchIdx++;    
        tupleIdx=0;
        //float factor_b1 = 0.2;    
        workLeft = Factoring_CalcTuples (p->nslaves, workLeft, sizeof (double) * 2, Factor_1, batchIdx); // workunitsizeinbytes = 1 point (x,y)
    }
    
	// keep distributing work until all tuples are processed & received back by the master	
	fprintf (stderr, "Distributing work until all task are processed \n");
    
    int b; //variable utilizada para recoger correctamente el evento, se recoge el batch al que pertenece el chunk procesado
	while (nTaskReceived < nTotalTask)
	{	
        compTime = 0;
        //wait receive from any worker
		global_receivework (&fromTid, pfront2, &compTime, &nPoints, &b); 
		nTaskReceived += nPoints;
		fprintf(stderr,"<---Iteracion %d: Recibido worker %d n.tareas=%d\n", iterIdx, fromTid, nPoints);				
		fprintf(stderr,"--------TUPLAS RECIBIDAS=%d------TOTAL=%d-------\n", nTaskReceived, nTotalTask);	
        
		// send next chunk to workers if there are any chunk en bacthIdx
		if (Factoring_HasMoreTuples (batchIdx, tupleIdx)) // Si hay mas chunk de batchIdx
		{
			// this functions returns size of the tuple to be processed
			tuple = Factoring_GetNextTuple (batchIdx, tupleIdx);
			assert(tuple>0);
			fprintf (stderr, "--->Sending work: %d tuples to worker %d\n", tuple, fromTid);
			global_sendwork (batchIdx, iterIdx, fromTid, 0, tuple, workIdx, pmap, pveget, pfront, pwind); //?
			tupleIdx++; //Increment ID tuple to know which chunk is the next
			workIdx += tuple; //Index over total task
			
			//if(tupleIdx == 3) TheFactorF=0.3;
			
			//When does it calculate the next batch?? the next batch is calculate when the chunk's number of actual batch
			//is less than half workers' number.
			//batch_ready controls that the calculation of a single batch
			//It's verified that there are task to arrange in batchs
			if(!Factoring_HasSufTuples(batchIdx, tupleIdx) && (batch_ready == 0) && (workLeft !=0)){               
               batch_ready = 1;
               Factor_2 = F2;
               workLeft = Factoring_CalcTuples (p->nslaves, workLeft, sizeof (double) * 2, Factor_2, batchIdx+1); //ncesito para generar el evento el id de la itera como parametro o lo pongo como variable global
            }
		}
		//There aren't any chunks of the actual batch
		else{             
            //Are there any batch to send?              
            if(Factoring_HasMoreBatch(batchIdx)){ // There are some batchs
               batchIdx++;
               batch_ready = 0;
               tupleIdx = 0; // start with the first chunk of the next batch
               tuple = Factoring_GetNextTuple (batchIdx, tupleIdx);
               fprintf (stderr, "---> NEXT BATCH: Sending work: %d tuples to worker %d\n", tuple, fromTid);
		       global_sendwork (batchIdx, iterIdx, fromTid, 0, tuple, workIdx, pmap, pveget, pfront, pwind); 
		       tupleIdx++;
		       workIdx += tuple;                                     
             }
             //There aren't any batch, so the process has finished
             else{
               fprintf(stderr, "-----NO HAY MÁS QUE ENVIAR-----\n");
             }
             // Back to the top of loop to finish receiving tasks...
        }

	}
	// we have collected all perf data 
	printf ("%d tuples received\n", nTaskReceived);
	assert (nTaskReceived != 0);
	
	check_fireline ("global_sendreceive: after receiving all results", pfront2);
	/* copy moved points of the front */ 
	for (n=0;n<pfront2->nPoints; ++n)
  	{
    /* if the target area is not burned, move to new position */
		if (gis_getVegetModel(&pmap,&(pfront2->points[n]))>0)
		{
	  		pfront->points[n].x=pfront2->points[n].x;
	  		pfront->points[n].y=pfront2->points[n].y;
		}
	}  
	// free copy of the front
	free (pfront2->points);
  	free (pfront2);
}		       		 

/* IMAGINARY Dynamic load balancing of load work of the slaves */
void global_arcSlavesPartition(comm_info_master* p, pMAP* map, 
			       pvegetModels* veget,pFIRELINE* front,
			       pwindPoints* wind)
{
  int ntasks; /* number of process tasks */
  int npoints; /* number of points in fire front */
  int arcpoints; /* points in arc */
  int arcinit,arcend; /* numbers (index) of init & end points */
  int point0,pointn; /* previous and rear points to the front arc */
  int n,i;
  int message=PACKET_IDLE;
  pFIRELINE pfront;

  int vegetmodel;
  double windspeed,winddirection;
  double m,mangle;
  
  pfront=*front;

  npoints=pfront->nPoints;
  ntasks=p->nslaves;
 
  if (ntasks<npoints)
    {
      p->nutilslaves=ntasks;
    }
  else
    {
      p->nutilslaves=npoints/2; /* min of 2 points for each task */
    }

  if (ntasks==1) 
    {arcpoints=npoints;arcinit=0;arcend=npoints-1;}
  else
    {arcpoints=(int)floor((double)npoints/ntasks);}

  /* number of tasks >1 */
  arcinit=0;
  for (n=0;n<p->nutilslaves;n++)
  {
      /* is this the last task?  */
      if (n==p->nutilslaves-1) 
		{arcend=arcinit+npoints-1;}
      else arcend=arcinit+arcpoints-1;
      
      /* set the previous and rear points index */
      if (arcinit==0) point0=pfront->nPoints-1;
      else point0=arcinit-1;
      if (arcend==pfront->nPoints-1) pointn=0;
      else pointn=arcend+1;

      mpi_pack_init ();
      mpi_pkint(&arcinit,1,1);
      mpi_pkint(&arcend,1,1);
      /* pack the arc */
      mpi_pkdouble((double *)&(pfront->points[arcinit]),
		   2*(arcend-arcinit+1),1);
      /* pack the previous & rear points */
      mpi_pkdouble((double *)&(pfront->points[point0]),2,1);
      mpi_pkdouble((double *)&(pfront->points[pointn]),2,1);

      /* pack the veget model numbers, orientation & wind of points */
      /* only for interior points, not boundary points */
      for (i=arcinit;i<=arcend;i++)
      {
		  //fprintf(ff,"gis_getVegetModel i=%d, px=%f, py=%f\n",i, pfront->points[i].x, pfront->points[i].y);
			
		  vegetmodel=gis_getVegetModel(map,&(pfront->points[i]));
		  mpi_pkint(&vegetmodel,1,1);
		  gis_getOrientation(map,&(pfront->points[i]),&m,&mangle);
		  mpi_pkdouble(&m,1,1);
		  mpi_pkdouble(&mangle,1,1);
		  wind_getWind(wind,&(pfront->points[i]),&windspeed,&winddirection);
		  mpi_pkdouble(&windspeed,1,1);
		  mpi_pkdouble(&winddirection,1,1);
      }
      
      /* Send info to slave */
	MPI_Send (
		mpi_pack_buf (),
		mpi_pack_buf_pos (),
		MPI_PACKED,
		p->stids[n], 
		PACKET_ARCP, 
		MPI_COMM_WORLD); 
		      
      npoints=npoints-arcpoints;

      arcinit=arcend+1;
    }
  
  /* set a idle the not util tasks */
  for (n=p->nutilslaves;n<ntasks;n++)
  {
      mpi_pack_init();
      mpi_pkint(&message,1,1);
	  MPI_Send (
		 mpi_pack_buf (),
		 mpi_pack_buf_pos (),
		 MPI_PACKED,
		 p->stids[n], 
		 PACKET_IDLE, 
		 MPI_COMM_WORLD);      
    }
}

void global_resultsSlave(comm_info_master* p,pFIRELINE* front,pMAP* map)
{
	int arcinit,arcend;
	int actualbuf; /* Buffer actual */
	int nbytes,sendtid;
	int mtype; /* message tag */
	int n;
	pFIRELINE pfront;
	pFIRELINE pfront2;
	double calcTime;
	hrtime_t workerCalcTime;
	MPI_Status st;
	
	pfront=*front;
	
	check_fireline ("global_resultsSlave: on entry", pfront);
	
	/* copy of front */
	pfront2=(FIRELINE*) malloc(sizeof(FIRELINE));
	pfront2->points= (pointLine*) malloc(pfront->nPoints*sizeof(pointLine));
	pfront2->nPoints=pfront->nPoints;
	pfront2->tline=pfront->tline;
	
	for(n=0;n<pfront->nPoints;n++)
	{
	 	pfront2->points[n].x=pfront->points[n].x;
	  	pfront2->points[n].y=pfront->points[n].y;
	}
	
	check_fireline ("global_resultsSlave: after copy", pfront2);

  	/* receive */
  	for (n=0;n<p->nutilslaves;n++)
    {
		MPI_Recv (mpi_pack_buf (),
	    	mpi_pack_buf_size (),
	    	MPI_PACKED,
	    	MPI_ANY_SOURCE,
	    	PACKET_ARCR,
	    	MPI_COMM_WORLD,
	    	&st);
		sendtid = st.MPI_SOURCE;
   		// pvm_bufinfo(actualbuf,&nbytes,&mtype,&sendtid);
	  mpi_unpack_init ();
      mpi_upkint(&arcinit,1,1);
      mpi_upkint(&arcend,1,1);
      mpi_upkdouble((double *)&(pfront2->points[arcinit]),
		    2*(arcend-arcinit+1),1);
	  mpi_upkdouble(&calcTime, 1, 1);
	  workerCalcTime = (hrtime_t)calcTime;
	  
#ifdef DEBUG
      /*if (ff == 0)
      	ff=fopen ("points.txt", "w");	
      fprintf(ff,"Results of task %d \n",sendtid);
      fprintf(ff,"----------------\n");
      fprintf(ff,"Front with %d points. \n",arcend-arcinit+1);
      fprintf(ff,"Arc points indexs -> [%d,%d]\n",arcinit,arcend);
      for (i=arcinit;i<=arcend;i++)
	  {
	  	fprintf(ff,"Point %d -> (%f,%f)\n",i,
			 pfront2->points[i].x,pfront2->points[i].y);
	  }
      fprintf(ff, "\n");
      fflush(ff);*/
#endif
    }

check_fireline ("global_resultsSlave: after receive", pfront2);
check_firelineEx ("global_resultsSlave: after receive EX CHECK", pfront2, *map);

  /* copy moved points of the front */ 
  for(n=0;n<pfront2->nPoints;n++)
    {
      /* if the target area is not burned, move to new position */
      if (gis_getVegetModel(map,&(pfront2->points[n]))>0)
	{
	  pfront->points[n].x=pfront2->points[n].x;
	  pfront->points[n].y=pfront2->points[n].y;
	}
    }
  
  free(pfront2->points);
  free(pfront2);
}

TBOOL global_model(int iterIdx, comm_info_master* p, pMAP* map, pvegetModels* veget,
		    pFIRELINE* front, pwindPoints* wind,int acttime,int dtime, ptimeSim* t)
{
  TBOOL fresult;
//  printf("VALOR INCICIAL DE NTASKS: %d", ntasks);
/*  if (ff==0)
  	ff=fopen ("points.txt", "w");	*/

  /* Determinar l'area cremada */
  check_firelineEx ("before global_fireArea", *front, *map);
  global_fireArea(map,front);
  check_fireline ("after global_fireArea", *front);

  /* Test d'hipotesi de quasi-equilibri */
  if (global_testeq(map,veget,front)==TFALSE)
    {
    	check_fireline ("after global_testeq", *front);
      /* No es cumpleix la hipotesi, cal generar nous punts */
      global_newpoints2(map,front);
    }
    
  /* ORIGINAL CODE (no tuning)  
  check_fireline ("before global_arcSlavesPartition", *front);
  global_arcSlavesPartition(p,map,veget,front,wind);

  // Aplicacio del model global (front(t) -> front(t+dt) 
  // --- global_kernel(map,veget,front,wind,acttime,dtime); *
  check_fireline ("before global_resultsSlave", *front);
  global_resultsSlave(p,front,map);
  */
  
  check_fireline ("before global_sendreceive", *front);
  
  // SETUP GLOBAL VARS FOR TUNER
  TheTotalWork = (*front)->nPoints;
  Factor_0 = F0;
  Factor_1 = F1;
  Factor_2 = F2;
  NW = p->nslaves;
  
  //global_sendreceive (iterIdx,p,*map,*veget,*front,*wind, t, schedData, schedProc);
  global_sendreceive (iterIdx,p,*map,*veget,*front,*wind, t);
  check_fireline ("after global_sendreceive", *front);
  
  fresult = global_testFrontInLand(map,front);
  check_fireline ("after global_testFrontInLand", *front);
  return fresult;
}


