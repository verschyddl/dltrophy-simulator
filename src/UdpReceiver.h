//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_UDPRECEIVER_H
#define DLTROPHY_SIMULATOR_UDPRECEIVER_H

#include <MinimalSocket/udp/UdpSocket.h>

class UdpReceiver {
private:
    MinimalSocket::Port port = 3413;

    // the boolean "false" here means "non-blocking":
    MinimalSocket::udp::Udp<false> socket;

    // UDP message size as limited by WLED:
    // https://kno.wled.ge/interfaces/udp-realtime/
    const std::size_t max_message_size = 1024;

public:
    explicit UdpReceiver(int port);
    ~UdpReceiver() = default;

    std::optional<MinimalSocket::ReceiveStringResult> listen() {
        return socket.receive(max_message_size);
    }
};

#endif //DLTROPHY_SIMULATOR_UDPRECEIVER_H
