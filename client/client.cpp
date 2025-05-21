/**
 * @file client.cpp
 * @brief UDP client for netcode demo project.
 *
 * Sends a simulated moving object's state to the server via UDP.
 * Receives and prints echoed packets, applies prediction, and prints both
 * actual and predicted positions each frame.
 *
 * Demonstrates:
 * - Use of Winsock for UDP sockets
 * - Use of the shared Packet struct for network data
 * - Simple network loop for real-time applications
 * - Client-side prediction via separate module (prediction.hpp/ .cpp)
 *
 * @author Aryan Malekian
 * @date 20.05.2025
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../include/packet.hpp"
#include "../include/prediction.hpp"

#pragma comment(lib, "Ws2_32.lib")

int main() {
    // (1) Initialize Winsock API
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    // (2) Create a UDP socket
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket() failed\n";
        WSACleanup();
        return 1;
    }

    // (3) Prepare server address (localhost:54000)
    sockaddr_in servAddr{};
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);
    servAddr.sin_port = htons(54000);

    // (4) Simulation state
    uint32_t seq = 0;
    float x = 0, y = 0;

    // (5) Remote state & timestamp for prediction
    Packet remote{};  // last known remote state
    auto lastRecvTime = std::chrono::steady_clock::now();

    // (6) Buffers for sending/receiving
    char buf[Packet::size()];
    sockaddr_in fromAddr;
    int fromSize = sizeof(fromAddr);

    // (7) Main loop: send packet, receive, predict
    while (true) {
        // 7a) Simulate local motion: move right at 1 unit/sec
        float dt = 0.1f;    // 100 ms per tick
        float vx = 1.0f, vy = 0.0f;
        x += vx * dt;

        // 7b) Serialize & send packet
        Packet p{ seq++, x, y, vx, vy };
        p.serialize(buf);
        sendto(sock, buf, Packet::size(), 0,
            (sockaddr*)&servAddr, sizeof(servAddr));

        // 7c) Receive echoed packet
        int bytes = recvfrom(sock, buf, Packet::size(), 0,
            (sockaddr*)&fromAddr, &fromSize);
        if (bytes == Packet::size()) {
            remote.deserialize(buf);  // update remote state
            lastRecvTime = std::chrono::steady_clock::now();

            
            std::cout << "Remote actual=("
                << remote.x << ", " << remote.y
                << ") seq=" << remote.seq << "\n";
        }

        // 7d) Predict remote position now
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = now - lastRecvTime;

        // manual unpack instead of structured binding, because for some reason my copmiler doesn't let me do it automatically
        std::pair<float, float> predicted = predictPosition(remote, elapsed.count());
        float predX = predicted.first;
        float predY = predicted.second;

        std::cout << "Predicted pos=("
            << predX << ", " << predY
            << ") after " << elapsed.count() << "s\n";

        // 7e) Wait next tick
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // (8) Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}
