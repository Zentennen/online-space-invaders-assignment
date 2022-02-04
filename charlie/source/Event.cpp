#include "Event.h"

id_t NetEvent::getID()
{
	/*for (id_t i = 0; i < eventIDs.size(); i++) {
		if (!eventIDs[i]) {
			eventIDs[i] = true;
			return i;
		}
	}
	eventIDs.resize(eventIDs.size() + 100, false);
	return getID();*/
	static id_t counter = 0;
	return counter++;
}

NetEvent::NetEvent(const uint8 _type) : type(_type), tick(0), eventID(getID())
{
	
}

NetEvent::NetEvent(const uint8 _type, const uint32& _time) : type(_type), tick(_time), eventID(getID())
{
	
}

NetEvent::~NetEvent()
{
	
}

//void IEventManager::pushEvent(std::unique_ptr<Event>&& e)
//{
//	manager->push(std::move(e));
//}
