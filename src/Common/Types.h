
/**************************************************************************
*                    Universitat Autonoma de Barcelona,					  *
*              HPC4SE: http://grupsderecerca.uab.cat/hpca4se/             *
*                        Analysis and Tuning Group, 					  *
*					            2002 - 2018                  			  */
/**************************************************************************
*	  See the LICENSE.md file in the base directory for more details      *
*									-- 									  *
*	This file is part of MATE.											  *	
*																		  *
*	MATE is free software: you can redistribute it and/or modify		  *
*	it under the terms of the GNU General Public License as published by  *
*	the Free Software Foundation, either version 3 of the License, or     *
*	(at your option) any later version.									  *
*																		  *
*	MATE is distributed in the hope that it will be useful,				  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 		  *
*	GNU General Public License for more details.						  *
*																		  *
*	You should have received a copy of the GNU General Public License     *
*	along with MATE.  If not, see <http://www.gnu.org/licenses/>.         *
*																		  *
***************************************************************************/

// MATE
// Copyright (C) 2002-2008 Ania Sikora, UAB.

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef __TYPES_H__
#define __TYPES_H__

// C++ includes
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>

typedef signed char		byte_t;		// 8 bits
typedef unsigned short	char_t;		// 8 bits
typedef unsigned char	bool_t;		// 8 bits
typedef short			short_t;	// 16 bits	
//typedef long			int_t;		// 32 bits
typedef int			int_t;		// 32 bits
//typedef long long		long_t;		// 64 bits
typedef long			long_t;		// 64 bits

# if __WORDSIZE == 64 || (defined __FLT_EVAL_METHOD__ && __FLT_EVAL_METHOD__ == 0)
/* The x86-64 architecture computes values with the precission of the
used type. Similarly for -m32 -mfpmath=sse. */
typedef float float_t;	 /* `float' expressions are evaluated as `float'. */
typedef double double_t;	/* `double' expressions are evaluated
as `double'. */
# else
/* The ix87 FPUs evaluate all values in the 80 bit floating-point format
which is also available for the user as `long double'. Therefore we
define: */
typedef long double float_t;	/* `float' expressions are evaluated as
`long double'. */
typedef long double double_t;	/* `double' expressions are evaluated as
`long double'. */
# endif

inline void LONG2INTS (long_t l, int_t & high, int_t & low)
{
	unsigned long long ul = (unsigned long long)l;
	high = static_cast<int_t>((ul & 0xFFFFFFFF00000000LL) >> 32);
	low = static_cast<int_t> (ul & 0x00000000FFFFFFFFLL);
}

// bug fixed, 03.04.2004
inline void INTS2LONG (int_t high, int_t low, long_t & l)
{
	unsigned int H = static_cast <unsigned int> (high);
	unsigned int L = static_cast <unsigned int> (low);

	unsigned long long h64 = static_cast <unsigned long long> (H);
	h64 <<= 32; 
	unsigned long long l64 = static_cast <unsigned long long> (L);
	l = static_cast <long_t> ((h64 | l64));	
}	

/*
inline void INTS2LONG (int_t high, int_t low, long_t & l)
{
	l = (static_cast<long_t>(high) << 32) | low;
}
*/

inline short_t HTONS (short_t s)
{
	return htons (s);
}

inline short_t NTOHS (short_t s)
{
	return ntohs (s);
}

inline int_t HTONL (int_t i)
{
	return htonl (i);
}

inline int_t NTOHL (int_t i)
{
	return ntohl (i);
}

#endif
