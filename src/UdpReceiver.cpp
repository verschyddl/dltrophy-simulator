//
// Created by qm210 on 10.05.2025.
//

#include <iostream>
#include "UdpReceiver.h"

UdpReceiver::UdpReceiver(int port)
: port(port) {

    socket = MinimalSocket::udp::Udp<false>(
            port,
            MinimalSocket::AddressFamily::IP_V4
            );

    if (!socket.open()) {
        throw std::runtime_error(std::format(
            "Socket cannot listen under port {0}, is it already in use?",
            port
        ));
    }

}
