/**
 * @file client.cpp
 * @brief UDP client for netcode demo project, with SFML visual demo.
 *
 * Sends a simulated moving object's state to the server via UDP.
 * Receives and prints echoed packets, applies prediction, and prints both
 * actual, predicted, and interpolated positions each frame. Visualizes all four
 * dots in a horizontal row behind the local dot for a “snake” effect.
 *
 * Demonstrates:
 * - Use of Winsock for UDP sockets
 * - Use of the shared Packet struct for network data
 * - Simple network loop for real-time applications
 * - Client-side prediction via separate module (prediction.hpp/.cpp)
 * - Client-side interpolation via separate module (interpolation.hpp/.cpp)
 * - Minimal graphics demo using SFML (A-grade requirement)
 *
 * @author Aryan Malekian
 * @date 20.05.2025
 */

#define NOMINMAX
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../include/packet.hpp"
#include "../include/prediction.hpp"
#include "../include/interpolation.hpp"

#include <SFML/Graphics.hpp>

#pragma comment(lib, "Ws2_32.lib")
 // With vcpkg, SFML is linked automatically if your project uses the vcpkg toolchain file

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

    // Make socket non-blocking (Windows only)
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    // (3) Prepare server address (localhost:54000)
    sockaddr_in servAddr{};
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);
    servAddr.sin_port = htons(54000);

    // (4) Simulation state
    uint32_t seq = 0;
    float x = 100, y = 200; // Start at (100, 200) for visuals

    // (5) Prediction/interpolation buffers and timestamps
    Packet prevPacket{}, nextPacket{};
    bool hasPrev = false;
    auto prevRecvTime = std::chrono::steady_clock::now();
    auto nextRecvTime = std::chrono::steady_clock::now();

    // (6) Buffers for sending/receiving
    char buf[Packet::size()];
    sockaddr_in fromAddr;
    int fromSize = sizeof(fromAddr);

    // (7) SFML setup
    sf::RenderWindow window(sf::VideoMode(800, 600), "Netcode Demo Visual");
    window.setFramerateLimit(60);
    sf::CircleShape localDot(10.f);      localDot.setFillColor(sf::Color::Green);
    sf::CircleShape remoteDot(10.f);     remoteDot.setFillColor(sf::Color::Red);
    sf::CircleShape predictedDot(10.f);  predictedDot.setFillColor(sf::Color::Blue);
    sf::CircleShape interpDot(10.f);     interpDot.setFillColor(sf::Color(255, 165, 0)); // Orange

    constexpr float SPACING = 30.f;  // pixels between each dot

    // (8) Main loop: send packet, receive, predict, interpolate, draw
    while (window.isOpen()) {
        // a) SFML event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // b) Simulate local motion (arrow keys)
        float dt = 0.1f;    // 100 ms per tick
        float vx = 0.0f, vy = 0.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  vx = 120.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))   vx = -120.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))   vy = 120.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))     vy = -120.0f;
        x += vx * dt;
        y += vy * dt;

        // c) Serialize & send packet
        Packet p{ seq++, x, y, vx, vy };
        p.serialize(buf);
        sendto(sock, buf, Packet::size(), 0,
            (sockaddr*)&servAddr, sizeof(servAddr));

        // d) Receive echoed packet (non-blocking)
        int bytes = recvfrom(sock, buf, Packet::size(), 0,
            (sockaddr*)&fromAddr, &fromSize);
        if (bytes == Packet::size()) {
            // Shift old -> prev, curr -> next
            prevPacket = nextPacket;
            prevRecvTime = nextRecvTime;

            nextPacket.deserialize(buf);
            nextRecvTime = std::chrono::steady_clock::now();
            hasPrev = true;

            std::cout << "Remote actual=("
                << nextPacket.x << ", " << nextPacket.y
                << ") seq=" << nextPacket.seq << "\n";
        }
        else if (bytes == -1 && WSAGetLastError() == WSAEWOULDBLOCK) {
            // no data available
        }
        else if (bytes == -1) {
            std::cerr << "Socket error: " << WSAGetLastError() << "\n";
        }

        // e) Prediction: where do we think it is now?
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = now - nextRecvTime;
        auto predicted = predictPosition(nextPacket, elapsed.count());
        float predX = predicted.first;
        float predY = predicted.second;

        std::cout << "Predicted pos=("
            << predX << ", " << predY
            << ") after " << elapsed.count() << "s\n";

        // f) Interpolation: smooth between prevPacket and nextPacket
        float interpX = nextPacket.x, interpY = nextPacket.y;
        if (hasPrev) {
            std::chrono::duration<float> interval = nextRecvTime - prevRecvTime;
            std::chrono::duration<float> sinceNext = now - nextRecvTime;
            float t = (interval.count() > 0.0001f)
                ? (sinceNext.count() / interval.count())
                : 0.f;
            t = std::clamp(t, 0.f, 1.f);
            auto interp = interpolatePosition(prevPacket, nextPacket, t);
            interpX = interp.first;
            interpY = interp.second;
        }

        // g) Compute horizontal positions for each dot
        sf::Vector2f basePos{ x, y };
        sf::Vector2f remotePos = basePos + sf::Vector2f(-SPACING, 0.f);
        sf::Vector2f predictedPos = basePos + sf::Vector2f(-2 * SPACING, 0.f);
        sf::Vector2f interpPos = basePos + sf::Vector2f(-3 * SPACING, 0.f);

        // h) SFML visual: draw local (green), remote (red),
        //               predicted (blue), interpolated (orange)
        window.clear(sf::Color::Black);
        localDot.setPosition(basePos);
        remoteDot.setPosition(remotePos);
        predictedDot.setPosition(predictedPos);
        interpDot.setPosition(interpPos);

        window.draw(localDot);
        window.draw(remoteDot);
        window.draw(predictedDot);
        if (hasPrev) window.draw(interpDot);
        window.display();

        // i) Wait next tick
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // (9) Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}
