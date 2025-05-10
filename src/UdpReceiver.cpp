//
// Created by qm210 on 10.05.2025.
//

#include "UdpReceiver.h"
//
//class UdpReceiver {
//private:
//    int sockfd;
//    struct sockaddr_in server_addr;
//
//public:
//    UdpReceiver(uint16_t port) {
//        // Create UDP socket
//        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//        if (sockfd < 0) return;
//
//        // Set address reuse to handle quick restarts
//        int optval = 1;
//        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
//
//        // Configure address
//        server_addr.sin_family = AF_INET;
//        server_addr.sin_port = htons(port);
//        server_addr.sin_addr.s_addr = INADDR_ANY;
//
//        // Bind socket
//        if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
//            close(sockfd);
//            sockfd = -1;
//            return;
//        }
//    }
//
//    ~UdpReceiver() {
//        if (sockfd >= 0) close(sockfd);
//    }
//
//    bool isValid() const { return sockfd >= 0; }
//
//    size_t receive(void* buffer, size_t len) {
//        if (!isValid()) return 0;
//
//        struct sockaddr_in sender_addr;
//        socklen_t sender_len = sizeof(sender_addr);
//
//        return recvfrom(sockfd, buffer, len, 0,
//                        (struct sockaddr*)&sender_addr, &sender_len);
//    }
//};

UdpReceiver::UdpReceiver(int port) {
    this->port = port;
    this->socket = MinimalSocket::udp::Udp<true>(
            port,
            MinimalSocket::AddressFamily::IP_V4
            );
}
