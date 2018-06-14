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
** 24-01-98		JJ		Creation
**
** ----------------------------------------------------------------------
*/

#ifndef _FACTOR2H_
#define _FACTOR2H_

#include "factor1.h"

/* 
** Modul de calcul de l'efecte del angle entre n i n0
**
** Es retorna fn(ang)
** 
*/
double func_F(double phi, double u); /* calcul de doble el.lipse */
void factor2_calculation(plocalParams p,double Rn0,double angle,double* fn);

#endif
