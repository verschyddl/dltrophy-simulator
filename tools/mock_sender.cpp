//
// Created by qm210 on 10.05.2025.
//

#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <ranges>
#include <algorithm>
#include <MinimalSocket/udp/UdpSocket.h>
#include <cmath>
#include "messages.h"

struct Message {
    std::vector<uint8_t> values;
    int delayMs = 0;
};

struct Config {
    std::string host;
    int port;
    std::vector<Message> messages;
    int repeats;
    bool debug;
};

std::vector<Message> createPattern(const std::string& patternName, bool useDrgb = false) {
    std::vector<Message> pattern{};

    auto allLeds = rangeOfAll();
    auto logo = rangeOfLogo();
    auto base = rangeOfBase();
    auto singles = rangeOfSingleLeds();

    auto messageCreator =
            useDrgb ? createDRGB : createWARLS;

    if (patternName == "logoblink") {
        int delay = 200;
        pattern.push_back({
                                  .values = messageCreator(singles, [](int x) {
                                      return RGB{100, 255, 0};
                                  }),
                                  .delayMs = 0
                          });
        pattern.push_back({
                                  .values = messageCreator(base, [](int x) {
                                      return RGB{0, 255, 200};
                                  }),
                                  .delayMs = 0
                          });

        for (int i = 0; i < 25; i++) {
            pattern.push_back({
                                      .values = messageCreator(logo, [i](int x) {
                                          return RGB(10 * i, 0, 255);
                                      }),
                                      .delayMs = delay
                              });
            pattern.push_back({
                                      .values = messageCreator(logo, [](int x) {
                                          return RGB();
                                      }),
                                      .delayMs = delay
                              });
        }
        for (uint8_t i = 0; i < 25; i++) {
            pattern.push_back({
                                      .values = messageCreator(logo, [i](int x) {
                                          return RGB(255 - 10 * i, 0, 255);
                                      }),
                                      .delayMs = delay
                              });
            pattern.push_back({
                                      .values = messageCreator(logo, [](int x) {
                                          return RGB();
                                      }),
                                      .delayMs = delay
                              });
        }
    }
    else if (patternName == "lauflichter") {
        auto delay = 100;
        for (int i=0; i < 2 * 64; i++) {
            float position = fmod(0.5 * static_cast<float>(i), static_cast<float>(N_BASE));
            pattern.push_back({
                .values = messageCreator(base, [position](int index) {
                    float val;
                    for (int edge = 0; edge < 4; edge++) {
                        float i = float((index - FIRST_BASE_INDEX + edge % N_BASE));
                        float edgeValue = i > position
                                    ? 0.
                                    : exp(1.2 * static_cast<float>(i - position)) * 255.;
                        val = std::max(val, edgeValue);
                    }
                    auto result = RGB(0, 0.4 * val, val);
                    return result;
//                    val = 255 * (0.5 + 0.5 * cos(2.1 * (position - index)));
//                    return RGB(val, val, val);
                }),
                .delayMs = delay,
            });
//            pattern.push_back({
//                                      .values = messageCreator(logo, [position](int index) {
//                                          float i = float((index - FIRST_BASE_INDEX + edge) % N_BASE);
//                                          float val;
//                                          for (int edge = 0; edge < 4; edge++) {
//                                              float edgeValue = i > position
//                                                                ? 0.
//                                                                : exp(1.2 * static_cast<float>(i - position)) * 255.;
//                                              val = std::max(val, edgeValue);
//                                          }
//                                          auto result = RGB(0, 0.4 * val, val);
//                                          return result;
////                    val = 255 * (0.5 + 0.5 * cos(2.1 * (position - index)));
////                    return RGB(val, val, val);
//                                      }),
//                                      .delayMs = delay,
//                              });
        }
    }
    else {
        pattern.push_back({
                                  .values = messageCreator(allLeds, [](int index) {
                                      return RGB(0., 120, 255);
                                  }),
                                  .delayMs = 0,
                          });
        pattern.push_back({
                                  .values = messageCreator(allLeds, [](int index) {
                                      return RGB();
                                  }),
                                  .delayMs = 2000,
                          });
        pattern.push_back({
                                  .values = messageCreator(allLeds, [](int index) {
                                      return RGB(0., 80, 150);
                                  }),
                                  .delayMs = 2000,
                          });
    }

    return pattern;
}

int main() {
    std::cout << "Mock Sender: Test UDP package sending." << std::endl;

    Config config{
            .host = "localhost",
            .port = 3413,
            // "logoblink", "lauflichter" or else, standard
            .messages = createPattern("logoblink"),
            .repeats = 10,
            .debug = false,
    };

    const MinimalSocket::Address remote(config.host, config.port);
    MinimalSocket::udp::Udp<true> sender(
            MinimalSocket::ANY_PORT,
            MinimalSocket::AddressFamily::IP_V4
            );

    if (!sender.open()) {
        std::cerr << "Failed to open UDP Sender." << std::endl;
        return 1;
    }

    for (std::size_t i = 0; i <= config.repeats; i++) {

        for (const auto &message : config.messages) {

            std::this_thread::sleep_for(std::chrono::milliseconds(message.delayMs));

            std::string message_string(message.values.begin(), message.values.end());
            sender.sendTo(message_string, remote);

            if (config.debug) {
                std::cout << "Sent UDP to " << to_string(remote)
                          << " (" << message_string.size() << " bytes)"
                          << std::endl;
            }
        }
    }

    std::cout << "Mock Sender Finished without errors." << std::endl;
    return 0;
}
