
#include "./udp.h"

using namespace Platform;
using namespace std;

UDPSocket::UDPSocket() {
    isServerSocket = true;
}

int UDPSocket::bind(int port, const struct sockaddr* addr) {
    isServerSocket = true;
}