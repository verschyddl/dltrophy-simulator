//
// Created by qm210 on 26.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_UDPINTERPRETER_H
#define DLTROPHY_SIMULATOR_UDPINTERPRETER_H

#include "LED.h"
#include "UdpReceiver.h"
#include <map>
#include <optional>
#include <format>

enum class RealtimeProtocol {
    // cf. https://kno.wled.ge/interfaces/udp-realtime/
    // there are more defined, but only these seem useless here:
    // WARLS: <2 header bytes>; INDEX R G B INDEX R G B ...
    // DRGB: <2 header bytes>; R G B R G B ...

    WARLS,
    DRGB,
};

struct ProtocolMessage {
    RealtimeProtocol protocol;
    std::optional<int> timeoutSec;
    std::unordered_map<uint8_t, LED> mapping;
    std::string source;
};

struct UnreadableMessage {
    std::string reason;
};

const std::unordered_map<RealtimeProtocol, size_t> protocolUnitSize{
        {RealtimeProtocol::WARLS, 4},
        {RealtimeProtocol::DRGB, 3},
};

class UdpInterpreter {
public:

    static std::variant<ProtocolMessage, UnreadableMessage> interpret(const RawMessage& message) {
        auto maybeResult = interpretHeader(message);
        if (std::holds_alternative<UnreadableMessage>(maybeResult)) {
            return maybeResult;
        }

        auto result = std::get<ProtocolMessage>(maybeResult);
        int unitSize = protocolUnitSize.at(result.protocol);
        size_t ledIndex;
        LED led;

        for (size_t i = 2; i < message.values.size(); i += unitSize) {
            switch (result.protocol) {

                case RealtimeProtocol::WARLS:
                    ledIndex = message.values[i];
                    led = LED(message.values[i + 1],
                              message.values[i + 2],
                              message.values[i + 3]);
                    break;

                case RealtimeProtocol::DRGB:
                    ledIndex = (i - 2) / unitSize;
                    led = LED(message.values[i],
                              message.values[i + 1],
                              message.values[i + 2]);
                    break;

                default:
                    // actually handled above already, but just to be sure.
                    return UnreadableMessage{"Invalid Protocol"};
            }

            result.mapping[ledIndex] = std::move(led);
        }

        return result;
    }

private:
    static std::variant<ProtocolMessage, UnreadableMessage> interpretHeader(const RawMessage& message) {
        if (message.values.size() < 2) {
            return UnreadableMessage{"Message too short (needs 2 header bytes)"};
        }

        int protocolIndex = message.values[0];
        std::optional<int> timeoutSec = message.values[1];

        auto protocol = asProtocol(protocolIndex);
        if (!protocol) {
            return UnreadableMessage{
                    std::format("Unsupported Protocol: {}", protocolIndex)
            };
        }

        if (timeoutSec < 0 || timeoutSec >= 255) {
            // 255 is the value WLED actually sends for "no timeout"
            timeoutSec = std::nullopt;
        }

        return ProtocolMessage{
            .protocol = *protocol,
            .timeoutSec = timeoutSec,
            .mapping = {},
            .source = message.source,
        };
    }

    static std::optional<RealtimeProtocol> asProtocol(uint8_t number) {
        switch (number) {
            case 1:
                return RealtimeProtocol::WARLS;
            case 2:
                return RealtimeProtocol::DRGB;
            default:
                return std::nullopt;
        }
    }
};

#endif //DLTROPHY_SIMULATOR_UDPINTERPRETER_H
