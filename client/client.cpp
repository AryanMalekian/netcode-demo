/**
 * @file client.cpp
 * @brief UDP client for netcode demo project.
 *
 * Sends a simulated moving object's state to the server via UDP.
 * Receives and prints echoed packets, applies prediction, and prints both
 * actual, predicted, and interpolated positions each frame.
 *
 * Demonstrates:
 * - Use of Winsock for UDP sockets
 * - Use of the shared Packet struct for network data
 * - Simple network loop for real-time applications
 * - Client-side prediction via separate module (prediction.hpp/ .cpp)
 * - Client-side interpolation via separate module (interpolation.hpp/ .cpp)
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
#include "../include/interpolation.hpp"

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

    // (5) Packet buffers & timestamps for prediction & interpolation
    Packet prevPacket{};                                // earlier state
    Packet nextPacket{};                                // latest state
    bool hasPrev = false;                               // true once we have two packets
    auto prevRecvTime = std::chrono::steady_clock::now();
    auto nextRecvTime = std::chrono::steady_clock::now();

    // (6) Buffers for sending/receiving
    char buf[Packet::size()];
    sockaddr_in fromAddr;
    int fromSize = sizeof(fromAddr);

    // (7) Main loop: send packet, receive, predict & interpolate
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
            // shift the buffer: latest becomes previous
            prevPacket = nextPacket;
            prevRecvTime = nextRecvTime;

            // deserialize into nextPacket
            nextPacket.deserialize(buf);
            nextRecvTime = std::chrono::steady_clock::now();
            hasPrev = true;

            // Print actual remote state
            std::cout << "Remote actual=("
                << nextPacket.x << ", " << nextPacket.y
                << ") seq=" << nextPacket.seq << "\n";
        }

        // 7d) Prediction: where we think it is now
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = now - nextRecvTime;
        auto predicted = predictPosition(nextPacket, elapsed.count());
        float predX = predicted.first;
        float predY = predicted.second;
        std::cout << "Predicted pos=("
            << predX << ", " << predY
            << ") after " << elapsed.count() << "s\n";

        // 7e) Interpolation: smooth between prevPacket and nextPacket
        if (hasPrev) {
            // total interval between packets
            std::chrono::duration<float> interval = nextRecvTime - prevRecvTime;
            // how far past the latest packet timestamp
            std::chrono::duration<float> past = now - nextRecvTime;
            // choose a small interpolation delay (e.g., 100ms) if desired, here zero:
            float t = past.count() / interval.count();
            if (t < 0.f) t = 0.f;
            if (t > 1.f) t = 1.f;

            auto interp = interpolatePosition(prevPacket, nextPacket, t);
            std::cout << "Interpolated pos=("
                << interp.first << ", " << interp.second
                << ") t=" << t << "\n";
        }

        // 7f) Wait next tick
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // (8) Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}
