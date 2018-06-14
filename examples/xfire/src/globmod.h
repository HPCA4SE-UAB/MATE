/*
** ----------------------------------------------------------------------
**
** FILE:	GLOBAL MODEL 
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

#include "global.h"
#include "gisfuncs.h"
#include "vegetmod.h"
#include "windmod.h"
#include "factor1.h"
#include "factor2.h"

#include "comm_utils.h"

#include "simulator.h"

#ifndef _GLOBMODH_
#define _GLOBMODH_

/* Determinacio de l'area cremada */
void global_fireArea(pMAP*map, pFIRELINE* front);

/* Test de les hipotesis de quasi equilibri que s'han de cumplir */
TBOOL global_testeq(pMAP* map, pvegetModels* veget, pFIRELINE* front);

/* Test del front, esta al territori? */
TBOOL global_testFrontInLand(pMAP* map, pFIRELINE* front);

/* Interpolacio (per splines) de nous punts del front */
void global_newpoints(pMAP* map, pFIRELINE* front);

/* Obtenir els parametres local pel punt determinat del front */
void global_getLocalParams( pMAP* map, pvegetModels* veget, pwindPoints* wind,
			    pointLine* p, plocalParams plocal);

/* Kernel del model global */
void global_kernel( pMAP* map, pvegetModels* veget,
		    pFIRELINE* front, pwindPoints* wind,int acttime,int dtime);

/* Dynamic load balancing of load work of the slaves */
void global_arcSlavesPartition(comm_info_master* p, pMAP* map, 
			       pvegetModels* veget,pFIRELINE* front,
			       pwindPoints* wind);

/* results of the slave calculations */
void global_resultsSlave(comm_info_master* p,pFIRELINE* front,pMAP *map);

/* Model global */
TBOOL global_model(int iterIdx, comm_info_master* p, pMAP* map, pvegetModels* veget,
		    pFIRELINE* front, pwindPoints* wind,int acttime,int dtime, ptimeSim* t);


/* Model global */
/*TBOOL global_model(int iterIdx, comm_info_master *p, pMAP* map, pvegetModels* veget,
		    pFIRELINE* front, pwindPoints* wind,int acttime,int dtime, ptimeSim* t, 
		     SCHEDULE * schedData, schedule schedProc);*/

/*TBOOL global_model(int iterIdx, pvminfo_master *p, pMAP* map, pvegetModels* veget,
		    pFIRELINE* front, pwindPoints* wind,int acttime,int dtime, ptimeSim* t);*/

/* NaN checks */
void check_fireline (char const * region, pFIRELINE front);
void check_firelineEx (char const * region, pFIRELINE front, pMAP map);


#endif
