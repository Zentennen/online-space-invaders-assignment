#pragma once
#include "Event.h"

class ReliableEventQueue {
	struct EventEntry {
		DynamicArray<uint16> seqs;
		std::shared_ptr<NetEvent> ev;
		EventEntry(const std::shared_ptr<NetEvent>& e);
	};
	DynamicArray<EventEntry> events;
public:
	void push(const std::shared_ptr<NetEvent>& _event, const bool replace);
	void onAcknowledge(const uint16 _ack, const uint32 _bits);
	uint32 onSend(const uint16 _newSeq, network::NetworkStreamWriter& _writer);
};

class EventQueue {
	DynamicArray<std::shared_ptr<NetEvent>> events;
public:
	void push(const std::shared_ptr<NetEvent>& _ev) {
		events.push_back(_ev);
	}
	void transferEvents(DynamicArray<std::shared_ptr<NetEvent>>& _arr, const uint32 _tick);
};

class INetEventManager {
protected:
	static INetEventManager* manager;
	virtual void pushEvent(std::shared_ptr<NetEvent>&& e, const bool replace) = 0;
public:
	template<typename T>
	static void push(T&& e, bool replace = false) {
		manager->pushEvent(std::move(std::make_shared<T>(std::move(e))), replace);
	}
};