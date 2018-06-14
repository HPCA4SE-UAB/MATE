/*
** ----------------------------------------------------------------------
**
** FILE:	A second factor : ORIENTATION 
**
**  (C-) Josep Jorba, 1997		C.A.O.S. - UAB
** 
** ----------------------------------------------------------------------
**
** Date			Who		What
** xx-01-98		JJ		Creation
**
** ----------------------------------------------------------------------
*/
#include <math.h>
#include <assert.h>
#include "localmod.h"
#include "factor2.h"

/* 
** Modul de calcul de l'efecte del angle entre n i n0
**
** Es passa parametres locals, velocitat en direccio de maxima variacio
** i l'angle entre n i n0
** Es retorna fn(ang)
** 
*/
/* Funció de forma, doble el.lipse de Anderson */
double func_F(double phi, double u)
{
  double d180;
  double b;
  double c1,c2,a1,a2,n;
  double result;
  double phi_equiv;
  double tphi,tphi2;
  double x,xinit,xend; /* vars por bisection method */
  double realphi,r,r2;
	int i=0;
  d180=0.492*exp(-0.1148*u);
  b=0.534*exp(-0.0713*u);
  c1=(d180/2.0)*(pow(b/d180,2.0)-1.0);
  
  /* quadrants inferiors a als superiors */
  if (phi>M_PI) { phi=-(2*M_PI-phi);}
  if ((phi<0) && (phi>-(M_PI))) 
    { phi=-phi;}
  /* Determinar quin quadrant superior estem */
  assert (c1 != 0);
  if ((phi<=M_PI) && (phi>=atan(b/c1)))
    {
      /* quart el.lipse, darrera superior*/
      a1=c1+d180;
      assert (a1-(c1*cos(phi)) != 0);
      result=pow(b,2.0)/(a1-(c1*cos(phi)));
    }
  else
    {
      /* quart el.lipse, davant-superior */
      /* DEPURAR: esta en aquest quadrant segur? */
      
      /* Primer de tot cal trobar el phi_equiv de la segona el.lipse */
      a2=1-c1;
      assert (pow(a2,2.0)-pow(b,2.0) >=0);
      c2=sqrt(pow(a2,2.0)-pow(b,2.0));
      n=c2-c1;

      /* angles limits del nostre tros de el.lipse */
      assert (c1 != 0);
      assert (c2 != 0);
      tphi=atan(b/c1);  /* phi1 */
      tphi2=atan(b/c2); /* phi'1 */

      realphi=phi; /* Valor actual que tenim de phi */

      /* Primer cal trobar el valor de phi' que correspon */
      /* Metode de bisecció per trobar el valor de phi */
      xinit=0.0;
      xend=tphi2; /* phi'1 */
      do
	  {
	  	x=(xinit+xend)/2.0;
      	assert ((a2-c2*cos(x)) != 0);
	  	r2=pow(b,2.0)/(a2-c2*cos(x));
	  	assert (pow(r2,2.0)+pow(n,2.0)-2*r2*n*cos(x) >= 0);
	  	r=sqrt(pow(r2,2.0)+pow(n,2.0)-2*r2*n*cos(x));
	  	assert (r != 0);
	  	phi=asin((r2*sin(x))/r);
	    if (phi>realphi)
	    { 
	    	xend=x;
	    }
	  	else
	    { 
	    	xinit=x;
	    };
	    if (i>10000)
	    	break;
	    i++;
	  }
      while (fabs(realphi-phi)>=0.001);
      /* ^^^^ Tolerancia de la biseccio es de 0.001 */
      phi_equiv=x;
      phi=realphi;
     
      result=sqrt(pow(x,2.0)+pow(n,2.0)-2*x*n*cos(phi_equiv));
    }
  
  return result;
}

void factor2_calculation(plocalParams p,double Rn0,double angle, double* fn)
{
  double Ue;
  double x,xnext;
  double f,fd; /* Funcio de forma i derivada */
  double h; /* per aproximacio de la derivada de f */

  /* Calcul del vent equivalent */
  Ue=localmod_equivWind(Rn0,p->w0,p->beta,p->sigma,p->St,p->Se,
			   p->Mf,p->Mx,p->h,p->rhos);


  /* Algorisme iteratiu per trobar phi a partir del angle que tenim */
  
  xnext=angle; /* Inici phi=alpha*/
  
  do
    {
      x=xnext;
      f=func_F(x,Ue);
      /* Aproximar la derivada de f, per la regla de tres punts */
      h=0.1;
      fd=(1.0/2.0*h)*(-3.0*func_F(x,Ue)+4.0*func_F(x+h,Ue)-func_F(x+2.0*h,Ue));
      assert (sqrt(pow(f,2.0)+pow(fd,2.0)) != 0);
      xnext=angle+x-acos((fd*sin(x)+f*cos(x))/sqrt(pow(f,2.0)+pow(fd,2.0)));
    }
  while (fabs(xnext-x)>=0.05);
  /* ^^^^^^ tolerancia metode a 0.05, amb 0.01 oscil.lacions de vegades */

  /* Ja tenim phi */
  x=xnext;
  assert (sqrt(pow(fd,2.0)+pow(f,2.0)) != 0);
  *fn=pow(f,2.0)/sqrt(pow(fd,2.0)+pow(f,2.0));

  /* *fn=1.0; <=Si, llavors factor no afecta */
}
