/*
** ----------------------------------------------------------------------
**
** FILE:			A GIS functions
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
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <assert.h>
//SOLO PARA SOLARIS
//#include <ieeefp.h>


#include "gisfuncs.h"

/* Variables globals al modul CHANGE */
int numlands=0;
int numfronts=0;

/* Funcions d'ajust de punts a la reixa */
/* Ajust a sur-oest */
void gis_roundPointSO(pMAP* map, pointLine p, pointLine* pnext)
{
  pMAP pmap;
  int x0,y0;
 
  pmap=*map;
  x0= (int)floor((p.x - pmap->xmin) / pmap->xdelta);
  y0= (int)floor((p.y - pmap->ymin) / pmap->ydelta);
  pnext->x=pmap->mapMatrix[x0][y0].x;
  pnext->y=pmap->mapMatrix[x0][y0].y;
}

/* Ajust a nord-est*/
void gis_roundPointNE(pMAP* map, pointLine p, pointLine* pnext)
{
  pMAP pmap;
  int x0,y0;
 
  pmap=*map;
  x0= (int)floor((p.x - pmap->xmin) / pmap->xdelta);
  y0= (int)floor((p.y - pmap->ymin) / pmap->ydelta);
  pnext->x=pmap->mapMatrix[x0+1][y0+1].x;
  pnext->y=pmap->mapMatrix[x0+1][y0+1].y;
}

/* Calcul del coeficients de l'equacio del pla */
void gis_planeCofs(double *x, double *y,double *z, int num, 
		   double *a, double *b, double *c, double *d)
{
  double A,B,C,D;
  int i,j;

  A=B=C=D=0;
  for (i=0;i<num;i++)
    {
      j=(i+1) % num;
      A+=(z[i]+z[j])*(y[j]-y[i]);
      B+=-(x[i]+x[j])*(z[j]-z[i]);
      C+=(y[i]+y[j])*(x[j]-x[i]);
    }
  A/=2.0; B/=2.0; C/=2.0;
  D=-(A*x[0]+B*y[0]+C*z[0]);

  *a=A;*b=B;
  *c=C;*d=D;
}

/* Distancia entre dos punts */
double gis_dist2d(pointLine p, pointLine q)
{
  return sqrt(pow(q.x-p.x,2.0)+pow(q.y-p.y,2.0));
}

/* Calculation of circle parameters */
void gis_circle(pointLine p1, pointLine p2, pointLine p3,
		 double* a, double* b, double* r)
{
  double f,g,c;
  
  double t1,x2,x3,y2,y3;

  t1=pow(p1.x,2.0)+pow(p1.y,2.0);
  x2=p2.x-p1.x;
  x3=p3.x-p1.x;
  y2=p2.y-p1.y;
  y3=p3.y-p1.y;

  if ((2*x2*y3-2*x3*y2) != 0)
  	f=((t1-pow(p3.x,2.0)-pow(p3.y,2.0))*x2- (t1-pow(p2.x,2.0)-pow(p2.y,2.0))*x3)/(2*x2*y3-2*x3*y2);
  else
  	f=0;
  if (x2 != 0)
  	 g=(t1-pow(p2.x,2.0)-pow(p2.y,2.0)-2*y2*f)/(2*x2);
  else
     g=0;
  /* Calcul de c, pero hi han errors de redondeig */
  c=-t1-2*g*p1.x-2*f*p1.y;

  *a=-g;
  *b=-f; /* Center (a,b) */
  /* Calcul del radi seria aquest, pero errors en c */
  *r=sqrt(pow(*a,2.0)+pow(*b,2.0)-c);

  /* *r=sqrt(pow(p1.x-(*a),2.0)+pow(p1.y-(*b),2.0)); */
}

/* Normalitzar vector */
void gis_normalvect(double* x, double* y)
{
  double module;
  assert (!isnan (*x));
  assert (!isnan (*y));
  assert (pow(*x,2.0)>=0);
  assert (pow(*y,2.0)>=0);
  module=sqrt(pow(*x,2.0)+pow(*y,2.0));
  if (module != 0)
  {
  	*x=(*x)/module;
  	*y=(*y)/module;
  }
  else
  {
  	 *x = 0;
  	 *y = 0;
  }
}

/* Calcular la normal de un arc del front definit per tres punts */
void gis_arcnormal(pFIRELINE* f,int n, double* nx,double* ny)
{
  pointLine p1,p2,p3;
  pFIRELINE pfront;
  int npoints;
  int ip1,ip2,ip3; /* Indices de los puntos */
  int cp; /* Punt on s'ha de calcular la normal */
  /* a,b dist triangle format pels punts, i radi del cercle */
  double a,b,r;
  double m; /* modul del vector normal */
  double x,y;
  double x1,y1,x2,y2; /* Punts extrems*/
  double normaldir;
  double yproj;

  pfront=*f;

  npoints=pfront->nPoints;

  cp=2; /* Calcular normal en el punt central */

  if (pfront->tline==FL_CLOSEDCURVE)
  {
      if (n==npoints-1)
	  {
	  	ip1=n-1;ip2=n;ip3=0;
	  }
      else if (n==0)
	  {
	  	ip1=npoints-1;ip2=0;ip3=1;
	  }
      else
	  {
	    ip1=n-1;ip2=n;ip3=n+1;
	  }
  }
  else
  {
      if (n==npoints-1)
	  {
	    ip1=n-2;ip2=n-1;ip3=n;cp=3;
	  }
      else if (n==0)
	  {
	    ip1=0;ip2=1;ip3=2;cp=1;
	  }
      else
	  {
	   ip1=n-1;ip2=n;ip3=n+1;
	  }
  }

   p1.x=pfront->points[ip1].x;
   p1.y=pfront->points[ip1].y;
   p2.x=pfront->points[ip2].x;
   p2.y=pfront->points[ip2].y;
   p3.x=pfront->points[ip3].x;
   p3.y=pfront->points[ip3].y;
   assert (!isnan (p1.x));
   assert (!isnan (p1.y));
   assert (!isnan (p2.x));
   assert (!isnan (p2.y));
   assert (!isnan (p3.x));
   assert (!isnan (p3.y));
  
   gis_circle(p1,p2,p3,&a,&b,&r);

   switch (cp) 
     {
     case 1:
       x=p1.x;y=p1.y;     
       break;
     case 2:
       x=p2.x;y=p2.y;break;
     case 3:
       x=p3.x;y=p3.y;break;
     }

   /* orientation of normal */
   normaldir=1.0; /* direction center to point */
   /* secant calculation */
   x1=p1.x;y1=p1.y;
   x2=p3.x;y2=p3.y;
   if ((y2-y1)!=0.0) /* not a horitzontal line */
     {
       if ((x2-x1)!=0.0) /* not a vertical line */
	   {
	   	 assert (x2-x1 != 0);
	     m=(y2-y1)/(x2-x1); /* slope */
	     b=y1-m*x1;
	     /* projected central point in secant */
	     yproj=m*p2.x+b;

	   if (yproj>p2.y) normaldir=-1.0;
	 }
       else
	 {
	   if (p2.x<x2) normaldir=-1.0;
	 }
     }
   else
     {
       if (p2.y<y2) normaldir=-1.0;
     }
   /* Center to point vector */
   assert (!isnan (x));
   assert (!isnan (a));
   assert (!isnan (y));
   assert (!isnan (b));
   *nx=x-a;
   *ny=y-b;
   assert (!isnan (*nx));
   assert (!isnan (*ny));
   /* normalize */
   assert (pow(*nx,2.0)+pow(*ny,2.0) >=0);
   m=sqrt(pow(*nx,2.0)+pow(*ny,2.0));
   assert (m != 0);
   *nx=(*nx/m)*normaldir;
   *ny=(*ny/m)*normaldir;
}

/* Calcula la normal (3D) i la projecta en el pla x,y */
void gis_normalprojection(pMAP* map, pointLine* p, double* nx, double* ny)
{
  pMAP pmap;
  double *x, *y, *z;
  double a,b,c,d,modul;
  int num=4; /* Els quatre punts veins */
  int x0,y0; /* El punt que esta al extrem inferior esquerre */
  /* el punt es terreny[x0][y0], on x0,y0 coordenades de matriu */ 

  pmap=*map;
  x = (double *) malloc(num*sizeof(double));
  y = (double *) malloc(num*sizeof(double));
  z = (double *) malloc(num*sizeof(double));

  x0= (int)floor((p->x - pmap->xmin) / pmap->xdelta);
  y0= (int)floor((p->y - pmap->ymin) / pmap->ydelta);

  x[0]=pmap->mapMatrix[x0][y0].x;
  y[0]=pmap->mapMatrix[x0][y0].y;
  z[0]=pmap->mapMatrix[x0][y0].z;

  x[1]=pmap->mapMatrix[x0+1][y0].x;
  y[1]=pmap->mapMatrix[x0+1][y0].y;
  z[1]=pmap->mapMatrix[x0+1][y0].z;

  x[2]=pmap->mapMatrix[x0+1][y0+1].x;
  y[2]=pmap->mapMatrix[x0+1][y0+1].y;
  z[2]=pmap->mapMatrix[x0+1][y0+1].z;

  x[3]=pmap->mapMatrix[x0][y0+1].x;
  y[3]=pmap->mapMatrix[x0][y0+1].y;
  z[3]=pmap->mapMatrix[x0][y0+1].z;

  gis_planeCofs(x,y,z,num,&a,&b,&c,&d);
  /* Normalitzar el vector projectat (a,b) */
  modul= sqrt(pow(a,2.0)+pow(b,2.0));
  a=a/modul;
  b=b/modul;
  *nx=a;*ny=b;

  free(x);
  free(y);
  free(z);
}

void gis_getOrientation(pMAP* map, pointLine* p, double* m, double* mangle)
{
  pMAP pmap;
  int x0,y0;
  int npoint;
  int xmap0,ymap0;
  int z[9];
  double a,b;

  pmap=*map;
  assert (pmap->xdelta != 0);
  assert (pmap->ydelta != 0);
  assert (p->x >= pmap->xmin);
  assert (p->y >= pmap->ymin);
  
  x0= (int)floor((p->x - pmap->xmin) / pmap->xdelta);
  y0= (int)floor((p->y - pmap->ymin) / pmap->ydelta);
  assert (x0 >=0);
  assert (y0 >=0);
  
  xmap0=(int)pmap->mapMatrix[x0][y0].x;
  ymap0=(int)pmap->mapMatrix[x0][y0].y;

  if (p->x > (xmap0+pmap->xdelta))
  {
      if (p->y > (ymap0+pmap->ydelta)) 
      	npoint=3;
      else 
        npoint=2;
  }
  else if (p->y > (ymap0+pmap->ydelta)) 
  	npoint=4;
  else 
    npoint=1;
  
  /* Punto mas proximo a nuestro punto */
  switch(npoint)
    {
    case 1: x0=x0;y0=y0;break;
    case 2: x0=x0+1;y0=y0;break;
    case 3: x0=x0+1;y0=y0+1;break;
    case 4: x0=x0;y0=y0+1;break;
    }

  /* ania&olegm patch */
  if (x0 == 0) x0 = 1;
  if (y0 == 0) y0 = 1;

  /* Punts:
    0 1 2
    3 4 5
    6 7 8
   */
  assert (x0>0);
  assert (y0>0);
  z[0]=(int)pmap->mapMatrix[x0-1][y0+1].z;
  z[1]=(int)pmap->mapMatrix[x0][y0+1].z;
  z[2]=(int)pmap->mapMatrix[x0+1][y0+1].z;
  z[3]=(int)pmap->mapMatrix[x0-1][y0].z;
  z[4]=(int)pmap->mapMatrix[x0][y0].z;
  z[5]=(int)pmap->mapMatrix[x0+1][y0].z;
  z[6]=(int)pmap->mapMatrix[x0-1][y0-1].z;
  z[7]=(int)pmap->mapMatrix[x0][y0-1].z;
  z[8]=(int)pmap->mapMatrix[x0+1][y0-1].z;

  b=(z[0]+z[1]+z[2]-z[6]-z[7]-z[8])/(6.0*pmap->ydelta);
  a=(z[2]+z[5]+z[8]-z[0]-z[3]-z[6])/(6.0*pmap->xdelta);
  
  assert (pow(a,2)+pow(b,2) >=0);
  *m=sqrt(pow(a,2)+pow(b,2));
  assert (!isnan (*m));
  if (a==0)
     *mangle = atan (0);
   else
     *mangle=atan(b/a);
  assert (!isnan (*mangle));
}

int  gis_getVegetModel(pMAP* map, pointLine* p)
{
  pMAP pmap;
  int x0,y0;
  int npoint;
  int xmap0,ymap0;
  int model;

  pmap=*map;
  assert (pmap != 0);
  assert (p != 0);

  if ((p->x < pmap->xmin) || (p->y < pmap->ymin) ||
   (p->x > pmap->xmax) || (p->y > pmap->ymax))
  {
  	 return 0;
  }
  
  x0= (int)floor((p->x - pmap->xmin) / pmap->xdelta);
  y0= (int)floor((p->y - pmap->ymin) / pmap->ydelta);
  assert (x0 >= 0);
  assert (y0 >= 0);
    
  if (x0 > pmap->xmax-pmap->xmin)
  {
  	 fprintf (stderr, "p->x=%f, p->y=%f <==> pmap->xmax=%f, pmap->ymax=%f\n", 
  	 	p->x,p->y, pmap->xmax, pmap->ymax);
  	 if (isnan (p->x))
  	 	fprintf (stderr, "WAS NAN\n");
  	 fflush (stderr);
  }


  assert (x0 <= pmap->xmax-pmap->xmin);
  assert (y0 >= 0);
  assert (y0 <= pmap->ymax-pmap->ymin);
   
  xmap0=(int)pmap->mapMatrix[x0][y0].x;
  ymap0=(int)pmap->mapMatrix[x0][y0].y;

  if (p->x > (xmap0+pmap->xdelta))
    {
      if (p->y > (ymap0+pmap->ydelta)) npoint=3;
      else npoint=2;
    }
  else 
    if (p->y > (ymap0+pmap->ydelta)) npoint=4;
    else npoint=1;
  
  switch(npoint)
    {
    case 1: model=pmap->mapMatrix[x0][y0].model;break;
    case 2: model=pmap->mapMatrix[x0+1][y0].model;break;
    case 3: model=pmap->mapMatrix[x0+1][y0+1].model;break;
    case 4: model=pmap->mapMatrix[x0][y0+1].model;break;
    }

  return model;
}


/* Setup de les estructures del GIS*/
void gis_setup(pMAP* m, pFIRELINE* f, char* landFile, char* frontFile)
{
  *m=(MAP *) malloc(sizeof(MAP));
  (*m)->map=NULL;
  /* Suposem una unica linia de front de moment CHANGE */
  *f=(FIRELINE *) malloc(sizeof(FIRELINE));
  (*f)->points=NULL;
  (*f)->nPoints=0;

  gis_readLandFile(m,landFile);     /* Llegir fitxer de terreny */
  gis_readFrontFile(f,frontFile);
}

/* Alliberacio de les estructures */
void gis_unsetup(pMAP* m, pFIRELINE* f)
{
  if ((*m) != NULL)
    {
      if ((*m)->map != NULL) {free((*m)->map);free((*m)->mapMatrix);};
      free(*m);
    }

  if ((*f) != NULL)
    {
      if ((*f)->points != NULL) free((*f)->points);
      free(*f);
    }
}

/* Estadisticas del terreny i fronts actuals */
void gis_statistics(pMAP* m,pFIRELINE* f)
{
  pMAP pm;
  pFIRELINE pf;
  pointLine *p;
  int i,j;

  pm=*m;
  pf=*f;
  fprintf(stderr, "LAND statistics:\n");
  fprintf(stderr, "Map with: %d points\n",(pm->xdim)*(pm->ydim));
  fprintf(stderr, "%8.2f <=   x   <= %8.2f\n", pm->xmin, pm->xmax);
  fprintf(stderr, "%8.2f <=   y   <= %8.2f\n", pm->ymin, pm->ymax);
  fprintf(stderr, "%8.2f <=   z   <= %8.2f\n", pm->zmin, pm->zmax);
  fprintf(stderr, "%10d <= model <= %10d\n\n", pm->modelmin, pm->modelmax);
  fprintf(stderr, "FRONTS statistics\n");
  fprintf(stderr, "%d fronts\n", numfronts);

  for (i=1;i<=numfronts;i++)
  {
    fprintf(stderr, "Front %d with %d points.\n",i,pf->nPoints);
    switch(pf->tline)
      {
      case FL_POINT: fprintf(stderr, "Type POINT\n"); break;
      case FL_LINE: fprintf(stderr, "Type LINE\n"); break;
      case FL_OPENCURVE:  fprintf(stderr, "Type OPENCURVE\n"); break;
      case FL_CLOSEDCURVE:  fprintf(stderr, "Type CLOSEDCURVE\n"); break;
      }
    p=pf->points;
    /*
    for (j=1;j<=pf->nPoints;j++)
      {
	fprintf(stderr,"(%8.2f,%8.2f) ",p->x,p->y);
	p++;
      }
    fprintf(stderr,"\n");
    */
    pf++;
  }
  fprintf(stderr,"\n");
}

/* 
** Lectura d'un fitxer de terreny amb format raster 
** s'utilitza la variable numlands per indicar terrenys disponibles
*/
void gis_readLandFile(pMAP* m, char* name)
{
  FILE* fp;
  pointMap* p=NULL;
  pointMap** pMatrix=NULL;
  int i,j,k;
  pMAP pm=NULL; /* punter al terreny actual */

  if ((fp=fopen(name,"r"))==NULL)
    { printf("Land File error. exit...");exit(1);}
  else
    {
      numlands=1; /* Variable global al modul */
      pm=*m;  /* pm es el terreny actual amb que tractem */
      /* S'utilitza per facilitar la lectura pm == (*m) */
      fscanf(fp,"%d %d",&(pm->xdim),&(pm->ydim));
      /* el pas entre dos punts es DIM_DELTA (ara 50) */
      pm->xdelta=pm->ydelta=DIM_DELTA;

      pm->map= (pointMap *) malloc((pm->xdim)*(pm->ydim)*sizeof(pointMap));
      p=pm->map; /* Punt actual que estem llegint */

      pMatrix=pm->mapMatrix; /* Per indexar el terreny */
      pm->mapMatrix= (pointMap **) malloc((pm->xdim)*sizeof(pointMap *));
      pMatrix=pm->mapMatrix;
      for (k=0;k<(pm->xdim);k++) pMatrix[k]=(pm->map)+(k*(pm->ydim));

      pm->xmax=pm->ymax=pm->zmax=0.0;
      pm->xmin=pm->ymin=pm->zmin=DBL_MAX;
      pm->modelmax=0;pm->modelmin=INT_MAX;

      for (i=0;i<pm->xdim;i++)
	for (j=0;j<pm->ydim;j++)
	  {
	  fscanf(fp,"%lf %lf %lf %d",&(p->x),&(p->y),&(p->z),&(p->model));
	  pm->xmax=((pm->xmax)>(p->x)) ? pm->xmax : p->x;
	  pm->ymax=((pm->ymax)>(p->y)) ? pm->ymax : p->y;
	  pm->zmax=((pm->zmax)>(p->z)) ? pm->zmax : p->z;

	  pm->xmin=((pm->xmin)<(p->x)) ? pm->xmin : p->x;
	  pm->ymin=((pm->ymin)<(p->y)) ? pm->ymin : p->y;
	  pm->zmin=((pm->zmin)<(p->z)) ? pm->zmin : p->z;

	  pm->modelmax=((pm->modelmax)>(p->model)) ? pm->modelmax : p->model;
	  pm->modelmin=((pm->modelmin)<(p->model)) ? pm->modelmin : p->model;
	  ++p; /* Estem col.locant a pMatrix[i][j] */
	  };  
      fclose(fp);
      /* DEBUG:
	 p=(pm->map)+3;
	 printf("Des map    (0,3) :%lf %lf %lf %d\n",p->z,p->y,p->z,p->model);
	 printf("Des matrix (0,3) :%lf %lf %lf %d\n",
	    pMatrix[0][3].z,pMatrix[0][3].y,
	    pMatrix[0][3].z,pMatrix[0][3].model);
      */
    }
}

/* Lectura d'un fitxer de definicio de fronts */
void gis_readFrontFile(pFIRELINE* f, char* name)
{
  FILE* fp;
  pointLine* p=NULL;
  pFIRELINE pf=NULL; /* Punter al front actual */
  int iFronts=0,iPoints=0;
  char frontType[80];

  if ((fp=fopen(name,"r"))==NULL)
    { printf("Front File error. exit...");exit(1);}
  else
    {
      /* Quants fronts tenim? */
      fscanf(fp,"%d",&numfronts);
      /* Codi per si tenim mes d'un fron CHANGE */
      free(*f); /* sols teniem memoria per un */
      *f=(FIRELINE*) malloc(numfronts*sizeof(FIRELINE));
      pf=*f; /* pf es el punter al front actual */
      pf->points=NULL;
      pf->nPoints=0;
      /* Per facilitar la lectura del codi, pf == (*m) */

      /* Per cada front */
      for (iFronts=1;iFronts<=numfronts;iFronts++)
	{
	  fscanf(fp,"%s",frontType);
	  /* Identificacio del tipus de front i quants punts te */
	  if (strcmp(frontType,"POINT")==0)
	    { pf->nPoints=1; pf->tline=FL_POINT;}
	  else if (strcmp(frontType,"LINE")==0)
	    { pf->nPoints=2; pf->tline=FL_LINE;}
	  else 
	    {
	      fscanf(fp,"%d",&(pf->nPoints));
	      if (strcmp(frontType,"OPENCURVE")==0)
		{ pf->tline=FL_OPENCURVE; }
	      else if (strcmp(frontType,"CLOSEDCURVE")==0)
		{ pf->tline=FL_CLOSEDCURVE; }
	    }
	  /* reservar memoria pel punts i llegir-los */
	  pf->points= (pointLine*) malloc(pf->nPoints*sizeof(pointLine));
	  p=pf->points;
	  for (iPoints=1;iPoints<=pf->nPoints;iPoints++)
	    {
	      fscanf(fp,"%lf %lf",&(p->x),&(p->y));
	      if (isnan (p->x))
	      	 { printf("Front File error. x NAN. exit...");exit(1);}
		  if (isnan (p->y))
	      	 { printf("Front File error. y NAN. exit...");exit(1);}	      	
	      ++p;
	    }
	  ++pf; /* Seguent front*/
	}
      fclose(fp);
    }
    printf("Front file loaded. All ok\n");
}

