#pragma once
#include "charlie_network.hpp"
#include "charlie_messages.hpp"
#include "Config.h"
#include <fstream>
#include <WinSock2.h>

using namespace charlie;
using namespace network;

struct ServerData {
    IPAddress addr;
    Time timeSinceRegister;
};

void registerServer(const IPAddress& addr, DynamicArray<ServerData>& servers) {
    for (auto&& server : servers) {
        if (server.addr == addr) {
            printf("Server response: %s\n", addr.as_string());
            server.timeSinceRegister = Time(0.0);
            return;
        }
    }
    servers.push_back({ addr, Time(0.0) });
    printf("Server registered: %s\n", addr.as_string());
}

void unregisterServer(const IPAddress& addr, DynamicArray<ServerData>& servers) {
    for (auto it = servers.begin(); it != servers.end(); ++it) {
        if (it->addr == addr) {
            printf("Server unregistered: %s\n", addr.as_string());
            servers.erase(it);
            return;
        }
    }
}

void init(UDPSocket& sock) {
    WSADATA data = {};
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        printf("ERROR: WSAStartup: %i\n", WSAGetLastError());
    }
    IPAddress addr = getMasterIP();
    if (!sock.open(addr)) {
        int error_code = Error::get_last();
        printf("ERROR: %i (failed to open socket)\n", error_code);
    }
    if (!sock.is_valid()) {
        assert(!"Invalid socket");
    }
    printf("Master server opened: %s\n", addr.as_string());
}

int main()
{
    UDPSocket sock;
    init(sock);
    DynamicArray<ServerData> servers;
    servers.reserve(SERVER_RESERVE);

    Time last = Time::now();
    Time print = Time::now();
    while (true) {
        int32 receive_limiter = 20;
        IPAddress address;
        NetworkStream stream;

        Time current = Time::now();
        for (auto&& server : servers) {
            server.timeSinceRegister += current - last;
            if (server.timeSinceRegister >= SERVER_TIMEOUT) {
                printf("Server timed out: %s\n", server.addr.as_string());
                std::swap(server, servers.back());
                servers.pop_back();
            }
        }

        while (receive_limiter >= 0 && sock.receive(address, stream.buffer_, stream.length_)) {
            NetworkStreamReader reader(stream);
            switch (reader.peek()) {
            case REGISTER: {
                registerServer(address, servers);
                break;
            }
            case GET_SERVERS: {
                NetworkStream str;
                NetworkStreamWriter writer(str);
                DynamicArray<IPAddress> addrs;
                addrs.reserve(servers.size());
                for (auto&& server : servers) {
                    addrs.push_back(server.addr);
                }
                MessageSendServers message(addrs);
                message.write(writer);
                sock.send(address, str.buffer_, str.length_);
                printf("Request from: %s\n", address.as_string());
                break;
            }
            case UNREGISTER: {
                unregisterServer(address, servers);
                break;
            }
            default:
                printf("WRN: Invalid packet type: %i\n", (int32)reader.peek());
                break;
            }

            receive_limiter--;
            stream.reset();
        }

        if (current - print > SERVER_TIMEOUT) {
            print = current;
            printf("\nRegistered Servers:\n");
            for (auto&& server : servers) {
                printf("%s\n", server.addr.as_string());
            }
        }

        last = current;
        Time::sleep(0.001);
    }
}