#include "ServerFinder.h"
#include "charlie_messages.hpp"

void ServerFinder::receive()
{
	network::NetworkStream stream;
	network::NetworkStreamReader reader(stream);
	network::IPAddress recv;
	if (sock.receive(recv, stream.buffer_, stream.length_)) {
		switch (reader.peek()) {
		case network::SEND_SERVERS: {
			network::MessageSendServers msg;
			if (!msg.read(reader)) {
				assert(!"Could not serialize message");
			}
			servers = msg.servers;
			serverIndex = 0;
			printf("Received servers from %s\n", recv.as_string());
		} break;
		}
	}
	else {
		auto err = network::Error::get_last();
		if (network::Error::is_critical(err)) {
			printf("ERR: Critical error on receiving servers from master server: %i\n", err);
		}
	}
}

void ServerFinder::send()
{
	uint8 msg = network::GET_SERVERS;
	if (!sock.send(masterAddr, &msg, 1)) {
		auto err = network::Error::get_last();
		if (network::Error::is_critical(err)) {
			printf("ERR: Critical error on sending GET_SERVERS to master server: %i\n", err);
		}
	}
}

ServerFinder::ServerFinder(network::Connection& _connection)
	: accumulator(SERVER_REGISTER_RATE - 0.5)
	, masterAddr(getMasterIP())
	, connection(&_connection)
{
	sock.open();
}

ServerFinder::~ServerFinder()
{
	sock.close();
}

void ServerFinder::enter()
{
	accumulator = SERVER_REGISTER_RATE - 0.5;
	servers.clear();
	servers.reserve(SERVER_RESERVE);
}

void ServerFinder::draw(charlie::Renderer& _rend)
{
	_rend.render_text({ 2, 1 }, Color::White, SERVER_FINDER_TEXT_SIZE, "Servers found:");
	for (uint8 i = 0; i < servers.size(); i++) {
		_rend.render_text({ 2, 3 + SERVER_FINDER_BOX_H * (i + 1) }, Color::White, SERVER_FINDER_TEXT_SIZE, servers[i].as_string());
	}
	if (servers.size() > 0) _rend.render_rectangle({ 1, 1 + SERVER_FINDER_BOX_H * (serverIndex + 1), SERVER_FINDER_BOX_W, SERVER_FINDER_BOX_H }, Color::White);
}

bool ServerFinder::update(const charlie::Time& _dt, const uint32 _tick)
{
	accumulator += _dt;
	receive();
	if (accumulator >= SERVER_REGISTER_RATE) {
		accumulator -= SERVER_REGISTER_RATE;
		send();
	}

	Time::sleep(0.002);
	return true;
}

void ServerFinder::onKeyDown(const Keyboard::Key key)
{
	switch (key) {
	case Keyboard::Key::W: {
		if (serverIndex > 0) serverIndex--;
	} break;
	case Keyboard::Key::S: {
		if (serverIndex < servers.size() - 1 && servers.size() != 0) serverIndex++;
	} break;
	default: break;
	}
	if (key == Keyboard::Key::Space && serverIndex < servers.size()) {
		if (connection->is_disconnected() || !connection->is_valid()) connection->connect(servers[serverIndex]);
	}
}

void ServerFinder::onPress(const charlie::Keyboard::Key key)
{
	if (key == Keyboard::Key::Space && serverIndex < servers.size()) {
		if (connection->is_disconnected() || !connection->is_valid()) connection->connect(servers[serverIndex]);
	}
}
