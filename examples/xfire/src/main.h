/*
** ----------------------------------------------------------------------
**
** FILE:		Main File Header (main.h)
**
**  (C-) Josep Jorba, 1997		C.A.O.S. - UAB
** 
** ----------------------------------------------------------------------
**
** Date			Who		What
** 23-11-97		JJ		Creation
**
** ----------------------------------------------------------------------
*/

/* Basicament s'inclouen les funcions utilitzades pel main */

#ifndef _MAIN_H_
#define _MAIN_H_

#include "global.h"
#include "globalvar.h"
#include "comm_utils.h"

//int    TheTotalWork = 0;

void models_setup(pMAP* map, pvegetModels* veget,
		  pFIRELINE* front, pwindPoints* wind, ptimeSim* stime,
		  char* landFileName, char* vegetFileName,
		  char* frontFileName, char* windFileName,char* timeFileName);
void models_unsetup(pMAP* map, pvegetModels* veget,
		    pFIRELINE* front, pwindPoints* wind, ptimeSim* stime);
TBOOL takeParams(int numParams, char** Params,
		 char* landname, char* vegetname,
		 char* frontname, char* windname, char* timename, 
		 char * stats_file,
		 char * scheduling_mode,		 
		 int * useLoad);


int masterMain (int, char**);
/* PVM Master inicialization */
void masterInit(comm_info_master* p);

/* End master, terminate slaves */
void masterEnd(comm_info_master* p);

/* send vegetation model to slave */
void send_veget(comm_info_master *p,pvegetModels *v);

#endif /* _MAIN_H_*/
