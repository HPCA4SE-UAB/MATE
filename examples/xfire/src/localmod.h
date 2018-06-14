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

#ifndef _LOCALMOD_H_
#define _LOCALMOD_H_

/* Calcul del model local nuclear */
double localmod_calculation(double w0, double beta, double sigma, double St,
			    double Se, double Mf, double Mx, double h, 
			    double rhos, double m, double U);
/* Model local nuclear invers per calcular vent equivalent */
double localmod_equivWind(double Rn0,double w0, double beta, double sigma, 
			  double St, double Se, double Mf, 
			  double Mx, double h, double rhos);

#endif /* _LOCALMOD_H_*/
