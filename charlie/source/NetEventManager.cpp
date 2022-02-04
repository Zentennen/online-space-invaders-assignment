#include "NetEventManager.h"
#include "charlie_messages.hpp"
INetEventManager* INetEventManager::manager;

ReliableEventQueue::EventEntry::EventEntry(const std::shared_ptr<NetEvent>& e) :
	ev(e)
{

}

void ReliableEventQueue::push(const std::shared_ptr<NetEvent>& _event, const bool replace) {
	if (!replace) {
		events.push_back(EventEntry(_event));
	}
	else {
		for (auto& entry : events) {
			if (entry.ev->type == _event->type) {
				entry.ev = _event;
				return;
			}
		}
		events.push_back(EventEntry(_event));
	}
}

void ReliableEventQueue::onAcknowledge(const uint16 _ack, const uint32 _bits)
{
	for (uint16 i = 0; i < 32; i++) {
		uint32 bitshift = (1 << (31 - i));
		if ((_bits & bitshift) != bitshift) continue;
		for (auto it = events.begin(); it != events.end();) {
			bool found = false;
			for (auto&& seq : it->seqs) {
 				if (seq == _ack - i) {
					it = events.erase(it);
					found = true;
					break;
				}
			}
			if (!found) ++it;
		}
	}
}

uint32 ReliableEventQueue::onSend(const uint16 _newSeq, network::NetworkStreamWriter& _writer)
{
	uint32 messages = 0;
	for (auto&& entry : events) {
		entry.seqs.push_back(_newSeq);
		entry.ev->write(_writer);
		++messages;
	}
	return messages;
}

void EventQueue::transferEvents(DynamicArray<std::shared_ptr<NetEvent>>& _arr, const uint32 _tick)
{
	for (uint16 i = 0; i < events.size(); i++) {
		if (events[i]->tick <= _tick) {
			_arr.push_back(events[i]);
			std::swap(events[i], events.back());
			events.pop_back();
		}
	}
}
