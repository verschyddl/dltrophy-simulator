//
// Created by qm210 on 26.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_UDPINTERPRETER_H
#define DLTROPHY_SIMULATOR_UDPINTERPRETER_H

#include <map>
#include <optional>
#include <format>
#include <ctime>

#include "../LED.h"
#include "UdpListener.h"
#include "timeFormat.h"

enum class RealtimeProtocol {
    // cf. https://kno.wled.ge/interfaces/udp-realtime/
    // there are more defined, but only these seem useless here:
    // WARLS: <2 header bytes>; INDEX R G B INDEX R G B ...
    WARLS,
    // DRGB: <2 header bytes>; R G B R G B ...
    DRGB,
    // DRGBW: not implemented because of too little sense
    // DNRGB: <2 header bytes> <2 start index bytes>; R G B R G B ...
    DNRGB,
};

struct ProtocolMessage {
    RealtimeProtocol protocol;
    std::optional<int> timeoutSec;
    std::unordered_map<uint8_t, LED> mapping;
    std::string source;
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
};

struct UnreadableMessage {
    std::string reason;
    RawMessage original;
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();

    void printDebug(std::ostream& os) const {
        os << "[UDP MESSAGE][" << formatTime(timestamp)
           << "] Unreadable. " << reason
           << std::endl;
    }
};

struct IndexStride{
    size_t step;
    size_t start;
};

const std::unordered_map<RealtimeProtocol, IndexStride> protocolStride{
        {RealtimeProtocol::WARLS, {4, 2}},
        {RealtimeProtocol::DRGB, {3, 2}},
        {RealtimeProtocol::DNRGB, {3, 4}},
};

using AnyMessage = std::variant<ProtocolMessage, UnreadableMessage>;

class UdpInterpreter {
public:

    static AnyMessage interpret(const RawMessage& message) {
        auto maybeResult = interpretHeader(message);
        if (std::holds_alternative<UnreadableMessage>(maybeResult)) {
            return maybeResult;
        }
        auto result = std::get<ProtocolMessage>(maybeResult);
        auto stride = protocolStride.at(result.protocol);
        size_t ledIndex;
        LED led;

        for (size_t i = stride.start; i < message.values.size(); i += stride.step) {

            switch (result.protocol) {

                case RealtimeProtocol::WARLS:
                    ledIndex = message.values[i];
                    led = LED::from(message.values, i+1);
                    break;

                case RealtimeProtocol::DRGB:
                    ledIndex = (i - stride.start) / stride.step;
                    led = LED::from(message.values, i);
                    break;

                case RealtimeProtocol::DNRGB:
                    ledIndex = (i - stride.start) / stride.step
                            + int16from(message.values[2], message.values[3]);
                    led = LED::from(message.values, i);
                    break;

                default:
                    // actually handled above already, but just to be sure.
                    return UnreadableMessage{"Invalid Protocol", message};
            }

            result.mapping[ledIndex] = led;
        }

        return result;
    }

    static uint16_t int16from(uint8_t highByte, uint8_t lowByte) {
        return (static_cast<uint16_t>(highByte) << 8) | lowByte;
    }

private:
    static std::variant<ProtocolMessage, UnreadableMessage> interpretHeader(const RawMessage& message) {
        if (message.values.size() < 2) {
            return UnreadableMessage{"Message too short (needs 2 header bytes)", message};
        }

        int protocolIndex = message.values[0];
        std::optional<int> timeoutSec = message.values[1];

        auto protocol = asProtocol(protocolIndex);
        if (!protocol) {
            return UnreadableMessage{
                    std::format("Unsupported Protocol: {}", protocolIndex),
                    message
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
            case 4:
                return RealtimeProtocol::DNRGB;
            default:
                return std::nullopt;
        }
    }
};

#endif //DLTROPHY_SIMULATOR_UDPINTERPRETER_H
