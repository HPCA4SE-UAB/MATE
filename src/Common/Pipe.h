
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

#ifndef __PIPE_H__
#define __PIPE_H__

// C++ includes
#include <unistd.h>
#include <iostream>
     
using namespace std;    

namespace Common {
	/**
	 * @class Pipe
	 * @brief Element used to join output and input from two processes.
	 *
	 * A pair of channels that implements a unidirectional pipe.
	 *
	 * A pipe consists of a pair of channels: A writable sink channel and a
	 * readable source channel. Once some bytes are written to the sink channel
	 * these can be read from source channel in the exact order in which they were written.
	 *
	 * Whether or not a thread writing bytes to a pipe will block until another
	 * thread reads those bytes, or some previously-written bytes, from the pipe
	 * is system-dependent and therefore unspecified. Many pipe implementations
	 * will buffer up to a certain number of bytes between the sink and source channels,
	 * but such buffering should not be assumed.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2001
	 */

	// ------------------------------------------------------------
	class Pipe
	{
		enum Descriptor { fdRead = 0, fdWrite = 1 };

	public:

		/**
		 * @brief Constructor.
		 * @throws SysException
		 */
		Pipe ();

		/**
		 * @brief Destructor.
		 */
		~Pipe ();

		/**
		 * @brief Returns whether the read end is open or not.
		 */
		bool IsReadOpen () const
		{
			return _fds [fdRead] != -1;
		}

		/**
		 * @brief Returns whether the write end is open or not.
		 */
		bool IsWriteOpen () const
		{
			return _fds [fdWrite] != -1;
		}

		/**
		 * @brief Returns read file descriptor.
		 */
		int GetRead () const
		{
			return _fds [fdRead];
		}

		/**
		 * @brief Returns write file descriptor.
		 */
		int GetWrite () const
		{
			return _fds [fdWrite];
		}

		/**
		 * @brief Closes the read end.
		 */
		void CloseRead ()
		{
			Close (fdRead);
		}

		/**
		 * @brief Closes the write end.
		 */
		void CloseWrite ()
		{
			Close (fdWrite);
		}

		/**
		 * @brief Reads from the read end and stores the content on the buffer.
		 * @throws SysException
		 */
		int Read (char * buf, int bufSize);

		/**
		 * @brief Writes the content of the buffer on the write end.
		 * @throws SysException
		 */
		int Write (char const * buf, int bufSize);

	private:

		void Close (Descriptor d);

		int _fds [2];  		// pipe descriptors
	};
}

#endif
