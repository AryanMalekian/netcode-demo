/**
 * @file server.cpp
 * @brief Cross-platform UDP server for network programming project.
 *
 * Listens for UDP packets from clients on port 54000,
 * decodes each packet, prints its contents, and echoes it back to the sender.
 *
 * Demonstrates:
 * - Use of Winsock (on Windows) or BSD sockets (on macOS/Linux) for raw UDP communication
 * - Serialization/deserialization of custom packet types (see Packet in packet.hpp)
 * - Stateless server loop suitable for multiplayer games or real-time netcode
 *
 * Program flow:
 * 1. Initialize socket API (WSAStartup on Windows; nothing needed on Unix)
 * 2. Create a UDP socket (socket)
 * 3. Bind the socket to port 54000 (bind)
 * 4. Enter main loop:
 *    a. Wait for incoming packets (recvfrom)
 *    b. Deserialize the buffer into a Packet struct
 *    c. Print the packet contents to the console
 *    d. Echo the same packet back to the client (sendto)
 * 5. Cleanup resources on shutdown (closesocket/WSACleanup on Windows, close() on Unix)
 *
 * This code is portable and will compile and run on both Windows and Unix-like systems.
 *
 * @author Aryan Malekian
 * @date 20.05.2025
 */


#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int socket_t;
#endif

#include <iostream>
#include "netcode/common/packet.hpp"

int main() {
#ifdef _WIN32
    // (1) Initialize Winsock API (required on Windows)
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
#endif

    // (2) Create a UDP socket (IPv4, datagram, UDP protocol)
    socket_t sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#ifdef _WIN32
    if (sock == INVALID_SOCKET) {
#else
    if (sock < 0) {
#endif
        std::cerr << "socket() failed\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // (3) Prepare the server address struct and bind the socket to port 54000
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;   // Listen on all local interfaces
    serverAddr.sin_port = htons(54000);        // Convert port number to network byte order

    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr))
#ifdef _WIN32
        == SOCKET_ERROR
#else
        < 0
#endif
        ) {
        std::cerr << "bind() failed\n";
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        return 1;
    }

    std::cout << "Server listening on port 54000...\n";

    // (4) Buffer and address storage for incoming packets
    char buf[static_cast<int>(Packet::size())];
    sockaddr_in clientAddr;
#ifdef _WIN32
    int clientAddrSize = sizeof(clientAddr);
#else
    socklen_t clientAddrSize = sizeof(clientAddr);
#endif

    // (5) Main server loop: receive, process, and echo packets
    while (true) {
        // Wait for a UDP packet from any client
        int bytes = recvfrom(sock, buf, static_cast<int>(Packet::size()), 0,
            (sockaddr*)&clientAddr, &clientAddrSize);

        // If the correct number of bytes was received, process the packet
        if (bytes == static_cast<int>(Packet::size())) {
            Packet p;
            p.deserialize(buf);  // Unpack raw bytes into a Packet struct

            // Print packet details for debugging/logging
            std::cout << "Received seq=" << p.seq
                << " x=" << p.x << " y=" << p.y
                << " vx=" << p.vx << " vy=" << p.vy << "\n";

            // Echo the same packet data back to the sender
            sendto(sock, buf, static_cast<int>(Packet::size()), 0,
                (sockaddr*)&clientAddr, clientAddrSize);
        }
    }

    // (6) Cleanup (this will rarely run, but is good practice)
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    return 0;
    }
