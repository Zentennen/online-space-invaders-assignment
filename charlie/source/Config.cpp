#include "Config.h"
#include <fstream>
#include "charlie_network.hpp"

network::IPAddress getMasterIP() {
    static std::string ip = "";
    if (ip == "") {
        std::ifstream stream;
        while (!stream.is_open()) {
            stream.open("../master.txt");
        }
        assert(getline(stream, ip));
        stream.close();
    }
    return network::IPAddress(ip);
}