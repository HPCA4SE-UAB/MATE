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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "numeric.h"

/*
   Interpolacio per mitja de splines cubiques

   x -> valors x de cada punt
   f -> valors de la funcio en cada punt x
   n -> numero de valors que formen la taula (menys un)
   xi -> valors x que volem interpolar
   ni -> numero de valors a interpolar

   
*/
void spline(double *x, double *f, int n, 
	    double *xi, double *fi, int ni)
{
  int i,j,k;
  int im;
  double *h, *a, *b, *c, *s, z;
  double *dd;

  /* Creacio dinamica dels arrays temporals */
  h=(double *) malloc((n+1)*sizeof(double));
  a=(double *) malloc((n+1)*sizeof(double));
  b=(double *) malloc((n+1)*sizeof(double));
  c=(double *) malloc((n+1)*sizeof(double));
  s=(double *) malloc((n+1)*sizeof(double));

  dd=(double *) malloc((n+1)*sizeof(double));

  n=n-1; /* l'algorisme suposa que n es el numero de punts -1 */

  im=n-1;
  for (i=0;i<=n-1;i++) h[i]=x[i+1]-x[i];
  for (i=1;i<=n-1;i++)
    {
      a[i]=h[i-1];
      c[i]=h[i];
      b[i]=2*(a[i]+c[i]);
      s[i]=6*((f[i-1]-f[i])/h[i-1]+(f[i+1]-f[i])/h[i]);
    }

  /* Condicio de frontera, suposem corba ciclica (2 derivada igual)*/
  a[n]=h[n-1];
  c[n]=h[0];
  b[n]=2*(a[n]+c[n]);
  s[n]=6*((f[n-1]-f[n])/h[n-1] + (f[1]-f[0])/h[0]);
  tridcy(a,b,c,s,dd,n);
  dd[0]=dd[n];
  /* fi tractament condicio de frontera */

  j=0;
  for (k=0;k<=(ni-1);k++)
    {
      if ((xi[k] < x[0]) || (xi[k]>x[n]))
	{
	  printf("Spline...Out of range 1\n");
	  exit(0);
	}
      
      if ((xi[k] >= x[j]) && (xi[k]<=x[j+1]))
	{
	  z=xi[k]-x[j];
	  fi[k] = f[j]+(-(2.0*dd[j]+dd[j+1])/6.0*h[j] + (f[j+1]-f[j])/h[j])*z 
	    + (dd[j+1]-dd[j])/6.0/h[j]*pow(z,3.0)+dd[j]*pow(z,2.0)/2.0;
	}
      else
	{
	  if (j > n)
	    {
	      printf("Spline...Out of range 2\n");
	      exit(0);
	    }
	  else
	    {
	      j++;
	      --k;   /* Ejem: Fortran -> C */
	    }
	}
    }
  free(h);
  free(a);
  free(b);
  free(c);
  free(s);
  free(dd);
}

/* Calcul del sistema tridiagonal, amb condicio de frontera ciclica */
void tridcy(double *a, double *b, double *c, double *s, double *dd,int n)
{
  double *v;
  double *h;
  int im;
  int i;
  double r,p,t;

  v=(double *) malloc((n+1)*sizeof(double));
  h=(double *) malloc((n+1)*sizeof(double));

  v[1]=a[1];
  h[1]=c[n];
  h[n-1]=a[n];
  h[n]=b[n];
  v[n-1]=c[n-1];
  im=n-1;
  for (i=2;i<=im;i++)
    {
      r=a[i]/b[i-1];
      b[i]=b[i]-r*c[i-1];
      s[i]=s[i]-r*s[i-1];
      v[i]=v[i]-r*v[i-1];
      p=h[i-1]/b[i-1];
      h[i]=h[i]-p*c[i-1];
      h[n]=h[n]-p*v[i-1];
      s[n]=s[n]-p*s[i-1];
    }
  t=h[n-1]/b[n-1];
  h[n]=h[n]-t*v[n-1];
  dd[n]=(s[n]-t*s[n-1])/h[n];
  dd[n-1]=(s[n-1]-v[n-1]*dd[n])/b[n-1];
  for (i=n-2;i>=1;i--)
    dd[i]=(s[i]-v[i]*dd[n]-c[i]*dd[i+1])/b[i];

  free(v); 
  free(h);
}
