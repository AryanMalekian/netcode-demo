/**
 * @file client.cpp
 * @brief UDP client for advanced netcode demonstration with prediction, reconciliation, and artificial network delay.
 *
 * This interactive demo visualizes the effects of network latency and different prediction strategies for real-time multiplayer games.
 * It sends the local player's movement to a server via UDP, receives back the authoritative server state, and demonstrates:
 *
 * Features demonstrated:
 * - Cross-platform UDP sockets (Winsock/POSIX support)
 * - **Artificial network delay simulation** (both outbound and inbound)
 * - Use of a compact, serializable Packet struct for network communication
 * - **Naive prediction**: simple linear extrapolation for client-side prediction
 * - **Advanced prediction**: input buffering, replay, and server reconciliation with PredictionSystem
 * - **Server reconciliation**: correcting mispredictions by rolling back to server state and reapplying inputs, with smooth error correction
 * - **Client-side interpolation**: visual interpolation between server packets for smooth rendering
 * - **Live performance metrics**: FPS, RTT, packet loss, and buffered input count shown live in the window
 * - **Sectioned visualization**: Simultaneous side-by-side comparison of local, server, naive-predicted, and advanced-predicted positions
 * - **Trail visualization**: Each dot leaves a faded trail to show movement history and delay effects
 * - **Interactive UI**: Press [C] to clear all trails. Arrow keys move the local object.
 * - **Robust error handling**: Comprehensive validation and error reporting
 *
 * Visualization legend:
 *   - Section 1: Local input (green)
 *   - Section 2: Server state (red)
 *   - Section 3: Naive prediction (blue)
 *   - Section 4: Advanced prediction (magenta)
 *   - Interpolation (orange) shown in a separate area
 *
 * This version is suitable for educational and research use in real-time multiplayer networking and latency compensation.
 *
 * @author Aryan Malekian
 * @date 23.05.2025
 */

#define NOMINMAX

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
typedef int socket_t;
#endif

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <cmath>
#include <iomanip>
#include <deque>
#include <random>
#include "netcode/common/packet.hpp"
#include "netcode/common/prediction.hpp"
#include "netcode/common/interpolation.hpp"
#include "netcode/common/input.hpp"

#include <SFML/Graphics.hpp>

// -----------------------------------------------------------------------------
// Enhanced error handling utilities

/**
 * @brief Prints detailed error information for socket operations.
 * @param operation The socket operation that failed (e.g., "socket", "connect")
 */
void printSocketError(const char* operation) {
#ifdef _WIN32
    int error = WSAGetLastError();
    std::cerr << "[ERROR] " << operation << " failed with error code: " << error;

    switch (error) {
    case WSAEADDRINUSE:
        std::cerr << " (Address already in use)";
        break;
    case WSAECONNREFUSED:
        std::cerr << " (Connection refused - is server running?)";
        break;
    case WSAENETUNREACH:
        std::cerr << " (Network unreachable)";
        break;
    case WSAETIMEDOUT:
        std::cerr << " (Operation timed out)";
        break;
    case WSAEWOULDBLOCK:
        std::cerr << " (Operation would block - non-blocking socket)";
        break;
    default:
        std::cerr << " (Unknown error)";
        break;
    }
#else
    int error = errno;
    std::cerr << "[ERROR] " << operation << " failed: " << strerror(error) << " (" << error << ")";

    switch (error) {
    case ECONNREFUSED:
        std::cerr << " - Is the server running on port 54000?";
        break;
    case ENETUNREACH:
        std::cerr << " - Network unreachable";
        break;
    case ETIMEDOUT:
        std::cerr << " - Connection timed out";
        break;
    case EWOULDBLOCK:
    case EAGAIN:
        // This is normal for non-blocking sockets, don't print error
        return;
    default:
        break;
    }
#endif
    std::cerr << std::endl;
}

/**
 * @brief Gets current timestamp for logging.
 * @return Formatted timestamp string
 */
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
#ifdef _WIN32
    // Use thread-safe localtime_s on Windows
    struct tm timeinfo;
    localtime_s(&timeinfo, &time_t);
    ss << std::put_time(&timeinfo, "%H:%M:%S");
#else
    // Use localtime on Unix systems
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
#endif
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

// -----------------------------------------------------------------------------
// Artificial network delay simulation utilities

/// @brief Minimum and maximum artificial delay in milliseconds for simulated network lag.
constexpr int MIN_DELAY_MS = 80;
constexpr int MAX_DELAY_MS = 180;

/**
 * @brief Represents a packet scheduled for delayed send/receive to simulate network lag.
 */
struct DelayedPacket {
    std::vector<char> data;
    std::chrono::steady_clock::time_point releaseTime;
    sockaddr_in addr;
    int addrlen;

    DelayedPacket(const char* buf, size_t len, sockaddr_in a, int alen, std::chrono::steady_clock::time_point rel)
        : data(buf, buf + len), releaseTime(rel), addr(a), addrlen(alen) {}
};

/**
 * @class DelaySimulator
 * @brief Buffers packets for artificial network delay and releases them at the appropriate time.
 */
class DelaySimulator {
    std::deque<DelayedPacket> queue;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;

public:
    DelaySimulator() : rng(std::random_device{}()), dist(MIN_DELAY_MS, MAX_DELAY_MS) {}

    /**
     * @brief Schedules a packet to be released after a random network delay.
     * @param buf   Packet data buffer
     * @param len   Buffer size
     * @param addr  Target address
     * @param addrlen Length of sockaddr_in
     */
    void send(const char* buf, size_t len, sockaddr_in addr, int addrlen) {
        int delay = dist(rng);
        auto release = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay);
        queue.emplace_back(buf, len, addr, addrlen, release);
    }

    /**
     * @brief Retrieves a ready-to-send packet whose delay has expired.
     * @param buf   Output buffer to fill with packet data
     * @param len   Buffer size
     * @param addr  Output: address for packet
     * @param addrlen Output: address length
     * @return      True if a packet is ready, false otherwise
     */
    bool getReady(char* buf, size_t len, sockaddr_in& addr, int& addrlen) {
        if (!queue.empty() && queue.front().releaseTime <= std::chrono::steady_clock::now()) {
            const auto& pkt = queue.front();
            size_t toCopy = std::min(len, pkt.data.size());
            std::copy(pkt.data.begin(), pkt.data.begin() + toCopy, buf);
            addr = pkt.addr;
            addrlen = pkt.addrlen;
            queue.pop_front();
            return true;
        }
        return false;
    }

    /// @brief Clears all buffered packets (for debugging or tests)
    void clear() { queue.clear(); }
};

// -----------------------------------------------------------------------------
// Trail visualization for movement history

/**
 * @class Trail
 * @brief Maintains a fading movement trail for visualization in SFML.
 */
class Trail {
    std::deque<sf::Vector2f> positions;
    sf::Color color;
    size_t maxSize;

public:
    /**
     * @brief Constructs a new Trail.
     * @param c      Color of the trail
     * @param max    Maximum length of the trail (number of segments)
     */
    Trail(sf::Color c, size_t max = 30) : color(c), maxSize(max) {}

    /**
     * @brief Adds a new position to the trail.
     * @param x X coordinate
     * @param y Y coordinate
     */
    void addPosition(float x, float y) {
        positions.push_back({ x, y });
        if (positions.size() > maxSize) {
            positions.pop_front();
        }
    }

    /**
     * @brief Renders the trail in the given SFML window.
     * @param window Render target
     */
    void draw(sf::RenderWindow& window) {
        if (positions.size() < 2) return;

        for (size_t i = 1; i < positions.size(); ++i) {
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = positions[i - 1];
            line[1].position = positions[i];

            // Fade out older positions
            float alpha = static_cast<float>(i) / positions.size();
            sf::Color fadeColor = color;
            fadeColor.a = static_cast<sf::Uint8>(alpha * 100);

            line[0].color = fadeColor;
            line[1].color = fadeColor;

            window.draw(line);
        }
    }

    /// @brief Clears the trail.
    void clear() { positions.clear(); }
};

// -----------------------------------------------------------------------------

int main() {
#ifdef _WIN32
    // (1) Initialize Winsock API
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printSocketError("WSAStartup");
        return 1;
    }
    std::cout << "[" << getCurrentTimestamp() << "] Winsock initialized successfully" << std::endl;
#else
    std::cout << "[" << getCurrentTimestamp() << "] Starting UDP client on Unix-like system" << std::endl;
#endif

    // (2) Create a UDP socket (non-blocking)
    socket_t sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#ifdef _WIN32
    if (sock == INVALID_SOCKET) {
        printSocketError("socket");
        WSACleanup();
        return 1;
    }
    u_long mode = 1;
    if (ioctlsocket(sock, FIONBIO, &mode) != 0) {
        printSocketError("ioctlsocket (setting non-blocking)");
        closesocket(sock);
        WSACleanup();
        return 1;
    }
#else
    if (sock < 0) {
        printSocketError("socket");
        return 1;
    }
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0 || fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
        printSocketError("fcntl (setting non-blocking)");
        close(sock);
        return 1;
    }
#endif
    std::cout << "[" << getCurrentTimestamp() << "] UDP socket created and set to non-blocking mode" << std::endl;

    // (3) Prepare server address (localhost:54000)
    sockaddr_in servAddr{};
    servAddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr) <= 0) {
        std::cerr << "[ERROR] Invalid server address format" << std::endl;
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        return 1;
    }
    servAddr.sin_port = htons(54000);
    std::cout << "[" << getCurrentTimestamp() << "] Server address configured: 127.0.0.1:54000" << std::endl;

    // (4) Simulation state - start at center of play area
    uint32_t seq = 1; // Start from 1 (0 is invalid for packet validation)
    float x = 200, y = 300; // Center of play area

    // (5) Advanced prediction system (input buffering and reconciliation)
    PredictionSystem advancedPrediction(x, y);

    // (6) Server packet history for interpolation - initialize with starting position
    Packet prevPacket{};
    Packet nextPacket{};
    prevPacket.seq = 0;
    prevPacket.x = x;
    prevPacket.y = y;
    prevPacket.vx = 0;
    prevPacket.vy = 0;
    nextPacket = prevPacket;
    bool hasPrev = false;
    auto prevRecvTime = std::chrono::steady_clock::now();
    auto nextRecvTime = std::chrono::steady_clock::now();

    // (7) Buffers for sending/receiving
    char buf[Packet::size()];
    sockaddr_in fromAddr;
#ifdef _WIN32
    int fromSize = sizeof(fromAddr);
#else
    socklen_t fromSize = sizeof(fromAddr);
#endif

    // (8) Metrics and error tracking
    auto lastSendTime = std::chrono::steady_clock::now();
    float avgRTT = 100.0f;
    int packetsReceived = 0;
    int packetsSent = 0;
    int invalidPacketsReceived = 0;
    int sendErrors = 0;
    auto lastServerPacketTime = std::chrono::steady_clock::now();
    bool serverConnected = false;

    // (9) SFML window and visual setup (five sections for comparison)
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Advanced Netcode Demo - Visual Comparison");
    window.setFramerateLimit(60);

    // Section layout
    const float sectionWidth = 300.f;
    const float sectionHeight = 600.f;
    const float sectionY = 200.f;
    const float dotRadius = 8.f;
    sf::CircleShape localDot(dotRadius);          localDot.setFillColor(sf::Color::Green);
    sf::CircleShape remoteDot(dotRadius);         remoteDot.setFillColor(sf::Color::Red);
    sf::CircleShape naivePredictedDot(dotRadius); naivePredictedDot.setFillColor(sf::Color::Blue);
    sf::CircleShape advPredictedDot(dotRadius);   advPredictedDot.setFillColor(sf::Color::Magenta);
    sf::CircleShape interpDot(dotRadius);         interpDot.setFillColor(sf::Color(255, 165, 0));

    // Section backgrounds (5 sections now)
    sf::RectangleShape sections[5];
    for (int i = 0; i < 5; ++i) {
        sections[i].setSize({ sectionWidth - 10, sectionHeight });
        sections[i].setPosition(10 + i * sectionWidth, sectionY);
        sections[i].setFillColor(sf::Color(30, 30, 30));
        sections[i].setOutlineThickness(2);
        sections[i].setOutlineColor(sf::Color(100, 100, 100));
    }

    // Movement trails
    Trail localTrail(sf::Color::Green);
    Trail remoteTrail(sf::Color::Red);
    Trail naiveTrail(sf::Color::Blue);
    Trail advancedTrail(sf::Color::Magenta);
    Trail interpTrail(sf::Color(255, 165, 0));

    // Artificial network delay
    DelaySimulator outgoingDelay;
    DelaySimulator incomingDelay;

    // Load font for UI
    sf::Font font;
    bool fontLoaded = false;
    const char* fontPaths[] = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/Arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "arial.ttf",
        "fonts/arial.ttf"
    };
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            std::cout << "[" << getCurrentTimestamp() << "] Font loaded: " << path << std::endl;
            break;
        }
    }
    if (!fontLoaded) {
        std::cout << "[" << getCurrentTimestamp() << "] Warning: Could not load any font file" << std::endl;
    }

    // Section labels
    sf::Text sectionLabels[5];
    const char* labelTexts[] = {
        "Local Input\n(Your Position)",
        "Server State\n(Authoritative)",
        "Naive Prediction\n(Simple Extrapolation)",
        "Advanced Prediction\n(With Reconciliation)",
        "Interpolation\n(Smooth Between Updates)"
    };
    for (int i = 0; i < 5; ++i) {
        sectionLabels[i].setFont(font);
        sectionLabels[i].setString(labelTexts[i]);
        sectionLabels[i].setCharacterSize(16);
        sectionLabels[i].setFillColor(sf::Color::White);
        sectionLabels[i].setPosition(10 + i * sectionWidth + 10, sectionY - 40);
    }

    // Metrics and instructions
    sf::Text metricsText("", font, 14);
    metricsText.setPosition(10, 10);
    metricsText.setFillColor(sf::Color::White);

    sf::Text instructionsText("", font, 14);
    instructionsText.setPosition(10, 850);
    instructionsText.setFillColor(sf::Color(200, 200, 200));
    instructionsText.setString("Use Arrow Keys to move | Press C to clear trails | Watch how different prediction methods handle network latency");

    // Connection status text
    sf::Text statusText("", font, 16);
    statusText.setPosition(10, 120);

    std::cout << "[" << getCurrentTimestamp() << "] Client initialization complete. Starting main loop..." << std::endl;

    // (10) Main loop: simulate, communicate, predict, reconcile, visualize
    auto frameStart = std::chrono::steady_clock::now();

    while (window.isOpen()) {
        auto now = std::chrono::steady_clock::now();
        float frameDt = std::chrono::duration<float>(now - frameStart).count();
        frameStart = now;

        // a) SFML event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
                // Clear all movement trails
                localTrail.clear();
                remoteTrail.clear();
                naiveTrail.clear();
                advancedTrail.clear();
                interpTrail.clear();
                std::cout << "[" << getCurrentTimestamp() << "] Trails cleared by user" << std::endl;
            }
        }

        // b) Check server connection status
        auto timeSinceLastPacket = std::chrono::duration<float>(now - lastServerPacketTime).count();
        bool wasConnected = serverConnected;
        serverConnected = (timeSinceLastPacket < 5.0f); // 5 second timeout

        if (wasConnected != serverConnected) {
            if (serverConnected) {
                std::cout << "[" << getCurrentTimestamp() << "] Server connection established" << std::endl;
            }
            else {
                std::cout << "[" << getCurrentTimestamp() << "] Server connection lost (timeout)" << std::endl;
            }
        }

        // c) Gather keyboard input
        float inputX = 0.0f, inputY = 0.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  inputX = 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))   inputX = -1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))   inputY = 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))     inputY = -1.0f;

        // d) Advanced prediction: buffer input, predict forward
        InputCommand input(seq, inputX, inputY, frameDt);
        advancedPrediction.applyInput(input);

        // Update (applies smooth error correction if needed)
        advancedPrediction.update(frameDt);
        auto advPredPos = advancedPrediction.getPredictedPosition();
        x = advPredPos.first;
        y = advPredPos.second;

        // Keep within play area bounds
        x = std::clamp(x, 20.f, sectionWidth - 20.f);
        y = std::clamp(y, 20.f, sectionHeight - 20.f);

        auto advPredVel = advancedPrediction.getPredictedVelocity();
        float vx = advPredVel.first;
        float vy = advPredVel.second;

        // e) Send packet to server (with artificial delay)
        auto timeSinceLastSend = std::chrono::duration<float>(now - lastSendTime).count();
        if (timeSinceLastSend >= 0.033f && !advancedPrediction.shouldThrottle()) {  // ~30Hz send rate
            Packet p{ seq++, x, y, vx, vy };

            // Validate packet before sending
            if (!p.isValid()) {
                std::cerr << "[" << getCurrentTimestamp() << "] ERROR: Attempted to send invalid packet (seq="
                    << p.seq << " x=" << p.x << " y=" << p.y << " vx=" << p.vx << " vy=" << p.vy << ")" << std::endl;
            }
            else {
                p.serialize(buf);
                outgoingDelay.send(buf, Packet::size(), servAddr, sizeof(servAddr));
                lastSendTime = now;
                packetsSent++;
            }
        }

        // Outbound delay: actually send packets when delay expires
        sockaddr_in delayedAddr;
        int delayedAddrLen;
        char delayedBuf[Packet::size()];
        while (outgoingDelay.getReady(delayedBuf, Packet::size(), delayedAddr, delayedAddrLen)) {
            int result = sendto(sock, delayedBuf, static_cast<int>(Packet::size()), 0,
                (sockaddr*)&delayedAddr, delayedAddrLen);
            if (result < 0) {
                printSocketError("sendto");
                sendErrors++;
            }
            else if (result != static_cast<int>(Packet::size())) {
                std::cerr << "[" << getCurrentTimestamp() << "] WARNING: Partial send ("
                    << result << "/" << Packet::size() << " bytes)" << std::endl;
            }
        }

        // f) Receive from server (with artificial inbound delay)
        int bytes = recvfrom(sock, buf, static_cast<int>(Packet::size()), 0,
            (sockaddr*)&fromAddr, &fromSize);
        if (bytes == static_cast<int>(Packet::size())) {
            incomingDelay.send(buf, Packet::size(), fromAddr, fromSize);
        }
        else if (bytes > 0) {
            std::cerr << "[" << getCurrentTimestamp() << "] WARNING: Received packet with wrong size: "
                << bytes << " bytes (expected " << Packet::size() << ")" << std::endl;
            invalidPacketsReceived++;
        }
        else if (bytes < 0) {
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
        }

        // g) Process delayed inbound packets ("receive" when delay expires)
        bool gotDelayedPacket = false;
        sockaddr_in incomingAddr;
        int incomingAddrLen;
        char incomingBuf[Packet::size()];
        while (incomingDelay.getReady(incomingBuf, Packet::size(), incomingAddr, incomingAddrLen)) {
            Packet receivedPacket;
            receivedPacket.deserialize(incomingBuf);

            // Validate received packet
            if (!receivedPacket.isValid()) {
                std::cerr << "[" << getCurrentTimestamp() << "] WARNING: Received invalid packet from server (seq="
                    << receivedPacket.seq << " x=" << receivedPacket.x << " y=" << receivedPacket.y
                    << " vx=" << receivedPacket.vx << " vy=" << receivedPacket.vy << "). Packet dropped." << std::endl;
                invalidPacketsReceived++;
                continue;
            }

            gotDelayedPacket = true;
            packetsReceived++;
            lastServerPacketTime = now;

            float rtt = std::chrono::duration<float>(now - lastSendTime).count() * 1000.0f;
            avgRTT = avgRTT * 0.9f + rtt * 0.1f;

            prevPacket = nextPacket;
            prevRecvTime = nextRecvTime;

            nextPacket = receivedPacket;
            nextRecvTime = now;
            hasPrev = true;

            advancedPrediction.reconcileWithServer(nextPacket);
        }

        // h) Naive prediction: simple extrapolation from last server packet
        std::chrono::duration<float> elapsed = now - nextRecvTime;
        auto naivePredicted = predictPosition(nextPacket, elapsed.count());

        // i) Interpolation between last two server packets
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

        // j) Update trails for visualization
        localTrail.addPosition(x + 10, y + sectionY);
        remoteTrail.addPosition(nextPacket.x + 10 + sectionWidth, nextPacket.y + sectionY);
        naiveTrail.addPosition(naivePredicted.first + 10 + 2 * sectionWidth, naivePredicted.second + sectionY);
        advancedTrail.addPosition(advPredPos.first + 10 + 3 * sectionWidth, advPredPos.second + sectionY);
        interpTrail.addPosition(interpX + 10 + 4 * sectionWidth, interpY + sectionY);

        // k) Update live metrics text
        std::stringstream metrics;
        metrics << std::fixed << std::setprecision(1);
        metrics << "Network Statistics:\n";
        metrics << "FPS: " << (1.0f / frameDt) << " | ";
        metrics << "RTT: " << avgRTT << " ms | ";
        metrics << "Packets Sent: " << packetsSent << " | ";
        metrics << "Packets Received: " << packetsReceived << " | ";
        metrics << "Invalid Packets: " << invalidPacketsReceived << " | ";
        metrics << "Send Errors: " << sendErrors << "\n";
        metrics << "Packet Loss: " <<
            ((packetsSent > 0) ? (100.0f * (1.0f - (float)packetsReceived / packetsSent)) : 0.0f) << "% | ";
        metrics << "Unacked Inputs: " << advancedPrediction.getUnackedInputCount();
        metricsText.setString(metrics.str());

        // l) Update connection status
        std::stringstream status;
        if (serverConnected) {
            status << "Status: CONNECTED to server";
            statusText.setFillColor(sf::Color::Green);
        }
        else {
            status << "Status: DISCONNECTED - Check if server is running on port 54000";
            statusText.setFillColor(sf::Color::Red);
        }
        statusText.setString(status.str());

        // m) Place all dots in their visual sections
        localDot.setPosition(x + 10 - dotRadius, y + sectionY - dotRadius);
        remoteDot.setPosition(nextPacket.x + 10 + sectionWidth - dotRadius, nextPacket.y + sectionY - dotRadius);
        naivePredictedDot.setPosition(naivePredicted.first + 10 + 2 * sectionWidth - dotRadius,
            naivePredicted.second + sectionY - dotRadius);
        advPredictedDot.setPosition(advPredPos.first + 10 + 3 * sectionWidth - dotRadius,
            advPredPos.second + sectionY - dotRadius);
        interpDot.setPosition(interpX + 10 + 4 * sectionWidth - dotRadius, interpY + sectionY - dotRadius);

        // n) Render all visualization layers
        window.clear(sf::Color(20, 20, 20));
        for (int i = 0; i < 5; ++i) window.draw(sections[i]);

        localTrail.draw(window);
        remoteTrail.draw(window);
        naiveTrail.draw(window);
        advancedTrail.draw(window);
        interpTrail.draw(window);

        window.draw(localDot);
        window.draw(remoteDot);
        window.draw(naivePredictedDot);
        window.draw(advPredictedDot);
        window.draw(interpDot);

        if (fontLoaded) {
            for (int i = 0; i < 5; ++i) window.draw(sectionLabels[i]);
            window.draw(metricsText);
            window.draw(statusText);
            window.draw(instructionsText);
        }

        // Draw vertical dividers
        for (int i = 1; i < 5; ++i) {
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = sf::Vector2f(i * sectionWidth + 5, sectionY);
            line[1].position = sf::Vector2f(i * sectionWidth + 5, sectionY + sectionHeight);
            line[0].color = sf::Color(70, 70, 70);
            line[1].color = sf::Color(70, 70, 70);
            window.draw(line);
        }

        window.display();

        // o) Frame rate limiting (in addition to SFML's limiter)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }

    // (11) Cleanup
    std::cout << "[" << getCurrentTimestamp() << "] Shutting down client..." << std::endl;
    std::cout << "Final statistics:" << std::endl;
    std::cout << "  Packets sent: " << packetsSent << std::endl;
    std::cout << "  Packets received: " << packetsReceived << std::endl;
    std::cout << "  Invalid packets: " << invalidPacketsReceived << std::endl;
    std::cout << "  Send errors: " << sendErrors << std::endl;
    if (packetsSent > 0) {
        std::cout << "  Packet loss rate: " << std::setprecision(2) <<
            (100.0f * (1.0f - (float)packetsReceived / packetsSent)) << "%" << std::endl;
    }

#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    return 0;
}