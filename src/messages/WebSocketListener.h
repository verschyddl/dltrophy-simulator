//
// Created by qm210 on 08.06.2025.
//

#ifndef DLTROPHY_SIMULATOR_SOCKETSERVICE_H
#define DLTROPHY_SIMULATOR_SOCKETSERVICE_H

#include <mutex>
#include <queue>
#include <string>
#include <iostream>
#include <thread>
#include <ctime>
#include "ixwebsocket/IXWebSocket.h"
#include "timeFormat.h"

struct LiveviewMessage {
    char header; // required to be L
    int version; // required to be 2 (for 2D)
    Size size{};
    std::vector<LED> colors{};
    std::string error;
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();

    [[nodiscard]]
    std::string formattedTime() const {
        return formatTime(timestamp);
    }
};

class WebSocketListener {
    // unused since the UdpListener now works

public:
    enum class State {
        Disconnected,
        Connecting,
        Connected,
    };

private:
    std::string endpoint_;
    ix::WebSocket socket;

    std::thread thread;
    std::queue<LiveviewMessage> queue;
    std::mutex queueMutex;
    std::atomic<State> state{State::Disconnected};
    std::atomic<bool> hasError{false};

    template<typename... Args>
    static void log(std::format_string<Args...> fmt, Args&&... args) {

        auto message = std::format(fmt, std::forward<Args>(args)...);
        std::cout << "[WebSocketListener][" << formatTime() << "] "
                  << message << std::endl;
    }

    static LiveviewMessage interpret(const std::string& message) {
        LiveviewMessage result{
            .header = message[0],
            .version = message[1],
        };
        if (result.header != 'L') {
            result.error = "First Byte has to be \"L\" (76)";
            return result;
        }
        auto length = static_cast<int>(message.size());
        int startIndex;
        switch (result.version) {
            case 1:
                result.size = {static_cast<int>(message.size()) - 2, 1};
                startIndex = 2;
                break;
            case 2:
                result.size = {message[2], message[3]};
                startIndex = 4;
                break;
            default:
                result.error = "Version Byte is not known";
                return result;
        }

        try {
            for (size_t i = startIndex; i < length; i += 3) {
                result.colors.push_back(
                        LED(message[i], message[i + 1], message[i + 2])
                );
            }
        } catch (const std::exception& e) {
            result.error = e.what();
        }
        return result;
    }

    void onMessage(const ix::WebSocketMessagePtr &msg) {
        switch (msg->type) {
            case ix::WebSocketMessageType::Message: {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    auto message = interpret(msg->str);
                    queue.push(message);
                }
                break;
            case ix::WebSocketMessageType::Open:
                log("Open");
                break;
            case ix::WebSocketMessageType::Close:
                log("Close");
                break;
            case ix::WebSocketMessageType::Error: {
                auto err = msg->errorInfo;
                log("Error {}: {} ({} {}{})",
                    err.http_status,
                    err.reason,
                    err.retries,
                    err.wait_time,err.decompressionError ? " decompr" : "");
                }
                hasError = true;
                break;
            case ix::WebSocketMessageType::Ping:
                log("Ping");
                break;
            case ix::WebSocketMessageType::Pong:
                log("Pong");
                break;
            case ix::WebSocketMessageType::Fragment:
                log("Fragment");
                break;
        }
    }

    void run() {
        state = State::Connecting;
        auto url = std::format("ws://{}", endpoint_);
        socket.setUrl(url);
        socket.setOnMessageCallback(
                [this](const auto& msg) {
                    this->onMessage(msg);
                });
        socket.start();

        log("Initiating Websocket under {}", url);
        while (socket.getReadyState() != ix::ReadyState::Open) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (hasError.load()) {
                socket.stop();
                state = State::Disconnected;
                return;
            }
        }
        state = State::Connected;
        std::string startMessage = "{\"lv\": true}";
        socket.send(startMessage);

        log("Listening.");
        while (socket.getReadyState() != ix::ReadyState::Closed) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        state = State::Disconnected;
        log("Stopped listening.");
    }

public:
    explicit WebSocketListener(const std::string& endpoint) {
        connect(endpoint);
    }

    ~WebSocketListener() {
        disconnect();
    }

    [[nodiscard]]
    State connectionState() const { return state.load(); }

    void disconnect() {
        socket.stop();
        if (thread.joinable()) {
            thread.join();
        }
        state = State::Disconnected;
        hasError = false;
    }

    void connect(const std::string& endpoint) {
        if (connectionState() != State::Disconnected) {
            disconnect();
        }
        endpoint_ = endpoint;
        thread = std::thread([this] { this->run(); });
    }

    std::optional<LiveviewMessage> listen() {
        if (connectionState() != State::Connected) {
            return std::nullopt;
        }

        std::unique_lock<std::mutex> lock(queueMutex);
        if (queue.empty()) {
            return std::nullopt;
        }
        auto message = std::move(queue.front());
        queue.pop();
        return message;
    }

    [[nodiscard]]
    bool isRunningUnder(const std::string& endpoint) const {
        return connectionState() == State::Connected
               && endpoint == endpoint_;
    }
};

#endif //DLTROPHY_SIMULATOR_SOCKETSERVICE_H
