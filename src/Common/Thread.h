
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

#ifndef __THREAD_H__
#define __THREAD_H__

// Local includes
#include "sync.h"

// C++ includes
#include <pthread.h>

namespace Common {
	/**
	 * @class Thread
	 * @brief Posix thread.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class Thread
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param arg Pointer to the function to run.
		 * @param pArg Pointer to the function arguments.
		 *
		 * @throws SysException
		 */
		Thread (void* (*pFun) (void* arg), void* pArg);

		/**
		 * @brief Waits for termination of the thread.
		 * @throws SysException
		 *
		 */
		void WaitForDeath ();

		/**
		 * @brief Stops the thread execution.
		 */
		void Exit ();

	private:

		pthread_t		_handle;	// thread handle
		pthread_attr_t	_attrs;		// thread attributes

	};
}

#endif
