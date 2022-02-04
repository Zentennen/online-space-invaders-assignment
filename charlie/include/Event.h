#pragma once
#include "charlie_network.hpp"
#include "Config.h"
#include <memory>

namespace charlie {
	namespace network {
		struct NetworkStreamReader;
		struct NetworkStreamWriter;
	}
}

using namespace charlie;

struct NetEvent {
	uint8 type;
	id_t eventID;
	uint32 tick;
	NetEvent(const uint8 _type);
	NetEvent(const uint8 _type, const uint32& _time);
	virtual ~NetEvent();
	virtual bool read(network::NetworkStreamReader& reader) = 0;
	virtual bool write(network::NetworkStreamWriter& writer) = 0;
private:
	static id_t getID();
};