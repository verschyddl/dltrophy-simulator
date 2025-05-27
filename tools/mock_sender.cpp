//
// Created by qm210 on 10.05.2025.
//

#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <MinimalSocket/udp/UdpSocket.h>

struct Config {
    std::string host;
    int port;
    std::vector<std::vector<uint8_t>> messages;
    int delayMs;
    int repeats;
};

struct RGB

inline std::vector<uint8_t> warlsMessageFor(size_t nLeds, std::function<LED(size_t)> func) {
    std::vector<uint8_t> message{1, 255};
    for (unsigned int i = 0; i < nLeds; i++) {
        auto led = func(i);
        message.insert(message.end(), {
            i,
            led.r,
            led.g,
            led.b
        });
    }
    return message;
}

inline std::vector<uint8_t> drgbMessageFor(size_t nLeds, std::function<LED(size_t)> func) {
    std::vector<uint8_t> message{1, 255};
    for (size_t i = 0; i < nLeds; i++) {
        auto led = func(i);
        message.insert(message.end(), {led.r, led.g, led.b});
    }
    return message;
}

int main() {
    std::cout << "Mock Sender: Test UDP package sending." << std::endl;

    Config config{
            .host = "localhost",
            .port = 3413,
            .messages = {
                    {100, 7, 210},
                    {0, 210, 210},
                    {0, 0, 0}
            },
            .delayMs = 1000,
            .repeats = 5,
    };

    // some cases
//    config.message = {
//            warlsMessageFor(200, )
//    };

    const MinimalSocket::Address remote(config.host, config.port);
    MinimalSocket::udp::Udp<true> sender(
            MinimalSocket::ANY_PORT,
            MinimalSocket::AddressFamily::IP_V4
            );
    const auto delay = std::chrono::milliseconds(config.delayMs);

    if (!sender.open()) {
        std::cerr << "Failed to open UDP Sender." << std::endl;
        return 1;
    }

    bool is_first_message = true;
    for (std::size_t i = 0; i <= config.repeats; i++) {

        for (auto &message : config.messages) {

            if (!is_first_message) {
                std::this_thread::sleep_for(delay);
            } else {
                is_first_message = false;
            }

            std::string message_string(message.begin(), message.end());
            sender.sendTo(message_string, remote);

            std::cout << "Sent UDP to " << to_string(remote)
                      << ": \"" << message_string << "\""
                      << std::endl;
        }
    }

    std::cout << "Mock Sender Finished without errors." << std::endl;
    return 0;
}
