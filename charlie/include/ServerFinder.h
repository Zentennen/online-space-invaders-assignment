#pragma once
#include "AppState.h"
#include "Config.h"
#include "charlie_network.hpp"

class ServerFinder final : public AppState
{
	Time accumulator;
	uint8 serverIndex = 0;
	DynamicArray<network::IPAddress> servers;
	//network::Service* service;
	network::Connection* connection;
	network::IPAddress masterAddr;
	network::UDPSocket sock;
	void receive();
	void send();
public:
	ServerFinder(network::Connection& _connection);
	~ServerFinder();
	void enter() override;
	void draw(charlie::Renderer& _rend) override;
	bool update(const charlie::Time& _dt, const uint32 _tick) override;
	void onKeyDown(const Keyboard::Key key) override;
	void onPress(const charlie::Keyboard::Key key) override;
};

