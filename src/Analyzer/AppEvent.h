
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
// AppEvent.h
// 
// Abstract Application Model Classes
//
// Anna Sikora, UAB, 2004
//
//----------------------------------------------------------------------

#if !defined APPEVENT_H
#define APPEVENT_H

#include "auto_vector.h"
#include "Utils.h"
#include "ECPMsg.h"
#include "Exception.h"
#include <vector>
#include <map>
#include <string>

namespace Model
{
	//typedef auto_vector<Attribute> Attributes;
	class EventHandler; //Will be defined later

	/**
	 * @class Event
	 * @brief	Encapsulates information about the events that the target
	 * 			application generates.
	 * For each event holds identification information (id, name), the place
	 * where it is produced, its attributes (for example the parameters of a
	 * function) and a reference to a handler.
	 * As this is a model class the methods provided are for accessing and
	 * setting the members of the data structure.
	 */
	class Event {
		public:
			/**
			 * @brief Copy constructor.
			 * @param e		the event to copy
			 */
			Event (Event const & e);

			/**
			 * @brief Constructor
			 *
			 * @param id		unique identification number for the event
			 * @param funcName	name of the function which the event is associated to
			 * @param place		place of the function
			 */
			Event (int id, std::string const & funcName, InstrPlace place);

			/**
			 * @brief Destructor
			 */
			~Event ();

			/**
			 * @brief Globally unique event id getter.
			 * @return Event id
			 */
			int GetId () const { return _eventId; }

			/**
			 * @brief Name getter.
			 * @return Name of the function this event is associated to
			 */
			string GetFunctionName () const { return _funcName; }

			/**
			 * @brief Instruction place getter.
			 * @return Either the function entry or exit
			 */
			InstrPlace GetInstrPlace () const { return _instrPlace; }

			/**
			 * @brief Number of attributes getter.
			 * @return Number of event attributes
			 */
			int GetNumAttributes () const { return _nAttrs; }

			/**
			 * @brief Attributes getter.
			 * @return A collection of attributes to be recorded with this event
			 */
			Attribute * GetAttributes () const { return _attrs; }

			/**
			 * @brief Attributes setter.
			 *
			 * @param nAttrs	Number of attributes
			 * @param attrs		Collection of attributes to be recorded with this event
			 */
			void SetAttribute (int nAttrs, Attribute * attrs);


			/**
			 * @brief	Attributes setter.
			 *
			 * @param nAttrs	Number of attributes
			 * @param attrs		Collection of attributes to be recorded with this event
			 */
			void SetNEvents (int nEvents);

			/**
			 * @brief Installs a callback function that is called each time
			 * a record of this event is delivered.
			 *
			 * @param h Event handler
			 */
			void SetEventHandler (EventHandler & h);

			/**
			 * @brief Event handler getter
			 * @return Event handler
			 */
			EventHandler * GetEventHandler () { return _handler; }

			/**
			 * @brief Number of Papi metrics getter.
			 * @return Number of Papi metrics
			 */
			int GetNEvents () const { return _nEvents; }

			/**
			 * @brief Number of Papi metrics getter.
			 * @return Number of Papi metrics
			 */
			int GetNumPapiMetrics () const { return _nPapi; }


			std::string * GetMetrics () const { return _PapiMetrics; }

			void SetMetric (int nPapi, std::string * PapiMetrics);

		private:
			int				_eventId;
			std::string 	_funcName;
			InstrPlace 		_instrPlace;
			int 			_nEvents;
			int				_nAttrs;
			Attribute *  	_attrs;
			int 			_nPapi;
	  		std::string *   _PapiMetrics;
			EventHandler  * _handler;
	};//End class Event

	class Task; //Will be defined later

	/**
	 * @class EventRecord
	 * @brief Particular instance of the event abstraction.
	 * Holds information about the kind of event, the task that produced, the
	 * message sent and the values it contained.
	 * On the one hand it provides methods to get/set the information above, on
	 * the other hand, it provides methods to parse messages and get the
	 * information that they contain.
	 */
	class EventRecord {
		friend class Task;
		public:
			~EventRecord ();

			/**
			 * @brief Id getter.
			 * @returns Globally unique event id
			 */
			int GetEventId () const { return _event.GetId (); }

			/**
			 * @brief Associated event getter.
			 * @return Event object this record is associated to
			 */
			Event const & GetEvent () const { return _event; }

			/**
			 * @brief Time stamp getter.
			 * @return Time stamp that indicates when the event happened
			 */
			long_t GetTimestamp () const { return _msg.GetTimestamp (); }

			/**
			 * @brief Task getter.
			 * @return The task that generated this event
			 */
			Task & GetTask () const { return _task; }

			/**
			 * @brief Values getter.
			 * @return A collection of recorded attribute values
			 */
			AttributeValue * GetAttributeValues () const { return _values; }

			/**
			 * @brief Gets the i-th attribute from the list of values.
			 * @param index	Position of the attribute from which we want the value
			 *
			 * @return The recorded value for the i-th attribute
			 */
			AttributeValue const & GetAttributeValue (int index) const { return _values [index]; }

		protected:
			/**
			 * @brief Constructor
			 * @param e 	Event object this record is associated to
			 * @param t		Task object which produces the event
			 * @param msg	Message produced by the event
			 */
			EventRecord (Event const & e, Task & t, EventMsg const & msg);

			/**
			 * @brief Reads from the message and sets the value of the attributes depending on
			 * their type.
			 *
			 * @param msg	Reference to the msg to be read.
			 */
			void ParseAttrs (EventMsg const & msg);

		private:
			Event 	 const & _event;
			Task  	       & _task;
			EventMsg const & _msg;
			AttributeValue * _values;
	};//End class EventRecord
	
	/**
	 * @class EventHandler
	 * @brief Abstract class that holds a method to manage event records.
	 */
	class EventHandler {
		public:
			/**
			 * @brief Handles an event record (virtual).
			 * @param r Event record to be handled
			 */
			virtual void HandleEvent (EventRecord const & r) = 0;
	};//End class EventHandler
	
	/**
	 * @class Events
	 * @brief Encapsulates information to create and manage events lists.
	 * Uses a data structure based on a vector to keep data and a map to
	 * retrieve it.
	 * Provides methods to add, remove and find elements in the list.
	 */
	class Events {
		typedef std::pair<int, InstrPlace> Key;
		typedef std::map<Key, Event*> Map;
		
		public:
			/**
			 * @brief	Constructor
			 */
			Events () {}

			/**
			 * @brief Maps and adds an event to the events list.
			 * @param e		The event to be added
			 */
			void Add (Event const & e);

			/**
			 * @brief Removes an event from the events list.
			 *
			 * @param eventId	Unique Id of the event
			 * @param place		Instruction where the event is placed
			 * @return True if found &removed, false otherwise
			 */
			bool Remove (int eventId, InstrPlace place);

			/**
			 * @brief Searches for an event in the event list.
			 *
			 * @param eventId	Unique Id of the event.
			 * @param place		Instruction where the event is placed
			 *
			 * @return	A reference to the found event or NULL if not found
			 */
			Event * Find (int eventId, InstrPlace place);

			/**
			 * @brief Size getter.
			 * @return Number of events
			 */
			int Size () const;
		private:
			auto_vector<Event> 	_arr;
			Map					_map;
	};//End class Events
};//End namespace Model

#endif

