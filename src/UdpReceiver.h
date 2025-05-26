//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_UDPRECEIVER_H
#define DLTROPHY_SIMULATOR_UDPRECEIVER_H

#include <iostream>
#include <MinimalSocket/udp/UdpSocket.h>

struct RawMessage {
    std::vector<int> values;
    std::string source;
};

class UdpReceiver {
private:
    MinimalSocket::Port port = 3413;

    MinimalSocket::udp::Udp<false> socket;
    // <-- the "false" stands for "non-blocking"

    const std::size_t maxMessageSize = 1024;
    // <-- UDP message size as limited by WLED:
    // https://kno.wled.ge/interfaces/udp-realtime/

public:
    explicit UdpReceiver(int port)
    : port(port)
    {
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

    ~UdpReceiver() = default;

    std::optional<RawMessage> listen() {
        auto package = socket.receive(maxMessageSize);
        if (!package.has_value()) {
            return std::nullopt;
        }
        return RawMessage{
                .values = decodeIntegers(package.value()),
                .source = to_string(package.value().sender)
        };
    }

    static std::vector<int> decodeIntegers(MinimalSocket::ReceiveStringResult message) {
        std::vector<int> values;
        for (char c: message.received_message) {
            auto byteValue = static_cast<uint8_t>(c);
            auto intValue = static_cast<int>(byteValue);
            values.push_back(intValue);
        }
        return values;
    }
};

#endif //DLTROPHY_SIMULATOR_UDPRECEIVER_H
