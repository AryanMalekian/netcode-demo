/**
 * @file server.cpp
 * @brief UDP server for network programming  project.
 *
 * Listens for UDP packets from clients on port 54000,
 * decodes each packet, prints its contents, and echoes it back to the sender.
 *
 * Demonstrates:
 * - Use of Winsock for raw UDP communication
 * - Serialization/deserialization of custom packet types (see Packet in packet.hpp)
 * - Stateless server loop suitable for multiplayer games or real-time netcode
 *
 * Program flow:
 * 1. Initialize Winsock API (WSAStartup)
 * 2. Create a UDP socket (socket)
 * 3. Bind the socket to port 54000 (bind)
 * 4. Enter main loop:
 *    a. Wait for incoming packets (recvfrom)
 *    b. Deserialize the buffer into a Packet struct
 *    c. Print the packet contents to the console
 *    d. Echo the same packet back to the client (sendto)
 * 5. Cleanup resources on shutdown (closesocket, WSACleanup)
 *
 * @author Aryan Malekian
 * @date 20.05.2025
 */

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../include/packet.hpp"

#pragma comment(lib, "Ws2_32.lib")

int main() {
    // (1) Initialize Winsock API (required on Windows)
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    // (2) Create a UDP socket (IPv4, datagram, UDP protocol)
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket() failed\n";
        WSACleanup();
        return 1;
    }

    // (3) Prepare the server address struct and bind the socket to port 54000
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;   // Listen on all local interfaces
    serverAddr.sin_port = htons(54000);        // Convert port number to network byte order

    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "bind() failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port 54000...\n";

    // (4) Buffer and address storage for incoming packets
    char buf[Packet::size()];
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    // (5) Main server loop: receive, process, and echo packets
    while (true) {
        // Wait for a UDP packet from any client
        int bytes = recvfrom(sock, buf, Packet::size(), 0,
            (sockaddr*)&clientAddr, &clientAddrSize);

        // If the correct number of bytes was received, process the packet
        if (bytes == Packet::size()) {
            Packet p;
            p.deserialize(buf);  // Unpack raw bytes into a Packet struct

            // Print packet details for debugging/logging
            std::cout << "Received seq=" << p.seq
                << " x=" << p.x << " y=" << p.y
                << " vx=" << p.vx << " vy=" << p.vy << "\n";

            // Echo the same packet data back to the sender
            sendto(sock, buf, Packet::size(), 0,
                (sockaddr*)&clientAddr, clientAddrSize);
        }
    }

    // (6) Cleanup (this will rarely run, but is good practice)
    closesocket(sock);
    WSACleanup();
    return 0;
}