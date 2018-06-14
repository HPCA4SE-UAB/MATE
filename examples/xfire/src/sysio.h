/*
** ----------------------------------------------------------------------
**
** FILE:	SYSTEM I/O ROUTINES
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

#ifndef _SYSIOH_
#define _SYSIOH_

/* funcio sortia cap a fitxer del front */
void sysio_front(pFIRELINE* front,char* s);
/* Treure l'area cremada */
void sysio_fireArea(pMAP* map, char* s);

#endif
