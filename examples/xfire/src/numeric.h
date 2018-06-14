/*
** ----------------------------------------------------------------------
**
** FILE:	NUMERICAL ALGORITHMS
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

#ifndef _NUMERICALH_
#define _NUMERICALH_

/* Routines de splines */

/* 
   Spline : Interpolacio per splines d'una corba (x,f(x))
   x,f,n: corba original de n funts de la forma x,f(x)
   xi,fi,ni: ni punts a interponal en la corba anterior
*/
void spline(double *x, double *f, int n, 
	    double *xi, double *fi, int ni);
/* Resolucio del sistema tridiagonal amb condicio de frontera ciclica */
void tridcy(double *a, double *b, double *c, double *s, double *dd,int n);

#endif
