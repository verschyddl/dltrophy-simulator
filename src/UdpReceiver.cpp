//
// Created by qm210 on 10.05.2025.
//

#include <iostream>
#include "UdpReceiver.h"

UdpReceiver::UdpReceiver(int port)
: port(port) {

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

    // Thread: brauchen wir evtl gar nicht.
//
//    thread = std::thread([this] {
//        run();
//    });
}

UdpReceiver::~UdpReceiver() {
    alive = false;
    thread.join();
}

void UdpReceiver::run() {
    // to be run in its own thread
    while (alive) {
        auto message = socket.receive(max_message_size);
        if (!message.has_value()) {
            continue;
        }
        std::lock_guard<std::mutex> lock(queueMutex);
        messageQueue.push(message.value());
        queueCondition.notify_one();
    }
}

bool UdpReceiver::lookForMessage(MinimalSocket::ReceiveStringResult& message) {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (messageQueue.empty()) {
        return false;
    }
    message = std::move(messageQueue.front());
}
