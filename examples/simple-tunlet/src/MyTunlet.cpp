//----------------------------------------------------------------------
//
// MyTunlet.cpp
// 
// Alberto Olmo, UAB, 2018
//
//----------------------------------------------------------------------

#include "MyTunlet.h"
#include "Syslog.h"
#include "Utils.h"
#include <assert.h>
#include <math.h>
#include "Config.h"
#include <sstream>
#include <boost/assign/list_of.hpp>

std::map<std::string, EventsEnum> EventMap = \
	boost::assign::map_list_of("SetVariableValue", idSetVariableValue)("ReplaceFunction", idReplaceFunction);
	
MyTunlet::MyTunlet()
: _app (0){
	Syslog::Debug("[MT]: Created object MyTunlet");
}

MyTunlet::~MyTunlet() {
	Syslog::Debug("[MT]: Deleted object MyTunlet");
}

void MyTunlet::Initialize(Model::Application & app) {
	_app = &app;
	_cfg = ConfigHelper::ReadFromFile(DefConfigFile);

	Syslog::Debug("[MT]: MyTunlet initialized");
}

void MyTunlet::BeforeAppStart() {
	Syslog::Debug("[MT]: MyTunlet BeforeAppStart");
	assert(_app != 0);
	_app->SetTaskHandler(*this);
}

void MyTunlet::Destroy() {
	_app = 0;
	Syslog::Info ("[MT] Tunlet destroyed");
}

// Handles all incoming events
void MyTunlet::HandleEvent (EventRecord const & r) {
		Syslog::Debug("[MT Event] HandleEvent START");
		switch (r.GetEventId()) {
			case idReplaceFunction:
			{
				Syslog::Debug("[MT Event] idReplaceFunction");


				_app->GetMasterTask()->ReplaceFunction("instr_function", "hello_world", 0);

				break;
			}
			case idSetVariableValue:
			{
				Syslog::Debug("[MT Event] idSetVariableValue");

				double my_new_val;
				my_new_val = float(79);
			    AttributeValue value_f0;
			    value_f0.SetType(avtDouble);
				value_f0.doubleValue = my_new_val;
				
				Syslog::Info(" Executing tuning action, setting variable value_to_change=%F", my_new_val);
				_app->GetMasterTask()->SetVariableValue("my_val", value_f0, 0);

				break;
			}
	}
}

void MyTunlet::CreateEvent(Task & t)
{
	Syslog::Debug("[MT]CreateEvent(Task & t)");
	
	// The functions into tunlet.ini file are read
	std::string nameFunc,base = "function",func,option;

	bool exitsFunction =1, exitsAtribute;
	int iter=1,iterA;
	stringstream ss,numA;
	std::vector<Attribute> Attrs;
	Attribute *aux;
	InstrPlace entry;
	EventsEnum idEvent;

	while (exitsFunction)
	{
		Syslog::Debug("[MT]CreateEvent---while");
		ss << iter;
		nameFunc = base + ss.str();
		exitsFunction = _cfg.Contains ("Functions",nameFunc);
		if (exitsFunction)
		{
			func = _cfg.GetStringValue("Functions",nameFunc);
			iterA =1;
			exitsAtribute =1;
			while (exitsAtribute)
			{
				numA << iterA;
				exitsAtribute = _cfg.Contains(func,"source"+numA.str());
				if (exitsAtribute)
				{
					aux = new Attribute;
					option = _cfg.GetStringValue(func,"source"+numA.str());
					aux->source = (AttrSource) StringToEnum(option,1);
					option = _cfg.GetStringValue(func,"type"+numA.str());
					aux->type = (AttrValueType) StringToEnum(option,2);
					aux->id = _cfg.GetStringValue(func,"id"+numA.str());
					Attrs.push_back(*aux);
				}
				iterA ++;
				numA.str(std::string());
			}

			// Add start event
			entry = (InstrPlace) StringToEnum(_cfg.GetStringValue(func,"entry"), 3);
			idEvent = (EventsEnum) EventMap[_cfg.GetStringValue(func,"event")];
			Event startEvent(idEvent, func,entry);
			startEvent.SetAttribute(Attrs.size(), &Attrs[0]);
			startEvent.SetEventHandler(*this);
			
			// Insert event:
			t.AddEvent(startEvent);

			// Add end event
			Event endEvent(idEvent,func, ipFuncExit);
			endEvent.SetEventHandler(*this);
			t.AddEvent(endEvent);
				
			//endEvent.SetEventHandler(*this);
			//t.AddEvent(endEvent);
			Attrs.clear();
			Syslog::Debug("[MT] CreateEvent DONE");
		}
		iter ++;
		ss.str(std::string());
	}
    std::string dummyFunc = "MonitorSignal";
    Event dummyEvent(0,dummyFunc, ipFuncExit);
    dummyEvent.SetEventHandler(*this);
    t.AddEvent(dummyEvent);
}

void MyTunlet::TaskStarted(Task & t) {
	Syslog::Debug("[MT] TaskStarted");
	CreateEvent(t);
}

void MyTunlet::TaskTerminated (Task & t)
{
	Syslog::Debug("[MT] TaskTerminated");
}
