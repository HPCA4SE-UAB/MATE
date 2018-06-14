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

/*
** Components del sistema, funcions relacionades amb GIS
** Estructures de dades: definicio front i terreny
** aixi com les funcions per manipular aquests objectes
*/

#ifndef _GIS_H_
#define _GIS_H_

/* Pas de terreny es de 50 (UTM) tant en x com en y */
/* Maxima distancia emtre dos punts del front, a 3 cel.les */
#define DIM_DELTA 50
#define MAXDIST_TEST 3*DIM_DELTA

/* 
** Tipus de fronts permessos en el GIS:
**      Linies rectes, corbes obertes i tancades.
*/
typedef enum {
  FL_POINT=0, FL_LINE=1, FL_OPENCURVE=2, FL_CLOSEDCURVE=3 } lineType;

/* Un punt de la linia */
typedef struct {
  double x,y;
} pointLine;

/* definicio del front */
typedef struct {
  lineType   tline;      /* Tipus de front */
  int        nPoints;    /* Numero de punts */
  pointLine* points;     /* Punts que formen el front */
} FIRELINE;

/*
** Estructures per definir el terreny
*/
/* Definicio d'un punt del mapa */
typedef struct {
  double x;
  double y;
  double z;  /* coordenades d'un punt (x,y,z) */
  int model; /* nº de model de vegetacio */
} pointMap;

/* Definicio del mapa, es considera el mapa de tipus raster */
typedef struct {
  pointMap* map; /* Permet access sequencial al terreny*/
  pointMap** mapMatrix; /* Access indexat al terreny */
  int   xdim,ydim;
  double xdelta,ydelta;
  double xmin,xmax;
  double ymin,ymax;
  double zmin,zmax;
  int   modelmin,modelmax;
} MAP;

/* Definicio d'un tipus apuntador a MAP i a FIRELINE */
typedef MAP* pMAP;
typedef FIRELINE* pFIRELINE;

/* GIS functions */

/* Funcions d'ajust de punts a la reixa */
/* Ajust a sur-oest */
void gis_roundPointSO(pMAP* map, pointLine p, pointLine* pnext);
/* Ajust a nord-est*/
void gis_roundPointNE(pMAP* map, pointLine p, pointLine* pnext);

/* Calcul del coeficients de l'equacio del pla */
void gis_planeCofs(double *x, double *y,double *z, int num, 
		   double *a, double *b, double *c, double *d);
/* Distancia entre dos punts */
double gis_dist2d(pointLine p, pointLine q);
/* Cercle format per tres punts */
void gis_circle(pointLine p1, pointLine p2, pointLine p3,
		 double* a, double* b, double* r);
/* Normalitzar vector */
void gis_normalvect(double* x, double* y);
/* Calcular la normal de un arc del front definit per tres punts */
void gis_arcnormal(pFIRELINE* f,int n, double* nx,double* ny);
/* Calcula la normal (3D) i la projecta en el pla x,y */
void gis_normalprojection(pMAP* map, pointLine* p, double* nx, double* ny);
void gis_getOrientation(pMAP* map, pointLine* p, double* m, double* angle);
int  gis_getVegetModel(pMAP* map, pointLine* p);
void gis_setup(pMAP* m, pFIRELINE* f, char* landFile, char* frontFile);
void gis_unsetup(pMAP* m, pFIRELINE* f);
void gis_statistics(pMAP* m, pFIRELINE* f);
void gis_readLandFile(pMAP* m, char* name);
void gis_readFrontFile(pFIRELINE* m, char* name);

#endif /* _GIS_H_ */
