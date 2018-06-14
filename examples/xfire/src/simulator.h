/*
** ----------------------------------------------------------------------
**
** FILE:	Simulator Routines 
**
**  (C-) Josep Jorba, 1998		C.A.O.S. - UAB
** 
** ----------------------------------------------------------------------
**
** Date			Who		What
** 04-02-98		JJ		Creation
**
** ----------------------------------------------------------------------
*/

#include "gisfuncs.h"
#include "vegetmod.h"
#include "windmod.h"
#include "comm_utils.h"

#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

/* estructura per guardar els parametres temporals de la simulacio */
typedef struct {
  int hinit,minit;
  int hend,mend;
  int hinc,minc;
} timeSim;

/* tipus apuntador */
typedef timeSim* ptimeSim;

void simulator_setup(ptimeSim* t, char* timeFile);
void simulator_unsetup(ptimeSim* t);
void simulator_statistics(ptimeSim* t);
void simulator_readTimeFile(ptimeSim* t, char* name);
void simulator_loop(comm_info_master* p,ptimeSim* t, pMAP* map, 
		    pvegetModels* veget, pFIRELINE* front, pwindPoints* wind);
#endif /* _SIMULATOR_H_ */
