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
#include <uWebSockets/App.h>

class SocketService {
private:
    std::queue<std::string> messageQueue;
    std::mutex queueMutex;

public:
    SocketService() {
        uWS::App().ws("/*", {
                .open = [](auto *ws) {
                    std::cout << "WebSocket connected" << std::endl;
                },
                .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    messageQueue.push(std::string(message));
                }
        }).listen(9001, [](auto *listenSocket) {
            if (listenSocket) {
                std::cout << "Listening on port 9001" << std::endl;
            }
        }).run();
    }

    ~SocketService() {

    }

};

#endif //DLTROPHY_SIMULATOR_SOCKETSERVICE_H
