#include <iostream>
#include <chrono>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../include/packet.hpp"

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket() failed\n";
        return 1;
    }

    sockaddr_in servAddr{};
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);
    servAddr.sin_port = htons(54000);

    uint32_t seq = 0;
    float x = 0, y = 0;

    char buf[Packet::size()];
    sockaddr_in fromAddr;
    int fromSize = sizeof(fromAddr);

    while (true) {

        float dt = 0.1f;
        float vx = 1.0f, vy = 0.0f;
        x += vx * dt;

        Packet p{ seq++, x, y, vx, vy };
        p.serialize(buf);
        sendto(sock, buf, Packet::size(), 0,
            (sockaddr*)&servAddr, sizeof(servAddr));

        int bytes = recvfrom(sock, buf, Packet::size(), 0,
            (sockaddr*)&fromAddr, &fromSize);
        if (bytes == Packet::size()) {
            Packet p2;
            p2.deserialize(buf);
            std::cout << "Echo seq=" << p2.seq
                << " x=" << p2.x << " y=" << p2.y << "\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
