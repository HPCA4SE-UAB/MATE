
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
// SnippetMaker.h
// 
// SnippetMaker - class that creates snippets
//
// Anna Sikora, UAB, 2010
//
//----------------------------------------------------------------------

#ifndef SNIPPETMAKER_H
#define SNIPPETMAKER_H

#include "di.h"
#include "Utils.h"
#include "auto_vector.h"
#include <string>

/**
 * @class SnippetMaker
 *
 * @brief Prepares the snippets to be inserted into the processes.
 *
 * @version 1.0
 * @since 1.0
 * @author Ania Sikora, 2002
 */
class SnippetMaker
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param process Dyninst process to be modified.
	 * @param image Dyninst image of the process to be modified.
	 */
	SnippetMaker (DiProcess & process, DiImage & image) 
		: _process (process), _image (image)
	{}
	/**
	 * @brief Creates and inserts a snippet into the running process.
	 *
	 * @param eventId Identifier of the event.
	 * @param funcName Name of the function to be modified.
	 * @param instrPlace Place in the function where the snippet will be inserted.
	 * @param nAttrs Number of attributes.
	 * @param attrs Array of attributes.
	 *
	 * @return Handle for the prepared snippet.
	 */
	BPatchSnippetHandle * MakeEventSnippet (int eventId, 
								std::string const & funcName, 
								InstrPlace instrPlace, 
								/*auto_vector<Attribute> & attrs*/
								int nAttrs, 
								Attribute * attrs,
								int nPapi,
								std::string * PapiMetrics);
private:

	/**
	 * @brief Inserts the prepared snippets into the correct points of the process.
	 *
	 * @param funcName Name of the function to be modified.
	 * @param instrPlace Selects the location of the function where the
	 * snippet will be added.
	 * @param snippet Handler for the Dyninst snippet to be inserted.
	 *
	 * @return A handler for the added snippet or 0 if the insertion failed.
	 */
	BPatchSnippetHandle * InsertSnippet (std::string const & funcName,
		InstrPlace instrPlace, BPatch_snippet & snippet);

	/**
	 * @brief Gets the correct name to use to add the different types of parameters.
	 * @param attr Pointer to the attribute to be added.
	 */
	std::string GetAddParamName (Attribute const & attr);

	/**
	 * @brief Creates snippets for the different types of attributes.
	 * @param attr Pointer to the attribute to be added.
	 * @return Dyninst snippet that contains the data of the attribute to be added.
	 */
	BPatch_snippet * MakeSourceSnippet (Attribute const & attr);
					
private:
	DiProcess & _process;
	DiImage	 & _image;		// represents mutatee program image
};

#endif
