#pragma once
#include "charlie.hpp"

struct NetEvent;

namespace charlie {
	namespace network {
		struct NetworkStreamReader;
		struct NetworkStreamWrtier;
	}
}

class AppState
{
public:
	AppState();
	virtual ~AppState();
	virtual void enter();
	virtual void exit();
	virtual void draw(charlie::Renderer& _rend) = 0;
	virtual bool update(const charlie::Time& _dt, const charlie::uint32 _tick) = 0;
	virtual void onKeyDown(const charlie::Keyboard::Key key);
	virtual void onPress(const charlie::Keyboard::Key key);
	virtual void handleEvent(NetEvent* e);
};