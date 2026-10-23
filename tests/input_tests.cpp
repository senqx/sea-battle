#include "../helpers/IPPortConfig.hpp"
#include <cassert>
#include <string>
int main() {
    for (const char* text : {"127.0.0.1", "0.0.0.0", "255.255.255.255"}) {
        std::string ip = text;
        assert(isValidIp(ip));
    }
    for (const char* text : {"", "1.2.3", "1.2.3.4.5", "1.2.3.4.5.6", ".1.2.3", "1..2.3", "256.0.0.1", "1.2.3.4.", "1234.0.0.1", "-1.2.3.4", "1.2.3.a"}) {
        std::string ip = text;
        assert(!isValidIp(ip));
    }
    for (int port : {1, 1024, 65535}) assert(isValidPort(port));
    for (int port : {-1, 0, 65536}) assert(!isValidPort(port));
}
