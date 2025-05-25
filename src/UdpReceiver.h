//
// Created by qm210 on 10.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_UDPRECEIVER_H
#define DLTROPHY_SIMULATOR_UDPRECEIVER_H

#include <thread>
#include <queue>
#include <condition_variable>
#include <MinimalSocket/udp/UdpSocket.h>

struct Message {
    std::vector<int> values;
    std::string source;
};

class UdpReceiver {
private:
    MinimalSocket::Port port = 3413;

    MinimalSocket::udp::Udp<false> socket;
    // <-- the "false" stands for "non-blocking"
    const std::size_t max_message_size = 1024;
    // <-- UDP message size as limited by WLED:
    // https://kno.wled.ge/interfaces/udp-realtime/

public:
    explicit UdpReceiver(int port);
    ~UdpReceiver() = default;

    std::optional<Message> listen() {
        auto package = socket.receive(max_message_size);
        if (!package.has_value()) {
            return std::nullopt;
        }
        return Message{
            .values = decodeValues(package.value()),
            .source = to_string(package.value().sender)
        };
    }

    static std::vector<int> decodeValues(MinimalSocket::ReceiveStringResult message) {
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
