
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
// ECPMsg.h
// 
// ECPMsgs - represent messages that are interchanged between 
//			dynlib and analyzer
//
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#include "ECPMsg.h"
#include <stdio.h>
#include "Syslog.h"
#include "EventMsgReader.h"

void EventMsgReader::DumpValues() {
	//Syslog::Debug("Dumping ECP event parameters:");
	//Syslog::Debug("ParamCount: %d", GetParamCount());

	for (int i = 0; i < GetParamCount(); ++i) {
		AttrValueType type = (AttrValueType)_deser.GetByte();
		switch (type) {
			case avtInteger:
			{
				int value = _deser.GetInt();
				//Syslog::Debug("Param: %d type:%d value: %d", i, type, value);
				break;
			}
			case avtShort:
			{
				short value = _deser.GetShort();
				//Syslog::Debug("Param: %d type:short value: %d", i, (int)value);
				break;
			}
			case avtFloat:
			{
				float value = (float)_deser.GetDouble();
				//Syslog::Debug("Param: %d type:float value: %f", i, value);
				break;
			}
			case avtDouble:
			{
				double value = _deser.GetDouble();
				//Syslog::Debug("Param: %d type:double value: %f", i, value);
				break;
			}
			case avtChar:
			{
				char value = _deser.GetChar();
				//Syslog::Debug("Param: %d type:char value: %c", i, value);
				break;
			}
			case avtString:
			{
				string value = _deser.GetString();
				//Syslog::Debug("Param: %d type:string value: %s", i, value.c_str());
				break;
			}
			default:
				Syslog::Debug("Unknown datatype");
		}//End Switch
	}//End for
}

