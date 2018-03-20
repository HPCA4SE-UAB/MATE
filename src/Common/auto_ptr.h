
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

#ifndef __AUTO_PTR_H__
#define __AUTO_PTR_H__

/**
 * @class myauto_ptr
 */

template <class X>
class myauto_ptr
{
private:
  X* ptr;
  mutable bool owns;
public:
  typedef X element_type;
  explicit myauto_ptr(X* p = 0) : ptr(p), owns(p) {}
  myauto_ptr(const myauto_ptr& a) : ptr(a.ptr), owns(a.owns)
  {
    a.owns = 0;
  }
  myauto_ptr& operator=(const myauto_ptr& a)
  {
    if (&a != this) {
      if (owns)
        delete ptr;
      owns = a.owns;
      ptr = a.ptr;
      a.owns = 0;
    }
  }
  ~myauto_ptr() 
  {
    if (owns)
      delete ptr;
  }

  X& operator*() const { return *ptr; }
  X* operator->() const { return ptr; }
  X* get() const { return ptr; }
  X* release () const { owns = false; return ptr; }
};

#endif
