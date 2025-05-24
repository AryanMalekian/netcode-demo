/**
 * @file client.cpp
 * @brief UDP client for advanced netcode demonstration with prediction, reconciliation, and artificial network delay.
 *
 * This interactive demo visualizes the effects of network latency and different prediction strategies for real-time multiplayer games.
 * It sends the local player's movement to a server via UDP, receives back the authoritative server state, and demonstrates:
 *
 * Features demonstrated:
 * - Cross-platform UDP sockets (Winsock/POSIX support)
 * - **Multithreaded architecture**: Separate network thread for sending/receiving packets
 * - **Thread-safe communication**: Lock-free queues for inter-thread packet exchange
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
 *   - Section 5: Interpolation (orange)
 *
 * Threading model:
 *   - Main thread: Handles rendering, input, and game logic at 60 FPS
 *   - Network thread: Manages all UDP communication independently
 *   - Communication via thread-safe queues ensures no blocking between threads
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
#include <atomic>
#include <mutex>
#include <queue>
#include "netcode/common/packet.hpp"
#include "netcode/common/prediction.hpp"
#include "netcode/common/interpolation.hpp"
#include "netcode/common/input.hpp"

#include <SFML/Graphics.hpp>

// -----------------------------------------------------------------------------
// Thread-safe packet queue for inter-thread communication

/**
 * @class ThreadSafeQueue
 * @brief A simple thread-safe queue for passing packets between threads.
 */
template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex mutex_;
    std::queue<T> queue_;
    size_t maxSize_ = 1000;  // Prevent unbounded growth

public:
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.size() < maxSize_) {
            queue_.push(item);
        }
    }

    bool pop(T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return false;
        item = queue_.front();
        queue_.pop();
        return true;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
};

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
    mutable std::mutex mutex_;

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
        std::lock_guard<std::mutex> lock(mutex_);
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
        std::lock_guard<std::mutex> lock(mutex_);
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
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        queue.clear();
    }
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
// Network thread function

/**
 * @brief Network thread that handles all UDP communication.
 * @param sock UDP socket
 * @param servAddr Server address
 * @param outgoingQueue Queue for packets to send
 * @param incomingQueue Queue for received packets
 * @param running Flag to control thread lifecycle
 * @param stats Shared statistics structure
 */
struct NetworkStats {
    std::atomic<int> packetsSent{ 0 };
    std::atomic<int> packetsReceived{ 0 };
    std::atomic<int> sendErrors{ 0 };
    std::atomic<int> invalidPacketsReceived{ 0 };
    std::atomic<float> avgRTT{ 100.0f };
    std::chrono::steady_clock::time_point lastServerPacketTime;
    std::mutex timeMutex;
};

void networkThread(socket_t sock, sockaddr_in servAddr,
    ThreadSafeQueue<Packet>& outgoingQueue,
    ThreadSafeQueue<Packet>& incomingQueue,
    std::atomic<bool>& running,
    NetworkStats& stats) {

    // Buffers for sending/receiving
    char buf[Packet::size()];
    sockaddr_in fromAddr;
#ifdef _WIN32
    int fromSize = sizeof(fromAddr);
#else
    socklen_t fromSize = sizeof(fromAddr);
#endif

    // Delay simulators (thread-local)
    DelaySimulator outgoingDelay;
    DelaySimulator incomingDelay;

    auto lastSendTime = std::chrono::steady_clock::now();

    std::cout << "[Network Thread] Started successfully" << std::endl;

    while (running) {
        auto now = std::chrono::steady_clock::now();

        // Process outgoing packets from queue
        Packet outPacket;
        if (outgoingQueue.pop(outPacket)) {
            outPacket.serialize(buf);
            outgoingDelay.send(buf, Packet::size(), servAddr, sizeof(servAddr));
            lastSendTime = now;
        }

        // Send delayed packets
        sockaddr_in delayedAddr;
        int delayedAddrLen;
        char delayedBuf[Packet::size()];
        while (outgoingDelay.getReady(delayedBuf, Packet::size(), delayedAddr, delayedAddrLen)) {
            int result = sendto(sock, delayedBuf, static_cast<int>(Packet::size()), 0,
                (sockaddr*)&delayedAddr, delayedAddrLen);
            if (result < 0) {
                printSocketError("sendto");
                stats.sendErrors++;
            }
            else {
                stats.packetsSent++;
            }
        }

        // Receive packets
        int bytes = recvfrom(sock, buf, static_cast<int>(Packet::size()), 0,
            (sockaddr*)&fromAddr, &fromSize);
        if (bytes == static_cast<int>(Packet::size())) {
            incomingDelay.send(buf, Packet::size(), fromAddr, fromSize);
        }
        else if (bytes > 0) {
            stats.invalidPacketsReceived++;
        }

        // Process delayed incoming packets
        sockaddr_in incomingAddr;
        int incomingAddrLen;
        char incomingBuf[Packet::size()];
        while (incomingDelay.getReady(incomingBuf, Packet::size(), incomingAddr, incomingAddrLen)) {
            Packet receivedPacket;
            receivedPacket.deserialize(incomingBuf);

            if (receivedPacket.isValid()) {
                incomingQueue.push(receivedPacket);
                stats.packetsReceived++;

                // Update RTT
                float rtt = std::chrono::duration<float>(now - lastSendTime).count() * 1000.0f;
                float currentAvg = stats.avgRTT.load();
                stats.avgRTT = currentAvg * 0.9f + rtt * 0.1f;

                // Update last packet time
                {
                    std::lock_guard<std::mutex> lock(stats.timeMutex);
                    stats.lastServerPacketTime = now;
                }
            }
            else {
                stats.invalidPacketsReceived++;
            }
        }

        // Small sleep to prevent CPU spinning
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "[Network Thread] Shutting down..." << std::endl;
}

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

    // (4) Thread communication setup
    ThreadSafeQueue<Packet> outgoingPackets;
    ThreadSafeQueue<Packet> incomingPackets;
    std::atomic<bool> networkThreadRunning{ true };
    NetworkStats networkStats;

    // (5) Start network thread
    std::thread netThread(networkThread, sock, servAddr,
        std::ref(outgoingPackets), std::ref(incomingPackets),
        std::ref(networkThreadRunning), std::ref(networkStats));

    std::cout << "[" << getCurrentTimestamp() << "] Network thread started" << std::endl;

    // (6) Simulation state - start at center of play area
    uint32_t seq = 1; // Start from 1 (0 is invalid for packet validation)
    float x = 200, y = 300; // Center of play area

    // (7) Advanced prediction system (input buffering and reconciliation)
    PredictionSystem advancedPrediction(x, y);

    // (8) Server packet history for interpolation - initialize with starting position
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

    // (9) SFML window and visual setup (five sections for comparison)
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Advanced Netcode Demo - Multithreaded");
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
    instructionsText.setString("Use Arrow Keys to move | Press C to clear trails | Multithreaded networking demonstration");

    // Connection status text
    sf::Text statusText("", font, 16);
    statusText.setPosition(10, 120);

    // Threading info text
    sf::Text threadingText("", font, 14);
    threadingText.setPosition(10, 90);
    threadingText.setFillColor(sf::Color::Cyan);

    std::cout << "[" << getCurrentTimestamp() << "] Client initialization complete. Starting main loop..." << std::endl;

    // (10) Main loop: simulate, communicate, predict, reconcile, visualize
    auto frameStart = std::chrono::steady_clock::now();
    auto lastSendTime = std::chrono::steady_clock::now();
    bool serverConnected = false;

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
        std::chrono::steady_clock::time_point lastServerTime;
        {
            std::lock_guard<std::mutex> lock(networkStats.timeMutex);
            lastServerTime = networkStats.lastServerPacketTime;
        }
        auto timeSinceLastPacket = std::chrono::duration<float>(now - lastServerTime).count();
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

        // e) Send packet to network thread (with rate limiting)
        auto timeSinceLastSend = std::chrono::duration<float>(now - lastSendTime).count();
        if (timeSinceLastSend >= 0.033f && !advancedPrediction.shouldThrottle()) {  // ~30Hz send rate
            Packet p{ seq++, x, y, vx, vy };

            // Validate packet before sending
            if (p.isValid()) {
                outgoingPackets.push(p);
                lastSendTime = now;
            }
        }

        // f) Process incoming packets from network thread
        Packet receivedPacket;
        while (incomingPackets.pop(receivedPacket)) {
            prevPacket = nextPacket;
            prevRecvTime = nextRecvTime;

            nextPacket = receivedPacket;
            nextRecvTime = now;
            hasPrev = true;

            advancedPrediction.reconcileWithServer(nextPacket);
        }

        // g) Naive prediction: simple extrapolation from last server packet
        std::chrono::duration<float> elapsed = now - nextRecvTime;
        auto naivePredicted = predictPosition(nextPacket, elapsed.count());

        // h) Interpolation between last two server packets
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

        // i) Update trails for visualization
        localTrail.addPosition(x + 10, y + sectionY);
        remoteTrail.addPosition(nextPacket.x + 10 + sectionWidth, nextPacket.y + sectionY);
        naiveTrail.addPosition(naivePredicted.first + 10 + 2 * sectionWidth, naivePredicted.second + sectionY);
        advancedTrail.addPosition(advPredPos.first + 10 + 3 * sectionWidth, advPredPos.second + sectionY);
        interpTrail.addPosition(interpX + 10 + 4 * sectionWidth, interpY + sectionY);

        // j) Update live metrics text
        std::stringstream metrics;
        metrics << std::fixed << std::setprecision(1);
        metrics << "Network Statistics:\n";
        metrics << "FPS: " << (1.0f / frameDt) << " | ";
        metrics << "RTT: " << networkStats.avgRTT.load() << " ms | ";
        metrics << "Packets Sent: " << networkStats.packetsSent.load() << " | ";
        metrics << "Packets Received: " << networkStats.packetsReceived.load() << " | ";
        metrics << "Invalid Packets: " << networkStats.invalidPacketsReceived.load() << " | ";
        metrics << "Send Errors: " << networkStats.sendErrors.load() << "\n";

        int sent = networkStats.packetsSent.load();
        int received = networkStats.packetsReceived.load();
        metrics << "Packet Loss: " <<
            ((sent > 0) ? (100.0f * (1.0f - (float)received / sent)) : 0.0f) << "% | ";
        metrics << "Unacked Inputs: " << advancedPrediction.getUnackedInputCount() << " | ";
        metrics << "Network Queue: " << outgoingPackets.size() << " out / " << incomingPackets.size() << " in";
        metricsText.setString(metrics.str());

        // k) Update connection status
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

        // Update threading info
        std::stringstream threadInfo;
        threadInfo << "Threading: Main thread (rendering @ " << (int)(1.0f / frameDt) << " FPS) | Network thread (active)";
        threadingText.setString(threadInfo.str());

        // l) Place all dots in their visual sections
        localDot.setPosition(x + 10 - dotRadius, y + sectionY - dotRadius);
        remoteDot.setPosition(nextPacket.x + 10 + sectionWidth - dotRadius, nextPacket.y + sectionY - dotRadius);
        naivePredictedDot.setPosition(naivePredicted.first + 10 + 2 * sectionWidth - dotRadius,
            naivePredicted.second + sectionY - dotRadius);
        advPredictedDot.setPosition(advPredPos.first + 10 + 3 * sectionWidth - dotRadius,
            advPredPos.second + sectionY - dotRadius);
        interpDot.setPosition(interpX + 10 + 4 * sectionWidth - dotRadius, interpY + sectionY - dotRadius);

        // m) Render all visualization layers
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
            window.draw(threadingText);
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

        // n) Frame rate limiting (in addition to SFML's limiter)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }

    // (11) Cleanup
    std::cout << "[" << getCurrentTimestamp() << "] Shutting down client..." << std::endl;

    // Signal network thread to stop
    networkThreadRunning = false;
    netThread.join();

    std::cout << "Final statistics:" << std::endl;
    std::cout << "  Packets sent: " << networkStats.packetsSent.load() << std::endl;
    std::cout << "  Packets received: " << networkStats.packetsReceived.load() << std::endl;
    std::cout << "  Invalid packets: " << networkStats.invalidPacketsReceived.load() << std::endl;
    std::cout << "  Send errors: " << networkStats.sendErrors.load() << std::endl;

    int finalSent = networkStats.packetsSent.load();
    int finalReceived = networkStats.packetsReceived.load();
    if (finalSent > 0) {
        std::cout << "  Packet loss rate: " << std::setprecision(2) <<
            (100.0f * (1.0f - (float)finalReceived / finalSent)) << "%" << std::endl;
    }

#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    return 0;
}