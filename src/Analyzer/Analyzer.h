
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
// Analyzer.h
// 
// Analyzer - module that performs analysis of the events
// 
// Anna Sikora, UAB, 2002
//
//----------------------------------------------------------------------

#ifndef ANALYZER_H
#define ANALYZER_H

#include "Utils.h"
#include "EventList.h"
#include "FuncDefs.h"
#include "config.h"

/**
 * @class 	Analyzer
 * @brief	Analyzes events from a given set and determines if there are problems within them.
 * The class also has the capabilities to add or remove instrumentation from the code.
 *
 * @version 1.0b
 * @author Ania Sikora, 2002
 * @since 	1.0b
 */
class Analyzer
{
public:
	/**
	 * @brief Constructor
	 * @param list of events to be analyzed
	 */
	Analyzer (EventList & list)
		: _list (list), _receiveCount (0)
	{
		//Initialize (config)
	;
	}

	/**
	 *	@brief Analyzes an event and, if it finds a problem, makes tuning actions.
	 */
	void AnalyzeEvent ();

	/**
	 * @brief Requests to add instrumentation in the application so as to get information from it.
	 */
	void Instrument ();

	/**
	 * @brief Requests to remove instrumentation.
	 */
	void RemoveInstr ();

	/**
	 * @brief Requests to modify the application in order to improve its behavior.
	 */
	void Tune ();
	
private:

	//DEPRECATED
	//void Initialize (Config & config);

	/**
	 * @brief Sends a message to an AC in order to add instrumentation.
	 *
	 * @param machine	objective machine
	 * @param tid		identifier of the thread in which we will add the instruction
	 * @param eventId	identifier of the event
	 * @param fName		name of the function in which we will add the instruction
	 * @param place 	place in the function where we will add the instruction
	 * 		  			values are: instrUnknown, ipFuncEntry & ipFuncExit
	 * @param nAttrs	number of Attributes
	 * @param attrs		Attributes array
	 */
	void AddInstrReq (std::string & machine, int tid, int eventId, 
					std::string const & fName, InstrPlace place,
					int nAttrs, Attribute * attrs);

	/**
	 * @brief Sends a message to an AC in order to add instrumentation.
	 *
	 * @param machine	objective machine
	 * @param tid 		identifier of the thread in which we will remove the instruction
	 * @param eventId 	identifier of the associated event
	 * @param place		place in the function where the instruction will be removed
	 */
	void RemoveInstrReq (std::string & machine, int tid, int eventId, InstrPlace place);

	/**
	 * @brief Sends a message to an AC in order make tuning actions.
	 *
	 * @param machine	objective machine
	 * @param tid 		identifier of the thread in which we will remove the instruction
	 * @param variable	name of the variable for which we want to change the value
	 * @param value		new value for the variable
	 * @param brkpt		---
	 */
	void TuningReq (std::string & machine, int tid, std::string const & variable, 
					int value, Breakpoint * brkpt);
							
	/**
	 * @brief Gets the necessary elements to define a function: its id, number of parameters and
	 * their definition.
	 *
	 * @param name			name of the function.
	 * @param paramCount	its filled with the number of parameters of the function.
	 * @param funcId		its filled with the function id.
	 *
	 * @return				the definition of the parameters.
	 */
	std::string const & GetFuncDef (std::string const & name, int & paramCount,	int & funcId);

	/**
	 * @brief Gets the id of a function.
	 * @param name Function's name.
	 * @return	Function's id
	 */
	int GetFuncId (std::string const & name);

	/**
	 * @brief Analyzes a certain event so as to find problems.
	 */
	void Analyze ();

	/**
	 * @brief Determines if there is a problem or not in the application behavior.
	 * @return	0 if there were not any problems, 1 otherwise.
	 */
	int ProblemFound ();
	
private:
	EventList & _list;
	FuncDefs	_funcDefs;	// definition of the functions used to be traced
	int			_receiveCount;
};

#endif

