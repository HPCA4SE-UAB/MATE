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
** 24-01-98		JJ		Creation
**
** ----------------------------------------------------------------------
*/

#ifndef _FACTOR1_H_
#define _FACTOR1_H_

/* Parametres pel calcul del model local */
typedef struct {
  double  w0;
  double beta;
  double sigma;
  double St,Se;
  double Mx,Mf;
  double h;
  double rhos;
  double m;
  double mx,my;
  double U;
  double Ux,Uy;
} localParams;
/* Apuntador a parametres del model local */
typedef localParams* plocalParams;

/* Resultat calculat pel modul de factor1 */
typedef struct {
  double Rmaxx;
  double Rmaxy;
  double n0x;
  double n0y;
} vectRmax;

typedef vectRmax* pvectRmax;

/* 
** Modul de calcul de Rmax 
**
** Es retorna Rmax i la normal n0 en l'estructura de tipus vectRmax
** Nota: p es passat per referencia per velocitat
*/
void factor1_calculation(plocalParams p,double nx,double ny,pvectRmax r);

#endif /* _FACTOR1_H_*/
