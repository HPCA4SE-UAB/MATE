
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

#ifndef __PTPMSG_H__
#define __PTPMSG_H__

// Local includes
#include "NetSer.h"
#include "PTPMsgHeader.h"

// C++ includes
#include <stdarg.h>
#include <string>
#include <string.h>

namespace Common {
	/**
	 * @class PTPMessage
	 * @brief Performance tuning protocol, represents a message interchanged
	 * between analyzer and tuner/tracer.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */

	// ------------------------------------------------------------
	class PTPMessage : public Serializable
	{
	public:

		/**
		 * @brief Constructor
		 */
		PTPMessage (){}

		/**
		 * @brief To be implemented by subclasses.
		 */
		virtual PTPMsgType GetType () const {}

		/**
		 * @brief Returns size of the data once serialized.
		 */
		int GetDataSize () const;

		/**
		 * @brief Destructor.
		 */
		virtual ~PTPMessage () {}
		//void Serialize (Serializer & out) const {}
		//void DeSerialize (DeSerializer & in) {}
	};

	/**
	 * @class TuningRequest
	 * @brief Encapsulates a tuning request from the analyzer.
	 *
	 * @extends PTPMessage
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */

	// ------------------------------------------------------------
	class TuningRequest : public PTPMessage
	{
	public:

		/**
		 * @brief Destructor.
		 */
		~TuningRequest ()
		{
			delete _brkpt;
		}

		/**
		 * @brief Returns the pid of the process associated with the message.
		 */
		int GetPid () const { return _pid; }

		/**
		 * @brief Returns breakpoint.
		 */
		Breakpoint * GetBreakpoint () const { return _brkpt; }

		/**
		 * @brief Sends the message.
		 */
		void Serialize (Serializer & out) const;

		/**
		 * @brief Receives the message.
		 */
		void DeSerialize (DeSerializer & in);

		/**
		 * @brief Clears breakpoint.
		 */
		void ClearBreakpoint ();

	protected:

		TuningRequest (int pid, Breakpoint const * brkpt = 0)
			: _pid (pid)
		{
			if (brkpt != 0)
				_brkpt = new Breakpoint (*brkpt);
			else
				_brkpt = 0;
		}

	protected:
		int 		 _pid;
		Breakpoint * _brkpt;
	};

	/**
	 * @class LoadLibraryRequest
	 * @brief Encapsulates a tuning request to load the specified shared
	 * library to a given application process.
	 *
	 * @extends TuningRequest
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */

	// ------------------------------------------------------------
	class LoadLibraryRequest : public TuningRequest
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param pid Id of the process where the library will be included, default 0.
		 * @param libPath Path of the library, default "".
		 */
		LoadLibraryRequest (int pid=0, std::string const & libPath=std::string())
			: TuningRequest (pid, 0), _libPath (libPath)
		{}

		/**
		 * @brief Returns type of message (PTPLoadLibrary).
		 */
		PTPMsgType GetType () const { return PTPLoadLibrary; }

		/**
		 * @brief Returns the path of the library to be loaded.
		 */
		std::string const & GetLibraryPath () const { return _libPath;}

		/**
		 * @brief Sends the message.
		 */
		void Serialize (Serializer & out) const;
	
		/**
		 * @brief Receives the message.
		 */
		void DeSerialize (DeSerializer & in);

	private:
		std::string _libPath;
	};
	
	/**
	 * @class SetVariableValueRequest
	 * @brief Encapsulates a tuning request to modify a value of a specified variable
	 * in a given application process.
	 *
	 * @extends TuningRequest
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */
	
	// ------------------------------------------------------------
	class SetVariableValueRequest : public TuningRequest
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param pid Id of the process where the variable will be modified, default 0.
		 * @param varName Name of the variable, default "".
		 * @param varValue New value to set, default empty AttributeValue object.
		 * @param brkpt Used for synchronization purposes, the actual tuning will be
		 * executed when the execution reaches the breakpoint, default 0.
		 */
		SetVariableValueRequest (int pid=0,
			std::string const & varName=std::string(),
			AttributeValue const & varValue = AttributeValue (),
			Breakpoint * brkpt=0)
			: TuningRequest (pid, brkpt), _varName (varName), _varValue (varValue)
		{}

		/**
		 * @brief Returns the value to set in a buffer format.
		 *
		 * If the type is known, it can be used using a cast, for example:
		 *
		 * @code
		   int foo = (int) VarRequest.GetValueBuffer();
		   @endcode
		 *
		 */
		void * GetValueBuffer () { return _varValue.GetValueBuffer (); }

		/**
		 * @brief Returns size of the variable new value.
		 */
		int GetValueSize () const { return _varValue.GetSize (); }

		/**
		 * @brief Returns a string containing the value of the variable.
		 */
		std::string GetValueString () const { return _varValue.ToString (); }

		/**
		 * @brief Returns type of message (PTPSetVariableValue).
		 */
		PTPMsgType GetType () const { return PTPSetVariableValue; }

		/**
		 * @brief Returns a string containing the variable name.
		 */
		std::string const & GetVariableName () const { return _varName; }

		/**
		 * @brief Sends the message.
		 */
		void Serialize (Serializer & out) const;

		/**
		 * @brief Receives the message.
		 */
		void DeSerialize (DeSerializer & in);

	private:
		std::string 	_varName;
		AttributeValue	_varValue;
	};

	/**
	 * @class ReplaceFunctionRequest
	 * @brief Encapsulates a tuning request to replace all calls to a function
	 * inside a process with calls to another function.
	 *
	 * @extends TuningRequest
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */

	// ------------------------------------------------------------
	class ReplaceFunctionRequest : public TuningRequest
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param pid Id of the process where the function will be replaced, default 0.
		 * @param oldFunc Name of the function to replace, default "".
		 * @param newFunc Name of the function to add, default "".
		 * @param brkpt Used for synchronization purposes, the actual tuning will be
		 * executed when the execution reaches the breakpoint, default 0.
		 */
		ReplaceFunctionRequest (int pid=0, std::string const & oldFunc=std::string(),
								std::string const & newFunc=std::string(),
								Breakpoint * brkpt=0)
			: TuningRequest (pid, brkpt), _oldFunc (oldFunc), _newFunc (newFunc)
		{}

		/**
		 * @brief Returns type of message (PTPReplaceFunction).
		 */
		PTPMsgType GetType () const { return PTPReplaceFunction; }

		/**
		 * @brief Returns a string containing the name of the function
		 * to replace.
		 */
		std::string const & GetOldFunction () const { return _oldFunc; }

		/**
		 * @brief Returns a string containing the name of the function
		 * added.
		 */
		std::string const & GetNewFunction () const { return _newFunc; }

		/**
		 * @brief Sends the message.
		 */
		void Serialize (Serializer & out) const;

		/**
		 * @brief Receives the message.
		 */
		void DeSerialize (DeSerializer & in);

	private:
		std::string 	_oldFunc;
		std::string 	_newFunc;
	};

	/**
	 * @class InsertFunctionCallRequest
	 * @brief Encapsulates a tuning request to insert a new function invocation
	 * code with a specified attributes at a given location in an application
	 * process.
	 *
	 * @extends TuningRequest
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */

	// ------------------------------------------------------------
	class InsertFunctionCallRequest : public TuningRequest
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param pid Id of the process where the call will be inserted, default 0.
		 * @param funcName Name of the function to call, default "".
		 * @param nAttrs Number of attributes the function has, default 0.
		 * @param attrs Attribute array, default 0.
		 * @param destFunc Function where the call will be inserted, default "".
		 * @param place Place where the call will be added, default ipFuncEntry.
		 * @param brkpt Used for synchronization purposes, the actual tuning will be
		 * executed when the execution reaches the breakpoint, default 0.
		 *
		 */
		InsertFunctionCallRequest (int pid=0, std::string const & funcName=std::string(),
					int nAttrs=0, Attribute * attrs=0,
					std::string const & destFunc=std::string(),
					InstrPlace place=ipFuncEntry, Breakpoint * brkpt=0)
			: TuningRequest (pid, brkpt), _funcName (funcName), _nAttrs (nAttrs),
			  _destFunc (destFunc), _place (place)
		{
			if (nAttrs == 0)
				_attrs = 0;
			else
			{
				_attrs = new Attribute [nAttrs];
				for (int i=0; i<nAttrs; i++)
					_attrs[i] = attrs[i];
			}
		}

		/**
		 * @brief Destructor.
		 */
		~InsertFunctionCallRequest ()
		{
			delete [] _attrs;
		}

		/**
		 * @brief Returns type of message (PTPInsertFuncCall).
		 */
		PTPMsgType GetType () const { return PTPInsertFuncCall; }

		/**
		 * @brief Returns name of the function to add.
		 */
		std::string const & GetFuncName () const { return _funcName; }

		/**
		 * @brief Returns number of attributes the function has.
		 */
		int GetAttrCount () const { return _nAttrs; }

		/**
		 * @brief Returns array of attributes.
		 */
		Attribute * GetAttributes () const { return _attrs; }

		/**
		 * @brief Returns name of the function where the call will be added.
		 */
		std::string const & GetDestFunc () const { return _destFunc; }

		/**
		 * @brief Returns the place where the call will be added.
		 */
		InstrPlace GetInstrPlace () const { return _place; }
		
		/**
		 * @brief Sends the message.
		 */
		void Serialize (Serializer & out) const;
	
		/**
		 * @brief Receives the message.
		 */
		void DeSerialize (DeSerializer & in);

	private:
		std::string 	_funcName;
		int 			_nAttrs;
		Attribute * 	_attrs;
		std::string 	_destFunc;
		InstrPlace 		_place;
	};
	
	/**
	 * @class OneTimeFunctionCallRequest
	 * @brief Encapsulates a tuning request to invoke one time a given
	 * function in a given application process.
	 *
	 * @extends TuningRequest
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */

	// ------------------------------------------------------------
	class OneTimeFunctionCallRequest : public TuningRequest
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param pid Id of the process where the call will be inserted, default 0.
		 * @param funcName Name of the function to call, default "".
		 * @param nAttrs Number of attributes the function has, default 0.
		 * @param attrs Attribute array, default 0.
		 * @param brkpt Used for synchronization purposes, the actual tuning will be
		 * executed when the execution reaches the breakpoint, default 0.
		 */
		OneTimeFunctionCallRequest (int pid=0, std::string const & funcName=std::string(),
				int nAttrs=0, Attribute const * attrs=0, Breakpoint const * brkpt=0)
			: TuningRequest (pid, brkpt), _funcName (funcName), _nAttrs (nAttrs)
		{
			if (nAttrs == 0)
				_attrs = 0;
			else
			{
				_attrs = new Attribute [nAttrs];
				for (int i=0; i<nAttrs; i++)
					_attrs[i] = attrs[i];
			}
		}

		/**
		 * @brief Destructor.
		 */
		~OneTimeFunctionCallRequest ()
		{
			delete [] _attrs;
		}

		/**
		 * @brief Returns type of message (PTPOneTimeFuncCall).
		 */
		PTPMsgType GetType () const { return PTPOneTimeFuncCall; }

		/**
		 * @brief Returns name of the function to be added.
		 */
		std::string const & GetFuncName () const { return _funcName; }

		/**
		 * @brief Returns number of attributes the function has.
		 */
		int GetAttrCount () const { return _nAttrs; }

		/**
		 * @brief Returns array of attributes.
		 */
		Attribute * GetAttributes () const { return _attrs; }

		/**
		 * @brief Sends the message.
		 */
		void Serialize (Serializer & out) const;

		/**
		 * @brief Gets the message.
		 */
		void DeSerialize (DeSerializer & in);

	private:
		std::string 	_funcName;
		int 			_nAttrs;
		Attribute * 	_attrs;
	};

	/**
	 * @class RemoveFunctionCallRequest
	 * @brief Encapsulates a tuning request to remove all calls to
	 * a given function from the given caller function.
	 *
	 * @extends TuningRequest
	 *
	 * @version 1.0
	 * @since 1.0
	 * @author Ania Sikora, 2003
	 */

	// ------------------------------------------------------------
	class RemoveFunctionCallRequest : public TuningRequest
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param pid Id of the process where the call will be removed, default 0.
		 * @param funcName Name of the function to remove, default "".
		 * @param callerFunc Name of the caller function, default "".
		 * @param brkpt Used for synchronization purposes, the actual tuning will be
		 * executed when the execution reaches the breakpoint, default 0.
		 */
		RemoveFunctionCallRequest (int pid=0, std::string const & funcName=std::string(),
				std::string const & callerFunc=string(), Breakpoint const * brkpt=0)
			: TuningRequest (pid, brkpt), _funcName (funcName), _callerFunc (callerFunc)
		{}

		/**
		 * @brief Returns type of message (PTPRemoveFuncCall).
		 */
		PTPMsgType GetType () const { return PTPRemoveFuncCall; }

		/**
		 * @brief Returns name of the function to be added.
		 */
		std::string const & GetFuncName () const { return _funcName; }
	
		/**
		 * @brief Returns the function caller name.
		 */
		std::string const & GetCallerFunc () const { return _callerFunc; }

		/**
		 * @brief Sends the message.
		 */
		void Serialize (Serializer & out) const;
	
		/**
		 * @brief Receives the message.
		 */
		void DeSerialize (DeSerializer & in);

	private:
		std::string 		 	_funcName;
		std::string 			_callerFunc;
	};
	
	/**
	 * @class FunctionParamChangeRequest
	 * @brief Encapsulates a tuning request to set the value of an input
	 * parameter of a given function in a given application process.
	 *
	 * This parameter value is modified before the function body is invoked.
	 * It's also possible to change the parameter value under condition, namely
	 * if the parameter has a value equal to requiredOldValue, only then its value
	 * is changed to a new one. If the requiredOldValue is zero, then the value of
	 * the parameter is changed unconditionally.
	 *
	 * @extends TuningRequest
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */

	// ------------------------------------------------------------
	class FunctionParamChangeRequest : public TuningRequest
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param pid Id of the process where the parameter will be changed, default 0.
		 * @param funcName Name of the function call to modify, default "".
		 * @param paramIdx Parameter index inside the attributes array, default 0.
		 * @param newValue New Value to set, default 0.
		 * @param requiredOldValue Current value the parameter should have to perform the
		 * tuning. If the value doesn't match this one, the tuning won't be performed. If
		 * this value is 0, the tuning will be performed without checking the old value,
		 * default 0.
		 * @param brkpt Used for synchronization purposes, the actual tuning will be
		 * executed when the execution reaches the breakpoint, default 0.
		 */
		FunctionParamChangeRequest (int pid=0, std::string const & funcName=std::string(),
				int paramIdx=0, int newValue=0, int * requiredOldValue=0, Breakpoint * brkpt=0)
			: TuningRequest (pid, brkpt), _funcName (funcName), _paramIdx (paramIdx), _newValue (newValue)
		{
			if (requiredOldValue != 0)
			{
				_reqOldValue = *requiredOldValue;
				_hasOldValue = 0x01;
			}
			else
			{
				_hasOldValue = 0x00;
				_reqOldValue = 0;
			}
		}
	
		/**
		 * @brief Returns type of message (PTPFuncParamChange).
		 */
		PTPMsgType GetType () const { return PTPFuncParamChange; }

		/**
		 * @brief Returns name of the function.
		 */
		std::string const & GetFuncName () const { return _funcName; }

		/**
		 * @brief Returns index of the parameter to change on the attributes array.
		 */
		int GetParamIdx () const { return _paramIdx; }

		/**
		 * @brief Returns the new value to replace on the function call.
		 */
		int GetNewValue () const { return _newValue; }

		/**
		 * @brief Returns the value the parameter should have for the tuning to be
		 * performed.
		 */
		int const * GetReqOldValue () const
		{ return (_hasOldValue==0x01 ? &_reqOldValue : 0); }

		/**
		 * @brief Sends the message.
		 */
		void Serialize (Serializer & out) const;
	
		/**
		 * @brief Receives the message.
		 */
		void DeSerialize (DeSerializer & in);

	private:
		std::string 			_funcName;
		int 			_paramIdx;
		int			 	_newValue;
		byte_t			_hasOldValue;
		int				_reqOldValue;
	};
	
	/**
	 * @class AddInstrRequest
	 * @brief Represents message sent when analyzer requests
	 * to add instrumentation.
	 *
	 * @extends PTPMessage
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */
	
	// ------------------------------------------------------------
	class AddInstrRequest : public PTPMessage
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param pid Id of the process where the instrumentation will be added,
		 * default 0.
		 * @param eventId Event id, default 0.
		 * @param funcName Name of the function to modify, default "".
		 * @param place Place where the instrumentation will be added, default ipFuncEntry.
		 * @param nAttrs Number of attributes the function has, default 0.
		 * @param attrs Attribute array, default 0.
		 */
		/*AddInstrRequest (int pid=0, int eventId=0, std::string const & funcName=std::string(),
						InstrPlace place=ipFuncEntry, int nAttrs=0, Attribute * attrs=0)
			: _pid (pid), _eventId (eventId), _funcName (funcName), _instrPlace (place),
				_nAttrs (nAttrs), _attrs (attrs)
		{
			if (nAttrs == 0)
				_attrs = 0;
			else
			{
				_attrs = new Attribute [nAttrs];
				for (int i=0; i<nAttrs; i++)
					_attrs[i] = attrs[i];
			}
		}*/

		AddInstrRequest (int pid=0, int eventId=0, std::string const & funcName=std::string(),
						InstrPlace place=ipFuncEntry, int nAttrs=0, Attribute * attrs=0, 
						int nPapi = 0, std::string * PapiMetrics = 0)
			: _pid (pid), _eventId (eventId), _funcName (funcName), _instrPlace (place),
				_nAttrs (nAttrs), _attrs (attrs), _nPapi (nPapi), _PapiMetrics(PapiMetrics)
		{
			if (nAttrs == 0)
				_attrs = 0;
			else
			{
				_attrs = new Attribute [nAttrs];
				for (int i=0; i<nAttrs; i++)
					_attrs[i] = attrs[i];
			}
			if(nPapi == 0){
				_PapiMetrics = 0;
			} else {
				_PapiMetrics = new string [nPapi];
				for (int i = 0; i < nPapi; i++)
					_PapiMetrics[i] = PapiMetrics[i];
			}
		}

		/**
		 * @brief Destructor.
		 */
		~AddInstrRequest ()
		{
			delete [] _attrs;
		}
	
		/**
		 * @brief Returns type of message (PTPAddInstr).
		 */
		PTPMsgType GetType () const { return PTPAddInstr;}

		/**
		 * @brief Returns the process id.
		 */
		int GetPid () const { return _pid; }

		/**
		 * @brief Returns the place where the instruction should be
		 * added.
		 */
		InstrPlace GetInstrPlace () const { return _instrPlace; }

		/**
		 * @brief Returns function name.
		 */
		std::string const & GetFunctionName () const { return _funcName; }

		/**
		 * @brief Returns the event id.
		 */
		int GetEventId () const	{ return _eventId; }

		/**
		 * @brief Returns array of attributes.
		 */
		Attribute * GetAttributes () const { return _attrs; }

		/**
		 * @brief Returns number of attributes the function has.
		 */
		int GetAttrsCount () const { return _nAttrs; }

		/**
		 * @brief Returns array of Metrics.
		 */
		std::string * GetMetrics () const { return _PapiMetrics; }

		/**
		 * @brief Returns number of metrics the function has.
		 */
		int GetMetricsCount () const { return _nPapi; }

		/**
		 * @brief Sends the message.
		 */
		void Serialize (Serializer & out) const;

		/**
		 * @brief Receives the message.
		 */
		void DeSerialize (DeSerializer & in);

	private:
		int				_pid;
		std::string 	_funcName;
		int 			_eventId;
		InstrPlace		_instrPlace;
		int 			_nAttrs;
		Attribute 	*	_attrs;
		int 			_nPapi;
  		std::string *   _PapiMetrics;

	};

	/**
	 * @class RemoveInstrRequest
	 * @brief Represents message sent when analyzer requests to remove
	 * instrumentation.
	 *
	 * @extends PTPMessage
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */

	// ------------------------------------------------------------
	class RemoveInstrRequest : public PTPMessage
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param pid Id of the process where the instrumentation will be removed,
		 * default 0.
		 * @param eventId Event id, default 0.
		 * @param place Place where the instrumentation will be removed, default
		 * ipFuncEntry.
		 */
		RemoveInstrRequest (int pid=0, int eventId=0, InstrPlace place=ipFuncEntry)
			: _pid (pid), _eventId (eventId), _instrPlace (place)
		{}

		/**
		 * @brief Returns type of message (PTPRemoveInstr).
		 */
		PTPMsgType GetType () const { return PTPRemoveInstr; }

		/**
		 * @brief Returns the process id.
		 */
		int GetPid () const { return _pid; }

		/**
		 * @brief Returns the event id.
		 */
		int GetEventId () const { return _eventId; }

		/**
		 * @brief Returns the place where the instruction should be
		 * removed.
		 */
		InstrPlace GetInstrPlace () const { return _instrPlace; }

		/**
		 * @brief Sends the message.
		 */
		void Serialize (Serializer & out) const;

		/**
		 * @brief Receives the message.
		 */
		void DeSerialize (DeSerializer & in);

	private:
		int				_pid;
		int 			_eventId;
		InstrPlace		_instrPlace;
	};

	/**
	 * @class StartAppRequest
	 * @brief Represents a request to start the application.
	 *
	 * @extends PTPMessage
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2003
	 */

	// ------------------------------------------------------------
	class StartAppRequest : public PTPMessage
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param appPath Application path, default 0.
		 * @param argc Argument count, default 0.
		 * @param argv Arguments array, default 0.
		 * @param analyzerHost Host of the analyzer, default 0.
		 */
		StartAppRequest (std::string const & appPath = std::string(),
				int argc = 0, char const ** argv = 0,
				std::string const & analyzerHost = std::string());

		/**
		 * @brief Destructor.
		 */
		~StartAppRequest ();

		/**
		 * @brief Returns type of message (PTPStartApp).
		 */
		PTPMsgType GetType () const { return PTPStartApp; }

		/**
		 * @brief Returns the application path.
		 */
		std::string const & GetAppPath () const { return _appPath; }
	
		/**
		 * @brief Returns the analyzer host name.
		 */
		std::string const & GetAnalyzerHost () const { return _analyzerHost; }
	
		/**
		 * @brief Returns a pointer to the array of arguments.
		 */
		char ** GetArgs () const { return _argv; }
	
		/**
		 * @brief Returns number of arguments given.
		 */
		int GetArgCount () const { return _argc; }

		/**
		 * @brief Sends the message.
		 */
		void Serialize (Serializer & out) const;

		/**
		 * @brief Receives the message.
		 */
		void DeSerialize (DeSerializer & in);
	
	private:
		std::string _appPath;
		int			_argc;
		char **		_argv;
		std::string _analyzerHost;
	};
}

#endif
