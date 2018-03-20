
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

//----------------------------------------------------------------------
//
// MsgHeader.cpp
// 
// Header - represent header of a message
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------
	
#include "PTPMsgHeader.h"

void PTPMsgHeader::Serialize (Serializer & out) const
{
	/*printf ("HEADER: Magic: %d, Version: %d, Type: %d ", _magic, _version, _type);
	printf ("DataSize: %d, HeaderSize: %d\n", _dataSize, _headerSize);*/
	out.PutInt (_magic);
	out.PutInt (_version);
	out.PutInt (_type);
	out.PutInt (_dataSize);
	out.PutInt (_headerSize);
}
	
void PTPMsgHeader::DeSerialize (DeSerializer & in)
{
	_magic = in.GetInt ();
	// if (magic != ECP_MAGIC)
	//		throw "Invalid ECP magic number";
	_version = in.GetInt ();
	// check
	_type = (PTPMsgType) in.GetInt ();
	_dataSize = in.GetInt ();
	_headerSize = in.GetInt ();
	// check
}

