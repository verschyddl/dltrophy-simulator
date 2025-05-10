//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_UDPRECEIVER_H
#define DLTROPHY_SIMULATOR_UDPRECEIVER_H

#include <MinimalSocket/udp/UdpSocket.h>

class UdpReceiver {
private:
    MinimalSocket::Port port = 3413;
    MinimalSocket::udp::Udp<true> socket;

public:
    explicit UdpReceiver(int port);
    ~UdpReceiver() = default;
};

#endif //DLTROPHY_SIMULATOR_UDPRECEIVER_H
