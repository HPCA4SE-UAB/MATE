/*
** ----------------------------------------------------------------------
**
** FILE:	Nuclear Local Model (Rothermel)
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

/*
** Aqui trobem la implementacio del model local nuclear, segons les
** Equacions de Rothermel, tal com apareixen a la tesi doctoral
**
** "Uma teoria sobre a propagaçao de frentes de fogos florestais de
** superficie" de Jorge Campos Andre, Coimbra 1996, pag. 168-170.
**
** Nota: Totes les constants del model estan en S.I.
*/

#include <math.h>
//SOLO PARA SOLARIS
//#include <ieeefp.h>
#include <assert.h>
#include "localmod.h"

/*
** Calcul del model local nuclear, basat en parametres de la vegetacio
** ambient i topografia.
** Els parametres d'entrada son els seguents:
**
** w0 = carrega del llit (massa seca/area del llit)[Kg/m2]
** betha = compactacio   []
** sigma = relacio superficie/volum de les particules [m-1]
** St = contingut mineral [%]
** Se = contingut mineral excepte silice [%]
** Mf = contingut de humitat [%]
** Mx = contingut de humitat d'extencio
** h = poder calorific [J/kg]
** rhos = massa/volum (densitat) [Kg/m3]
** m = decliu del terreny [%]
** U = velocitat del vent [m/s]
**
** NOTA: el calcul (CHANGE) no esta optimitzat, per a major claretat de
** lectura de les equacions que tenim que avaluar.
*/
double localmod_calculation(double w0, double beta, double sigma, double St,
			    double Se, double Mf, double Mx, double h,
			    double rhos, double m, double U)
{
  double qigv; /* calor donat a una unitat de volum: equilibri -> ignicio */
  double qigm; /* calor per unitat de massa per provocar ignicio de part. */
  double qigm0; /* valor qigm en condicions ideals */
  double epsi; /* numero de "aquecimiento efectivo" ?? */
  double rhob; /* massa volumica aparaent */

  double phis;
  double a,b,c;
  double betaopt;
  double phiw;
  double xi;
  double etas,etam;
  double A;
  double rpmax,rp,r;
  double wn,Ir0,Ip,R;

  assert (!isnan (w0));
  assert (!isnan (beta));
  assert (!isnan (sigma));
  assert (!isnan (St));
  assert (!isnan (Se));
  assert (!isnan (Mf));
  assert (!isnan (Mx));
  assert (!isnan (h));
  assert (!isnan (rhos));
  assert (!isnan (m));
  assert (!isnan (U));

  /* calcul de qigv */
  qigm0 = 581 + 2594*Mf;
  assert (sigma != 0);
  epsi = exp(-453.0/sigma);
  qigm = epsi * qigm0;
  rhob = beta * rhos;
  qigv = rhob * qigm;
  
  /* calcul de Ip */
  phis = 5.275*pow(beta,-0.30)*pow(m,2);
  a = 7.47*exp(-0.069*pow(sigma,0.55));
  b = 0.01330 * pow(sigma, 0.54);
  c = 0.715 * exp(-1.09*pow(10,-4)*sigma);
  betaopt = 8.858 * pow(sigma,-0.8189);
  assert (betaopt != 0);
  phiw= a * pow(197*U,b)*pow((beta/betaopt),-c);
  assert ((192+0.079*sigma) != 0);
  xi=exp((0.792+0.376*sqrt(sigma))*(0.1+beta)) / (192+0.079*sigma);
  
  etas = 0.174*pow(Se,-0.19);
  assert (Mx != 0);
  etam = 1-2.59*(Mf/Mx)+5.11*pow((Mf/Mx),2)-3.52*pow((Mf/Mx),3);
  assert (4.24*pow(sigma,0.1) != 0);
  A = 1/(4.24*pow(sigma,0.1)-7.27);
  assert ((2940+0.0594*pow(sigma,1.5)) != 0);
  rpmax = (0.0167*pow(sigma,1.5))/(2940+0.0594*pow(sigma,1.5));
  assert (betaopt != 0);
  rp = rpmax*pow(beta/betaopt,A)*exp(A*(1-(beta/betaopt)));
  r = etas*etam*rp;
  assert (1+St != 0);
  wn=w0/(1+St);
  Ir0=h*wn*r;

  Ip=(xi*Ir0)*(1+phis+phiw);

  /* calcul de R, velocitat de propagacio */
  assert (qigv != 0);
  R=Ip/qigv;

  return R;
}

/* 
   Model local nuclear invers per calcular vent equivalent 
   Partim de Rn0 ja calculat i els parametres local per obtenir 
   Ue -> Vent equivalent
*/
double localmod_equivWind(double Rn0,double w0, double beta, double sigma,
			  double St, double Se, double Mf, 
			  double Mx, double h, double rhos)
{
  double qigv; /* calor donat a una unitat de volum: equilibri -> ignicio */
  double qigm; /* calor per unitat de massa per provocar ignicio de part. */
  double qigm0; /* valor qigm en condicions ideals */
  double epsi; /* numero de "aquecimiento efectivo" ?? */
  double rhob; /* massa volumica aparaent */

  double a,b,c;
  double betaopt;
  double phiw;
  double xi;
  double etas,etam;
  double A;
  double rpmax,rp,r;
  double wn,Ir0,Ip0;
  double Ue;

  /* calcul de qigv */
  qigm0 = 581 + 2594*Mf;
  epsi = exp(-453.0/sigma);
  qigm = epsi * qigm0;
  rhob = beta * rhos;
  qigv = rhob * qigm;

  /* calcul de Ip0 */
  a = 7.47*exp(-0.069*pow(sigma,0.55));
  b = 0.01330 * pow(sigma, 0.54);
  c = 0.715 * exp(-1.09*pow(10,-4)*sigma);
  betaopt = 8.858 * pow(sigma,-0.8189);
  xi=exp((0.792+0.376*sqrt(sigma))*(0.1+beta)) / (192+0.079*sigma);
  
  etas = 0.174*pow(Se,-0.19);
  etam = 1-2.59*(Mf/Mx)+5.11*pow((Mf/Mx),2)-3.52*pow((Mf/Mx),3);
  A = 1/(4.24*pow(sigma,0.1)-7.27);

  rpmax = (0.0167*pow(sigma,1.5))/(2940+0.0594*pow(sigma,1.5));
  rp = rpmax*pow(beta/betaopt,A)*exp(A*(1-(beta/betaopt)));
  r = etas*etam*rp;

  wn=w0/(1+St);
  Ir0=h*wn*r;

  Ip0=xi*Ir0;

  /* Calcul de phiw */
  phiw=((Rn0*qigv)/Ip0)-1;

  /* Calcul del vent equivalent */
  Ue= (1.0/197.0)*pow(pow((beta/betaopt),c)*(phiw/a),1.0/b);
  
  return Ue;
}
