
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
// AppEvent.cpp
// 
// Abstract Application Model Classes
//
// Anna Sikora, UAB, 2004
//
//----------------------------------------------------------------------

#include "AppEvent.h"
#include "AppTask.h"
#include "ECPMsg.h"//new
#include "EventMsgReader.h"//new
#include "Syslog.h"
#include <assert.h>

using namespace Model;

//Event =================================================================
Event::Event (Event const & e)
	: _eventId(e._eventId),
	  _funcName(e._funcName),
	  _instrPlace(e._instrPlace),
	  _nEvents(e._nEvents),
	  _nAttrs(e._nAttrs),
	  _attrs(e._attrs),
	  _nPapi(e._nPapi),
	  _PapiMetrics(e._PapiMetrics),
	  _handler(e._handler)
{
	if (_nAttrs == 0) {
		_attrs = 0;
	} else {
		_attrs = new Attribute [_nAttrs];
		for (int i = 0; i < _nAttrs; i++)
			_attrs[i] = e._attrs[i];
	}
	if(_nPapi == 0){
		_PapiMetrics = 0;
	} else {
		_PapiMetrics = new string [_nPapi];
		for (int i = 0; i < _nPapi; i++)
			_PapiMetrics[i] = e._PapiMetrics[i];
	}
}

Event::Event (int id, std::string const & funcName, InstrPlace place)
	: _eventId(id),
	  _funcName(funcName),
	  _instrPlace(place),
	  _nEvents(0),
	  _nAttrs(0),
	  _attrs(0),
	  _nPapi(0),
	  _PapiMetrics(0),
	  _handler(0)
{}

Event::~Event (){
	delete [] _attrs;
}

void Event::SetAttribute(int nAttrs, Attribute * attrs) {
	delete [] _attrs;
	_attrs = new Attribute[nAttrs];
	_nAttrs = nAttrs;
	for (int i = 0; i < _nAttrs; ++i)
		_attrs[i] = attrs[i];
}

void Event::SetNEvents(int nEvents) {
	_nEvents = nEvents;
}

void Event::SetEventHandler(EventHandler & h) {
	_handler = &h;
}


void Event::SetMetric (int nPapi, std::string * PapiMetrics){
	delete [] _PapiMetrics;
	_PapiMetrics = new std::string[nPapi];
	_nPapi = nPapi;
	for (int i = 0; i < _nPapi; ++i)
		_PapiMetrics[i] = PapiMetrics[i];
}
//Events =================================================================
void Events::Add(Event const & e) {
	auto_ptr<Event> pEvent (new Event(e));
	Key key (e.GetId(), e.GetInstrPlace());
	_map[key] = pEvent.get();
	_arr.push_back(pEvent);
}

bool Events::Remove(int eventId, InstrPlace place) {
	Key key(eventId, place);
	Map::iterator it = _map.find(key);
	if (it != _map.end()) {
		_map.erase(it); //remove from map
		//TODO: remove from _arr
		return true;
	}
	return false;	
}

Event * Events::Find(int eventId, InstrPlace place){
	Key key(eventId, place);
	Map::iterator it = _map.find(key);
	if (it != _map.end()) {
		return it->second;
	}
	return 0;
}

int Events::Size() const {
	return _map.size();
}

//EventRecord =================================================================
EventRecord::EventRecord(Event const & e, Task & t, EventMsg const & msg)
	: _event(e), _task(t), _msg(msg), _values(0)
{
	ParseAttrs(msg);
}


void EventRecord::ParseAttrs(Common::EventMsg const & msg) {
	//Syslog::Debug("ParseAttrs");
	int nAttrs = _event.GetNumAttributes();
	if (_event.GetInstrPlace() == 1)
		nAttrs = nAttrs + _event.GetNumPapiMetrics();
	_values = new AttributeValue[nAttrs];
	//Syslog::Debug("ParseAttrs nAttrs: %d instrPlace: %d", nAttrs, _event.GetInstrPlace());
	EventMsgReader reader(msg);
	for (int i = 0; i < nAttrs; ++i) {
		AttrValueType type = reader.GetAttrType();
		_values[i].SetType(type);// = reader.GetType();
		switch (type) {
			case avtInteger:
				_values[i].intValue = reader.GetIntValue();
				break;
			case avtFloat:
				_values[i].floatValue = reader.GetFloatValue();
				break;
			case avtDouble:
				_values[i].doubleValue = reader.GetDoubleValue();
				break;
			case avtString:
				_values[i].SetStrValue(reader.GetStringValue());
				break;
			case avtShort:
				_values[i].shortValue = reader.GetShortValue();
				break;
			case avtChar:
				_values[i].charValue = reader.GetCharValue();
				break;
			default:
				throw Exception ("Unsupported message type");
		}//End switch

	}//End for
}

EventRecord::~EventRecord() {
	delete [] _values;
}

