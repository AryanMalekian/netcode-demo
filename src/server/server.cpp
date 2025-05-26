/**
 * @file server.cpp
 * @brief Cross-platform UDP server for network programming project.
 *
 * Listens for UDP packets from clients on port 54000,
 * decodes each packet, processes input commands, and sends back authoritative game state.
 *
 * Demonstrates:
 * - Use of Winsock (on Windows) or BSD sockets (on macOS/Linux) for raw UDP communication
 * - Serialization/deserialization of custom packet types (see Packet in packet.hpp)
 * - Authoritative server-side game simulation for multiplayer games
 * - Client input processing and server-side physics
 * - Robust error handling and packet validation
 *
 * Program flow:
 * 1. Initialize socket API (WSAStartup on Windows; nothing needed on Unix)
 * 2. Create a UDP socket (socket)
 * 3. Bind the socket to port 54000 (bind)
 * 4. Enter main loop:
 *    a. Wait for incoming packets (recvfrom)
 *    b. Deserialize the buffer into a Packet struct
 *    c. Validate packet contents for security
 *    d. Process input commands and update server-side player state
 *    e. Send back authoritative player position to the client (sendto)
 * 5. Cleanup resources on shutdown (closesocket/WSACleanup on Windows, close() on Unix)
 *
 * This code is portable and will compile and run on both Windows and Unix-like systems.
 *
 * @author Aryan Malekian & Jonathan Skomsøy Hübertz,  w/ use of A.I. Models
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
#include <cerrno>
#include <cstring>
typedef int socket_t;
#endif

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include "netcode/common/packet.hpp"

/**
 * @struct ClientState
 * @brief Server-side state for each connected client
 */
struct ClientState {
    float x, y;           // Authoritative position
    float vx, vy;         // Current velocity  
    uint32_t lastSeq;     // Last processed sequence number
    std::chrono::steady_clock::time_point lastUpdate;

    ClientState() : x(200.0f), y(300.0f), vx(0.0f), vy(0.0f), lastSeq(0),
        lastUpdate(std::chrono::steady_clock::now()) {}
};

/**
 * @brief Prints detailed error information for socket operations.
 * @param operation The socket operation that failed (e.g., "socket", "bind")
 */
void printSocketError(const char* operation) {
#ifdef _WIN32
    int error = WSAGetLastError();
    std::cerr << operation << " failed with error code: " << error;

    // Provide human-readable error descriptions for common errors
    switch (error) {
    case WSAEADDRINUSE:
        std::cerr << " (Address already in use - port 54000 may be occupied)";
        break;
    case WSAEACCES:
        std::cerr << " (Permission denied - try running as administrator)";
        break;
    case WSAEINVAL:
        std::cerr << " (Invalid parameter)";
        break;
    case WSAENOTSOCK:
        std::cerr << " (Socket operation on non-socket)";
        break;
    default:
        std::cerr << " (Unknown error)";
        break;
    }
#else
    int error = errno;
    std::cerr << operation << " failed with error: " << strerror(error) << " (" << error << ")";

    switch (error) {
    case EADDRINUSE:
        std::cerr << " - Port 54000 is already in use";
        break;
    case EACCES:
        std::cerr << " - Permission denied (try using a different port or run with sudo)";
        break;
    case EBADF:
        std::cerr << " - Invalid socket descriptor";
        break;
    default:
        break;
    }
#endif
    std::cerr << std::endl;
}

/**
 * @brief Gets current timestamp for logging purposes.
 * @return Formatted timestamp string
 */
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
#ifdef _WIN32
    struct tm timeinfo;
    localtime_s(&timeinfo, &time_t);
    ss << std::put_time(&timeinfo, "%H:%M:%S");
#else
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
#endif
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

int main() {
#ifdef _WIN32
    // (1) Initialize Winsock API (required on Windows)
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printSocketError("WSAStartup");
        return 1;
    }
    std::cout << "[" << getCurrentTimestamp() << "] Winsock initialized successfully" << std::endl;
#else
    std::cout << "[" << getCurrentTimestamp() << "] Starting UDP server on Unix-like system" << std::endl;
#endif

    // (2) Create a UDP socket (IPv4, datagram, UDP protocol)
    socket_t sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#ifdef _WIN32
    if (sock == INVALID_SOCKET) {
#else
    if (sock < 0) {
#endif
        printSocketError("socket");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }
    std::cout << "[" << getCurrentTimestamp() << "] UDP socket created successfully" << std::endl;

    // (3) Prepare the server address struct and bind the socket to port 54000
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;   
    serverAddr.sin_port = htons(54000);        

    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr))
#ifdef _WIN32
        == SOCKET_ERROR
#else
        < 0
#endif
        ) {
        printSocketError("bind");
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        return 1;
    }

    std::cout << "[" << getCurrentTimestamp() << "] Server bound to port 54000 and listening..." << std::endl;
    std::cout << "Waiting for client connections..." << std::endl;
    std::cout << "Server Mode: AUTHORITATIVE (processes input and sends back game state)" << std::endl;

    // (4) Buffer and address storage for incoming packets
    char buf[static_cast<int>(Packet::size())];
    sockaddr_in clientAddr;
#ifdef _WIN32
    int clientAddrSize = sizeof(clientAddr);
#else
    socklen_t clientAddrSize = sizeof(clientAddr);
#endif

    uint64_t totalPacketsReceived = 0;
    uint64_t validPacketsProcessed = 0;
    uint64_t invalidPacketsDropped = 0;

    // Client state management
    std::unordered_map<uint32_t, ClientState> clients; 

    // Server constants
    const float MOVE_SPEED = 120.0f;        
    const float BOUNDS_MIN = 30.0f;         
    const float BOUNDS_MAX = 310.0f;        

    // (5) Main server loop: receive, process input, simulate, and send authoritative state
    while (true) {
        clientAddrSize = sizeof(clientAddr);

        int bytes = recvfrom(sock, buf, static_cast<int>(Packet::size()), 0,
            (sockaddr*)&clientAddr, &clientAddrSize);

        if (bytes < 0) {
#ifdef _WIN32
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {  
                printSocketError("recvfrom");
            }
#else
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                printSocketError("recvfrom");
            }
#endif
            continue;
        }

        totalPacketsReceived++;

        if (bytes != static_cast<int>(Packet::size())) {
            std::cerr << "[" << getCurrentTimestamp() << "] WARNING: Received packet with invalid size: "
                << bytes << " bytes (expected " << Packet::size() << " bytes). Packet dropped." << std::endl;
            invalidPacketsDropped++;
            continue;
        }

        // Deserialize the packet
        Packet inputPacket;
        inputPacket.deserialize(buf);

        // Basic packet validation
        if (inputPacket.seq == 0) {
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
            std::cerr << "[" << getCurrentTimestamp() << "] WARNING: Invalid packet received from "
                << clientIP << ":" << ntohs(clientAddr.sin_port)
                << " (seq=0). Packet dropped." << std::endl;
            invalidPacketsDropped++;
            continue;
        }

        validPacketsProcessed++;

        uint32_t clientId = clientAddr.sin_addr.s_addr;

        auto& client = clients[clientId];
        auto now = std::chrono::steady_clock::now();

        if (inputPacket.seq > client.lastSeq) {
            float dt = std::chrono::duration<float>(now - client.lastUpdate).count();
            dt = std::clamp(dt, 0.0f, 0.1f); // Sanity check: max 100ms per frame

            float inputX = std::clamp(inputPacket.x, -1.0f, 1.0f);
            float inputY = std::clamp(inputPacket.y, -1.0f, 1.0f);

            client.vx = inputX * MOVE_SPEED;
            client.vy = inputY * MOVE_SPEED;

            client.x += client.vx * dt;
            client.y += client.vy * dt;

            client.x = std::clamp(client.x, BOUNDS_MIN, BOUNDS_MAX);
            client.y = std::clamp(client.y, BOUNDS_MIN, BOUNDS_MAX);

            client.lastSeq = inputPacket.seq;
            client.lastUpdate = now;

            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);

            std::cout << "[" << getCurrentTimestamp() << "] Processed input from " << clientIP << ":" << ntohs(clientAddr.sin_port)
                << " seq=" << inputPacket.seq << " input=(" << std::fixed << std::setprecision(2)
                << inputX << "," << inputY << ") -> pos=(" << client.x << "," << client.y << ")" << std::endl;
        }

        Packet responsePacket;
        responsePacket.seq = inputPacket.seq;  // Echo back the sequence number for reconciliation
        responsePacket.x = client.x;           // Server's authoritative position
        responsePacket.y = client.y;
        responsePacket.vx = client.vx;         // Server's computed velocity
        responsePacket.vy = client.vy;

        responsePacket.serialize(buf);
        int sentBytes = sendto(sock, buf, static_cast<int>(Packet::size()), 0,
            (sockaddr*)&clientAddr, clientAddrSize);

        if (sentBytes < 0) {
            printSocketError("sendto");
        }
        else if (sentBytes != static_cast<int>(Packet::size())) {
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
            std::cerr << "[" << getCurrentTimestamp() << "] WARNING: Partial send to " << clientIP
                << " (" << sentBytes << "/" << Packet::size() << " bytes)" << std::endl;
        }

        if (totalPacketsReceived % 100 == 0) {
            double validRate = (double)validPacketsProcessed / totalPacketsReceived * 100.0;
            std::cout << "[" << getCurrentTimestamp() << "] Statistics: "
                << totalPacketsReceived << " total, "
                << validPacketsProcessed << " valid (" << std::setprecision(1) << validRate << "%), "
                << invalidPacketsDropped << " dropped, "
                << clients.size() << " active clients" << std::endl;
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