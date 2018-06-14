/*
** ----------------------------------------------------------------------
**
** FILE:	A first factor : Rmax
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

#include <assert.h>
#include <math.h>
//SOLO PARA SOLARIS??
//#include <ieeefp.h>
#include "localmod.h"
#include "factor1.h"

/* 
** Determinacio de Rmax a partir del calcul de tres components separades
** Rn(0º,0), Rn(angle,0) i Rn(0º,U) i posteriorment calcul de Rn0 a traves
** de la Hipotesi lineal de Rothermel (1983)
**
** Nota: en lloc de l'engle s'utilitza directament la pendent,
** pendent = tan angle.
*/
void factor1_calculation(plocalParams p,double nx,double ny,pvectRmax r)
{
  double Rn1,Rn2,Rn3;
  double dRn2x,dRn2y,dRn3x,dRn3y;
  double modul;

  assert (!isnan (nx));
  assert (!isnan (ny));
  
  /* calcul de cada component */
  Rn1=localmod_calculation(p->w0,p->beta,p->sigma,p->St,p->Se,
			   p->Mf,p->Mx,p->h,p->rhos,0,0);
  Rn2=localmod_calculation(p->w0,p->beta,p->sigma,p->St,p->Se,
			   p->Mf,p->Mx,p->h,p->rhos,p->m,0);
  Rn3=localmod_calculation(p->w0,p->beta,p->sigma,p->St,p->Se,
			   p->Mf,p->Mx,p->h,p->rhos,0,p->U);
  
  /*calcul Rmax segons hipotesi lineal */
  assert (!isnan (p->mx));
  assert (!isnan (p->my));
  assert (!isnan (p->Ux));
  assert (!isnan (p->Uy));
  assert (!isnan (Rn2));
  assert (!isnan (Rn1));
  assert (!isnan (Rn3));
  dRn2x=(Rn2-Rn1)*p->mx;
  dRn2y=(Rn2-Rn1)*p->my;
  dRn3x=(Rn3-Rn1)*p->Ux;
  dRn3y=(Rn3-Rn1)*p->Uy;

  /* calcul n0 */
  assert (!isnan (dRn2x));
  assert (!isnan (dRn3x));
  assert (!isnan (dRn2y));
  assert (!isnan (dRn3y));
  
  assert (pow(dRn2x+dRn3x,2)+pow(dRn2y+dRn3y,2) >= 0);
  modul=sqrt(pow(dRn2x+dRn3x,2)+pow(dRn2y+dRn3y,2));
  assert (modul != 0);
  r->n0x=(dRn2x+dRn3x)/modul;
  r->n0y=(dRn2y+dRn3y)/modul;

  /* calcul Rmax */
  r->Rmaxx=Rn1*r->n0x+dRn2x+dRn3x;
  r->Rmaxy=Rn1*r->n0y+dRn2y+dRn3y;
}


