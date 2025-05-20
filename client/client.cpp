/**
 * @file client.cpp
 * @brief UDP client for netcode demo project.
 *
 * Sends a simulated moving object's state to the server via UDP.
 * Receives and prints echoed packets from the server.
 *
 * Demonstrates:
 * - Use of Winsock for UDP sockets
 * - Use of the shared Packet struct for network data
 * - Simple network loop for real-time applications
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

    // (5) Buffers for sending/receiving
    char buf[Packet::size()];
    sockaddr_in fromAddr;
    int fromSize = sizeof(fromAddr);

    // (6) Main loop: send packet every 100ms, print echo from server
    while (true) {
        // Simulate motion: move right at 1 unit/sec
        float dt = 0.1f;
        float vx = 1.0f, vy = 0.0f;
        x += vx * dt;

        // Create and serialize packet
        Packet p{ seq++, x, y, vx, vy };
        p.serialize(buf);

        // Send packet to server
        sendto(sock, buf, Packet::size(), 0,
            (sockaddr*)&servAddr, sizeof(servAddr));

        // Receive echoed packet from server
        int bytes = recvfrom(sock, buf, Packet::size(), 0,
            (sockaddr*)&fromAddr, &fromSize);

        // If valid, print out the echoed packet
        if (bytes == Packet::size()) {
            Packet p2;
            p2.deserialize(buf);
            std::cout << "Echo seq=" << p2.seq
                << " x=" << p2.x << " y=" << p2.y << "\n";
        }

        // Sleep for 100 ms
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // (7) Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}
