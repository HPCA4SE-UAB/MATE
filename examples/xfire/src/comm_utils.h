/*
** ----------------------------------------------------------------------
**
** FILE:       General structures for PVM
**
**  (C-) Josep Jorba, 1997		C.A.O.S. - UAB
** 
** ----------------------------------------------------------------------
**
** Date			Who		What
** 19-07-98		JJ		Creation
**
** ----------------------------------------------------------------------
*/

/*
** Definicio de tipus globals utilitzats a diversos llocs
*/

#ifndef _COMM_UTILS_H_
#define _COMM_UTILS_H_

#define PACKET_ERROR  0   /* PACKET of error */
#define PACKET_INIT   1   /* PACKET for sincronization on a inicialization */
#define PACKET_INITOK 2   /* Slave init correct */
#define PACKET_ARCP   3   /* PACKET of data paoints */
#define PACKET_IDLE   4   /* PACKET for a idle state */
#define PACKET_ENDT   5   /* PACKET of a end task work */
#define PACKET_ENDTOK 6   /* End of task correct */
#define PACKET_VEGT   7   /* PACKET of vegetation models */
#define PACKET_ARCR   8   /* PACKET arc newpoints results */


/* master & slave info */
typedef struct {
  int mytid; /* Rank of master task */
  int nslaves; /* number of slaves controled by the master */
  int nutilslaves; /* number of current workers */
  int *stids; /* slave ranks */
} comm_info_master;

typedef struct {
  int mytid;	/* Slave rank */
  int mastertid;	/* Master rank */
} comm_info_slave;

void mpi_pack_init ();
void mpi_unpack_init ();
void mpi_pkint (int * pInts, int size, int);
void mpi_pkdouble (double * pDoubles, int size, int);
void mpi_upkint (int * pInts, int size, int);
void mpi_upkdouble (double * pDoubles, int size, int);
void * mpi_pack_buf ();
int mpi_pack_buf_size ();
int mpi_pack_buf_pos ();

#endif /* _COMM_UTILS_H_*/
  
