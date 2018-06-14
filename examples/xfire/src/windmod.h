/*
** ----------------------------------------------------------------------
**
** FILE:	WIND MODEL
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

#ifndef _WINDMOD_H_
#define _WINDMOD_H_

#include "gisfuncs.h"
/*
** Estructures que defineicen un conjunt d'estacions
*/
/* Definicio d'una estacio */
typedef struct {
  int stationCode;  /* Codi de la estacio */
  double x,y;        /* Coordenades UTM de la estacio */ 
  double direction;  /* Direccio del vent (en graus) */
  double speed;      /* Velocitat del vent en km/h */
} windStation;
/* Recull de les dades de totes les estacions */
typedef struct {
  int nStations; /* Numero de estacions */
  windStation* stations; /* Dades de les estacions */
} windPoints;

/* Definicio d'un tipus apuntador a windowPoints*/
typedef windPoints* pwindPoints;

/* Wind functions */
void wind_getWind(pwindPoints* w,pointLine* p,double *speed,double *direction);
void wind_setup(pwindPoints* w, char* windFile);
void wind_unsetup(pwindPoints* w);
void wind_statistics(pwindPoints* w);
void wind_readWindFile(pwindPoints* w, char* name);

#endif /* _WINDMOD_H_ */
