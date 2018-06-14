
#include "comm_utils.h"
#include <mpi.h>

#ifndef PACK_BUFFER_SIZE
#define PACK_BUFFER_SIZE 1024*1024*64	// 4MB
#endif

static char PACK_BUFFER [PACK_BUFFER_SIZE];
static int  PACK_BUFFER_POS = 0;

void mpi_pack_init ()
{
	PACK_BUFFER_POS = 0;
}

void mpi_unpack_init ()
{
	PACK_BUFFER_POS = 0;
}

void mpi_pkint (int * pInts, int size, int)
{
	MPI_Pack (pInts, size, MPI_INT, PACK_BUFFER, PACK_BUFFER_SIZE, 
		&PACK_BUFFER_POS, MPI_COMM_WORLD);
}

void mpi_pkdouble (double * pDoubles, int size, int)
{
	MPI_Pack (pDoubles, size, MPI_DOUBLE, PACK_BUFFER, PACK_BUFFER_SIZE, 
		&PACK_BUFFER_POS, MPI_COMM_WORLD);
}
	
void mpi_upkint (int * pInts, int size, int)
{
	MPI_Unpack (PACK_BUFFER, PACK_BUFFER_SIZE, &PACK_BUFFER_POS,
		pInts, size, MPI_INT, MPI_COMM_WORLD);
}

void mpi_upkdouble (double * pDoubles, int size, int)
{
	MPI_Unpack (PACK_BUFFER, PACK_BUFFER_SIZE, &PACK_BUFFER_POS,
		pDoubles, size, MPI_DOUBLE, MPI_COMM_WORLD);
}
	
void * mpi_pack_buf ()
{
	return PACK_BUFFER;
}

int mpi_pack_buf_size ()
{
	return PACK_BUFFER_SIZE;
}

int mpi_pack_buf_pos ()
{
	return PACK_BUFFER_POS;
}
