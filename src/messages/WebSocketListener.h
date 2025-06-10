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
//#include "App.h" // uWebSockets (as you can clearly infer from the name)
#include "ixwebsocket/IXWebSocket.h"

#define IS_WINDOWS defined(_WIN32) || defined(_WIN64)
#define IS_LINUX defined(__linux__) || defined(__unix__)

struct LiveviewMessage {
    char header; // required to be L
    int version; // required to be 2 (for 2D)
    Size size{};
    std::vector<LED> colors{};
    std::string error;
};

class WebSocketListener {
public:
    enum class State {
        Disconnected,
        Connecting,
        Connected,
    };

private:
    std::string endpoint_;
    std::string pattern = "/*";
    int port = 9001;

    ix::WebSocket socket;
    std::thread thread;
    std::queue<LiveviewMessage> queue;
    std::mutex queueMutex;
//    std::condition_variable queueCv;
    std::atomic<State> state{State::Disconnected};
    std::string error_;

//    struct PerSocketData {
//        std::string username;
//    };

    static std::string timestamp() {
        time_t now = time(nullptr);
        char result[32] = "TIMESTAMP_ERROR";
        struct tm tm;
        // to unite cross-compatibility and thread-safety -- this abomination.
        if (
#if IS_WINDOWS
                localtime_s(&tm, &now) == 0
#elif IS_LINUX
                localtime_r(&now, &tm) != nullptr
#else
                false
#endif
        ) {
            strftime(result, sizeof(result), "%Y/%m/%d %H:%M:%S", &tm);
        }
        return std::string(result);
    }

    template<typename... Args>
    static void log(std::format_string<Args...> fmt, Args&&... args) {

        auto message = std::format(fmt, std::forward<Args>(args)...);
        std::cout << "[WebSocketListener][ " << timestamp() << "] "
                  << message << std::endl;
    }

    static LiveviewMessage interpret2D(const std::string& message) {
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
        std::string info;
        switch (msg->type) {
            case ix::WebSocketMessageType::Message: {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    auto message = interpret2D(msg->str);
                    queue.push(message);
//                    queueCv.notify_one();

                    info = message.error.empty()
                                ? std::format("Liveview: {}x{}, {} LEDs",
                                              message.size.width,
                                              message.size.height,
                                              message.colors.size())
                                : std::format("Corrupted: {}", message.error);
                }
                break;
            case ix::WebSocketMessageType::Open:
                info = "Open";
                break;
            case ix::WebSocketMessageType::Close:
                info = "Close";
                break;
            case ix::WebSocketMessageType::Error: {
                auto err = msg->errorInfo;
                info = std::format("Error {}: {} ({} {}{})",
                                   err.http_status,
                                   err.reason,
                                   err.retries,
                                   err.wait_time,
                                   err.decompressionError ? " decompr" : "");
                }
                break;
            case ix::WebSocketMessageType::Ping:
                info = "Ping";
                break;
            case ix::WebSocketMessageType::Pong:
                info = "Pong";
                break;
            case ix::WebSocketMessageType::Fragment:
                info = "Fragment";
                break;
        }
        log("Lel... got message: {}", info);
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
        state = State::Connected;

        log("Initiating Websocket under {}", url);
        while (socket.getReadyState() != ix::ReadyState::Open) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
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
//        uWS::App().ws<PerSocketData>(pattern, {
//                .open = [](auto *ws) {
//                    std::cout << "[WebSocketListener] Connected." << std::endl;
//                    auto *data = (PerSocketData*)ws->getUserData();
//                    data->username = "testing-username-for-fun";
//                },
//                .message = [this](auto *ws, std::string_view message, uWS::OpCode opCode) {
//                    std::lock_guard<std::mutex> lock(queueMutex);
//                    queue.push(std::string(message));
//                }
//        }).listen(port, [this](auto *listenSocket) {
//            if (listenSocket) {
//                std::cout << "[WebSocketListener] Listening on port " << port << std::endl;
//            } else {
//                std::cerr << "[WebSocketListener] Not Listening." << std::endl;
//            }
//        }).run();
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
            std::cerr << "[WebSocketListener] not connected, cannot listen for shit." << std::endl;
            return std::nullopt;
        }

        std::unique_lock<std::mutex> lock(queueMutex);
        if (queue.empty()) {
            return std::nullopt;
        }
        // queueCv.wait(lock, [this] { return !queue.empty(); });
        auto message = std::move(queue.front());
        queue.pop();
        return message;
    }

    [[nodiscard]]
    bool isRunningUnder(const std::string& endpoint) const {
        return connectionState() == State::Connected
               && endpoint == endpoint_;
    }

    [[nodiscard]]
    std::string error() { return error_; }
};

#endif //DLTROPHY_SIMULATOR_SOCKETSERVICE_H
