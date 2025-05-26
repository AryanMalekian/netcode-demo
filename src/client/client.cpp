/**
 * @file client.cpp
 * @brief UDP client for advanced netcode demonstration with prediction, reconciliation, and network delay simulation.
 *
 * This interactive demo visualizes the effects of network latency and different prediction strategies for real-time multiplayer games.
 * It sends the local player's movement to a server via UDP, receives back the authoritative server state, and demonstrates:
 *
 * Features demonstrated:
 * - Cross-platform UDP sockets (Winsock/POSIX support)
 * - **Multithreaded architecture**: Separate network thread for sending/receiving packets
 * - **Thread-safe communication**: Condition variable queues for inter-thread packet exchange
 * - **Network delay simulation** with configurable latency presets (5-450ms range)
 * - **Latency preset selection**: Choose from predefined network conditions for demonstration
 * - Use of a compact, serializable Packet struct for network communication
 * - **Naive prediction**: simple linear extrapolation for client-side prediction
 * - **Advanced prediction**: input buffering, replay, and server reconciliation with PredictionSystem
 * - **Server reconciliation**: correcting mispredictions by rolling back to server state and reapplying inputs, with smooth error correction
 * - **Client-side interpolation**: visual interpolation between server packets for smooth rendering
 * - **Live performance metrics**: FPS, RTT, packet loss, and buffered input count shown live in the window
 * - **Sectioned visualization**: Simultaneous side-by-side comparison of local, server, naive-predicted, and advanced-predicted positions
 * - **Trail visualization**: Each dot leaves a faded trail to show movement history and delay effects
 * - **Interactive UI**: Press [C] to clear all trails. Arrow keys move the local object. Number keys select latency presets.
 * - **Robust error handling**: Comprehensive validation and error reporting
 * - **Packet loss tracking**: Real packet loss detection via sequence gap analysis
 *
 * NEW CONTROLS:
 *   - 1-5: Select latency preset
 *   - C: Clear trails
 *
 * Visualization legend:
 *   - Section 1: Local input (green)
 *   - Section 2: Server state (red) - AUTHORITATIVE TRUTH
 *   - Section 3: Naive prediction (blue)
 *   - Section 4: Advanced prediction (magenta)
 *   - Section 5: Interpolation (orange)
 *
 * Threading model:
 *   - Main thread: Handles rendering, input, and game logic at 60 FPS
 *   - Network thread: Manages all UDP communication independently with condition variables
 *   - Communication via thread-safe queues ensures no blocking between threads
 *
 * @author Aryan Malekian & Jonathan Skomsøy Hübertz,  w/ use of A.I. Models
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
#include <condition_variable>
#include <algorithm>
#include "netcode/common/packet.hpp"
#include "netcode/common/prediction.hpp"
#include "netcode/common/interpolation.hpp"
#include "netcode/common/input.hpp"

#include <SFML/Graphics.hpp>

// -----------------------------------------------------------------------------
// Latency preset system

/**
 * @struct LatencyPreset
 * @brief Predefined latency configurations for demonstration purposes
 */
struct LatencyPreset {
    std::string name;
    int minDelay;
    int maxDelay;
    sf::Color displayColor;

    LatencyPreset(const std::string& n, int min, int max, sf::Color color)
        : name(n), minDelay(min), maxDelay(max), displayColor(color) {}
};

/**
 * @class LatencyPresetManager
 * @brief Manages predefined latency presets for easy network condition comparison
 */
class LatencyPresetManager {
public:
    std::vector<LatencyPreset> presets;
    std::atomic<int> currentPresetIndex{ 2 }; // Start with "Normal" preset

    LatencyPresetManager() {
        presets.emplace_back("5-15ms (LAN)", 5, 15, sf::Color::Green);
        presets.emplace_back("30-60ms (Fast)", 30, 60, sf::Color::Cyan);
        presets.emplace_back("80-180ms (Normal)", 80, 180, sf::Color::White);
        presets.emplace_back("150-300ms (Slow)", 150, 300, sf::Color::Yellow);
        presets.emplace_back("250-450ms (Bad)", 250, 450, sf::Color(255, 165, 0)); // Orange
    }

    void selectPreset(int index) {
        if (index >= 0 && index < static_cast<int>(presets.size())) {
            currentPresetIndex = index;
        }
    }

    const LatencyPreset& getCurrentPreset() const {
        return presets[currentPresetIndex.load()];
    }

    std::pair<int, int> getCurrentDelayRange() const {
        const auto& preset = getCurrentPreset();
        return { preset.minDelay, preset.maxDelay };
    }
};

// -----------------------------------------------------------------------------
// Thread-safe packet queue with condition variables for inter-thread communication

/**
 * @class ThreadSafeQueue
 * @brief A thread-safe queue with condition variables for efficient packet passing between threads.
 */
template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<T> queue_;
    size_t maxSize_ = 1000;

public:
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.size() < maxSize_) {
            queue_.push(item);
            cv_.notify_one(); // Wake waiting thread
        }
    }

    bool pop(T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return false;
        item = queue_.front();
        queue_.pop();
        return true;
    }

    bool waitAndPop(T& item, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (cv_.wait_for(lock, timeout, [this] { return !queue_.empty(); })) {
            item = queue_.front();
            queue_.pop();
            return true;
        }
        return false;
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
    case EAGAIN:
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
    struct tm timeinfo;
    localtime_s(&timeinfo, &time_t);
    ss << std::put_time(&timeinfo, "%H:%M:%S");
#else
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
#endif
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

// -----------------------------------------------------------------------------
// Network delay simulation utilities

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
 * @brief Buffers packets for network delay simulation and releases them at the appropriate time.
 */
class DelaySimulator {
    std::deque<DelayedPacket> queue;
    std::mt19937 rng;
    mutable std::mutex mutex_;
    LatencyPresetManager& presetManager;

public:
    DelaySimulator(LatencyPresetManager& manager) : rng(std::random_device{}()), presetManager(manager) {}

    /**
     * @brief Schedules a packet to be released after a random network delay.
     * @param buf   Packet data buffer
     * @param len   Buffer size
     * @param addr  Target address
     * @param addrlen Length of sockaddr_in
     */
    void send(const char* buf, size_t len, sockaddr_in addr, int addrlen) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto delayRange = presetManager.getCurrentDelayRange();
        std::uniform_int_distribution<int> dist(delayRange.first, delayRange.second);
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

    void clear() { positions.clear(); }
};

// -----------------------------------------------------------------------------
// Network statistics and thread function

/**
 * @brief Network statistics structure for thread-safe communication
 */
struct NetworkStats {
    std::atomic<int> packetsSent{ 0 };
    std::atomic<int> packetsReceived{ 0 };
    std::atomic<int> sendErrors{ 0 };
    std::atomic<int> invalidPacketsReceived{ 0 };
    std::atomic<int> packetsLost{ 0 };  // Actual packet loss count
    std::atomic<float> avgRTT{ 100.0f };
    std::chrono::steady_clock::time_point lastServerPacketTime;
    std::mutex timeMutex;

    // Sequence tracking for packet loss detection
    uint32_t expectedServerSeq{ 1 };
    std::mutex seqMutex;
};

/**
 * @brief Network thread that handles all UDP communication.
 * @param sock UDP socket
 * @param servAddr Server address
 * @param outgoingQueue Queue for packets to send
 * @param incomingQueue Queue for received packets
 * @param running Flag to control thread lifecycle
 * @param stats Shared statistics structure
 * @param presetManager Latency preset manager for dynamic delay control
 */
void networkThread(socket_t sock, sockaddr_in servAddr,
    ThreadSafeQueue<Packet>& outgoingQueue,
    ThreadSafeQueue<Packet>& incomingQueue,
    std::atomic<bool>& running,
    NetworkStats& stats,
    LatencyPresetManager& presetManager) {

    char buf[Packet::size()];
    sockaddr_in fromAddr;
#ifdef _WIN32
    int fromSize = sizeof(fromAddr);
#else
    socklen_t fromSize = sizeof(fromAddr);
#endif

    DelaySimulator outgoingDelay(presetManager);
    DelaySimulator incomingDelay(presetManager);

    auto lastSendTime = std::chrono::steady_clock::now();

    std::cout << "[Network Thread] Started successfully with condition variables" << std::endl;

    while (running) {
        auto now = std::chrono::steady_clock::now();

        // Wait for outgoing packets with timeout
        Packet outPacket;
        if (outgoingQueue.waitAndPop(outPacket, std::chrono::milliseconds(10))) {
            outPacket.serialize(buf);
            outgoingDelay.send(buf, Packet::size(), servAddr, sizeof(servAddr));
            lastSendTime = now;
        }

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

        int bytes = recvfrom(sock, buf, static_cast<int>(Packet::size()), 0,
            (sockaddr*)&fromAddr, &fromSize);
        if (bytes == static_cast<int>(Packet::size())) {
            incomingDelay.send(buf, Packet::size(), fromAddr, fromSize);
        }
        else if (bytes > 0) {
            stats.invalidPacketsReceived++;
        }

        sockaddr_in incomingAddr;
        int incomingAddrLen;
        char incomingBuf[Packet::size()];
        while (incomingDelay.getReady(incomingBuf, Packet::size(), incomingAddr, incomingAddrLen)) {
            Packet receivedPacket;
            receivedPacket.deserialize(incomingBuf);

            if (receivedPacket.isValid()) {
                // Packet loss detection via sequence gap analysis
                {
                    std::lock_guard<std::mutex> seqLock(stats.seqMutex);
                    if (receivedPacket.seq > stats.expectedServerSeq) {
                        // Gap detected - packets were lost
                        int lostCount = receivedPacket.seq - stats.expectedServerSeq;
                        stats.packetsLost += lostCount;
                        std::cout << "[Network Thread] Detected " << lostCount
                            << " lost packets (gap: " << stats.expectedServerSeq
                            << " to " << receivedPacket.seq << ")" << std::endl;
                    }
                    stats.expectedServerSeq = receivedPacket.seq + 1;
                }

                incomingQueue.push(receivedPacket);
                stats.packetsReceived++;

                float rtt = std::chrono::duration<float>(now - lastSendTime).count() * 1000.0f;
                float currentAvg = stats.avgRTT.load();
                stats.avgRTT = currentAvg * 0.9f + rtt * 0.1f;

                {
                    std::lock_guard<std::mutex> lock(stats.timeMutex);
                    stats.lastServerPacketTime = now;
                }
            }
            else {
                stats.invalidPacketsReceived++;
            }
        }

        // No sleep needed - condition variable handles efficient waiting
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
    LatencyPresetManager presetManager;

    // (5) Start network thread
    std::thread netThread(networkThread, sock, servAddr,
        std::ref(outgoingPackets), std::ref(incomingPackets),
        std::ref(networkThreadRunning), std::ref(networkStats), std::ref(presetManager));

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
    sf::RenderWindow window(sf::VideoMode(1800, 1000), "Advanced Netcode Demo - Multithreaded");
    window.setFramerateLimit(60);

    const float sectionWidth = 340.f;
    const float sectionHeight = 550.f;
    const float sectionY = 280.f;
    const float dotRadius = 10.f;
    sf::CircleShape localDot(dotRadius);          localDot.setFillColor(sf::Color::Green);
    sf::CircleShape remoteDot(dotRadius);         remoteDot.setFillColor(sf::Color::Red);
    sf::CircleShape naivePredictedDot(dotRadius); naivePredictedDot.setFillColor(sf::Color::Blue);
    sf::CircleShape advPredictedDot(dotRadius);   advPredictedDot.setFillColor(sf::Color::Magenta);
    sf::CircleShape interpDot(dotRadius);         interpDot.setFillColor(sf::Color(255, 165, 0));

    sf::RectangleShape sections[5];
    for (int i = 0; i < 5; ++i) {
        sections[i].setSize({ sectionWidth - 20, sectionHeight });
        sections[i].setPosition(20 + i * sectionWidth, sectionY);
        sections[i].setFillColor(sf::Color(25, 25, 25));
        sections[i].setOutlineThickness(2);
        sections[i].setOutlineColor(sf::Color(120, 120, 120));
    }

    Trail localTrail(sf::Color::Green);
    Trail remoteTrail(sf::Color::Red);
    Trail naiveTrail(sf::Color::Blue);
    Trail advancedTrail(sf::Color::Magenta);
    Trail interpTrail(sf::Color(255, 165, 0));

    sf::Font font;
    bool fontLoaded = false;
    const char* fontPaths[] = {
    #ifdef _WIN32
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/Arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/verdana.ttf",
        "C:/Windows/Fonts/tahoma.ttf"
    #elif __APPLE__
        "/System/Library/Fonts/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Avenir.ttc",
        "/System/Library/Fonts/Verdana.ttf",
        "/System/Library/Fonts/Geneva.ttf"
    #else
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/TTF/arial.ttf",
        "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf"
    #endif
    };

    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            std::cout << "[" << getCurrentTimestamp() << "] Font loaded: " << path << std::endl;
            break;
        }
    }

    if (!fontLoaded) {
        const char* fallbackFonts[] = { "arial.ttf", "fonts/arial.ttf", "assets/arial.ttf" };
        for (const auto& path : fallbackFonts) {
            if (font.loadFromFile(path)) {
                fontLoaded = true;
                std::cout << "[" << getCurrentTimestamp() << "] Fallback font loaded: " << path << std::endl;
                break;
            }
        }
    }

    if (!fontLoaded) {
        std::cout << "[" << getCurrentTimestamp() << "] Warning: Could not load any font file - text will use default font" << std::endl;
    }

    sf::Text sectionLabels[5];
    const char* labelTexts[] = {
        "Local Input\n(Immediate Response)",
        "Server State\n(AUTHORITATIVE TRUTH)",
        "Naive Prediction\n(Simple Extrapolation)",
        "Advanced Prediction\n(With Reconciliation)",
        "Interpolation\n(Smooth Server Updates)"
    };
    for (int i = 0; i < 5; ++i) {
        sectionLabels[i].setFont(font);
        sectionLabels[i].setString(labelTexts[i]);
        sectionLabels[i].setCharacterSize(18);
        sectionLabels[i].setFillColor(sf::Color::White);
        sectionLabels[i].setPosition(30 + i * sectionWidth, sectionY - 60);
    }

    sf::Text metricsText("", font, 16);
    metricsText.setPosition(20, 20);
    metricsText.setFillColor(sf::Color::White);

    sf::Text instructionsText("", font, 16);
    instructionsText.setPosition(20, 910);
    instructionsText.setFillColor(sf::Color(220, 220, 220));
    instructionsText.setString("Arrow Keys: move | C: clear trails | 1-5: Select latency preset | Multithreaded networking demonstration");

    sf::Text statusText("", font, 18);
    statusText.setPosition(20, 140);

    sf::Text threadingText("", font, 16);
    threadingText.setPosition(20, 110);
    threadingText.setFillColor(sf::Color::Cyan);

    sf::Text latencyPresetText("", font, 18);
    latencyPresetText.setPosition(20, 170);

    std::vector<sf::RectangleShape> presetBoxes;
    std::vector<sf::Text> presetLabels;
    const float boxWidth = 280.f;
    const float boxHeight = 35.f;
    const float boxSpacing = 20.f;
    const float presetStartY = 850.f;

    for (size_t i = 0; i < presetManager.presets.size(); ++i) {
        sf::RectangleShape box(sf::Vector2f(boxWidth, boxHeight));
        box.setPosition(20 + i * (boxWidth + boxSpacing), presetStartY);
        box.setFillColor(sf::Color(45, 45, 45));
        box.setOutlineThickness(2);
        box.setOutlineColor(presetManager.presets[i].displayColor);
        presetBoxes.push_back(box);

        sf::Text label("", font, 14);
        label.setString(std::to_string(i + 1) + ": " + presetManager.presets[i].name);
        label.setPosition(30 + i * (boxWidth + boxSpacing), presetStartY + 8);
        label.setFillColor(sf::Color::White);
        presetLabels.push_back(label);
    }

    std::cout << "[" << getCurrentTimestamp() << "] Client initialization complete. Starting main loop..." << std::endl;

    // (10) Main loop: simulate, communicate, predict, reconcile, visualize
    auto frameStart = std::chrono::steady_clock::now();
    auto lastSendTime = std::chrono::steady_clock::now();
    bool serverConnected = false;

    while (window.isOpen()) {
        auto now = std::chrono::steady_clock::now();
        float frameDt = std::chrono::duration<float>(now - frameStart).count();
        frameStart = now;

        // a) SFML event handling - Added preset selection
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::C) {
                    localTrail.clear();
                    remoteTrail.clear();
                    naiveTrail.clear();
                    advancedTrail.clear();
                    interpTrail.clear();
                    std::cout << "[" << getCurrentTimestamp() << "] Trails cleared by user" << std::endl;
                }
                else if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num5) {
                    int presetIndex = event.key.code - sf::Keyboard::Num1;
                    if (presetIndex < static_cast<int>(presetManager.presets.size())) {
                        presetManager.selectPreset(presetIndex);
                        std::cout << "[" << getCurrentTimestamp() << "] Selected latency preset: "
                            << presetManager.getCurrentPreset().name << std::endl;
                    }
                }
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
        serverConnected = (timeSinceLastPacket < 10.0f);

        if (wasConnected != serverConnected) {
            if (serverConnected) {
                std::cout << "[" << getCurrentTimestamp() << "] Server connection established" << std::endl;
            }
            else {
                std::cout << "[" << getCurrentTimestamp() << "] Server connection lost (timeout)" << std::endl;
            }
        }

        // c) Gather keyboard input (raw input to send to server)
        float inputX = 0.0f, inputY = 0.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  inputX = 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))   inputX = -1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))   inputY = 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))     inputY = -1.0f;

        // d) Send RAW INPUT to server (server decides position, not client)
        auto timeSinceLastSend = std::chrono::duration<float>(now - lastSendTime).count();
        if (timeSinceLastSend >= 0.033f) {  // ~30Hz send rate

            Packet inputPacket{ seq++, inputX, inputY, 0, 0 };

            if (inputPacket.seq > 0) {
                outgoingPackets.push(inputPacket);
                lastSendTime = now;
            }
        }

        // e) LOCAL INPUT: Apply input immediately for responsive feel (green dot)
        const float MOVE_SPEED = 120.0f;
        float localVx = inputX * MOVE_SPEED;
        float localVy = inputY * MOVE_SPEED;
        x += localVx * frameDt;
        y += localVy * frameDt;

        x = std::clamp(x, 30.f, sectionWidth - 30.f);
        y = std::clamp(y, 30.f, sectionHeight - 30.f);

        // f) ADVANCED PREDICTION: Apply input to prediction system
        InputCommand input(seq - 1, inputX, inputY, frameDt);
        advancedPrediction.applyInput(input);
        advancedPrediction.update(frameDt);
        auto advPredPos = advancedPrediction.getPredictedPosition();

        // g) Process incoming packets from server (SERVER IS AUTHORITATIVE)
        Packet serverPacket;
        while (incomingPackets.pop(serverPacket)) {
            prevPacket = nextPacket;
            prevRecvTime = nextRecvTime;

            nextPacket = serverPacket;
            nextRecvTime = now;
            hasPrev = true;

            advancedPrediction.reconcileWithServer(nextPacket);
        }

        // h) Naive prediction: simple extrapolation from AUTHORITATIVE server packet
        std::chrono::duration<float> elapsed = now - nextRecvTime;
        // Add estimated network latency to show naive prediction error
        auto latencyPreset = presetManager.getCurrentPreset();
        float estimatedLatency = (latencyPreset.minDelay + latencyPreset.maxDelay) / 2000.0f;
        auto naivePredicted = predictPosition(nextPacket, elapsed.count() + estimatedLatency);

        // i) Interpolation between server packets (smooth server state visualization)
        float interpX = nextPacket.x, interpY = nextPacket.y;
        if (hasPrev) {
            std::chrono::duration<float> interval = nextRecvTime - prevRecvTime;
            std::chrono::duration<float> sinceNext = now - nextRecvTime;

            // Interpolate forward from the last received packet
            if (interval.count() > 0.0001f) {
                float t = std::clamp(sinceNext.count() / interval.count(), 0.f, 2.f);
                auto interp = interpolatePosition(prevPacket, nextPacket, t);
                interpX = interp.first;
                interpY = interp.second;
            }
        }

        // j) Update trails for visualization - Adjusted for new spacing
        localTrail.addPosition(x + 30, y + sectionY);
        remoteTrail.addPosition(nextPacket.x + 30 + sectionWidth, nextPacket.y + sectionY);
        naiveTrail.addPosition(naivePredicted.first + 30 + 2 * sectionWidth, naivePredicted.second + sectionY);
        advancedTrail.addPosition(advPredPos.first + 30 + 3 * sectionWidth, advPredPos.second + sectionY);
        interpTrail.addPosition(interpX + 30 + 4 * sectionWidth, interpY + sectionY);

        // k) Update live metrics text
        std::stringstream metrics;
        metrics << std::fixed << std::setprecision(1);
        metrics << "Network Statistics (Server Authoritative):\n";
        metrics << "FPS: " << (1.0f / frameDt) << " | ";
        metrics << "RTT: " << networkStats.avgRTT.load() << " ms | ";
        metrics << "Input Packets Sent: " << networkStats.packetsSent.load() << " | ";
        metrics << "Server Updates Received: " << networkStats.packetsReceived.load() << " | ";
        metrics << "Invalid Packets: " << networkStats.invalidPacketsReceived.load() << " | ";
        metrics << "Send Errors: " << networkStats.sendErrors.load() << "\n";

        int sent = networkStats.packetsSent.load();
        int received = networkStats.packetsReceived.load();
        int lost = networkStats.packetsLost.load();

        metrics << "Packets Lost: " << lost << " packets | ";
        metrics << "Connection Quality: " <<
            ((sent > 0) ? (100.0f * (float)received / sent) : 0.0f) << "% response rate | ";
        metrics << "Unacked Inputs: " << advancedPrediction.getUnackedInputCount() << " | ";
        metrics << "Network Queue: " << outgoingPackets.size() << " out / " << incomingPackets.size() << " in";
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

        std::stringstream threadInfo;
        threadInfo << "Threading: Main thread (rendering @ " << (int)(1.0f / frameDt) << " FPS) | Network thread (event-driven)";
        threadingText.setString(threadInfo.str());

        const auto& currentPreset = presetManager.getCurrentPreset();
        std::stringstream latencyInfo;
        latencyInfo << "Current Network Profile: " << currentPreset.name << " | ";
        latencyInfo << "Latency Range: " << currentPreset.minDelay << "-" <<
            currentPreset.maxDelay << "ms";
        latencyPresetText.setString(latencyInfo.str());
        latencyPresetText.setFillColor(currentPreset.displayColor);

        // m) Place all dots in their visual sections - Adjusted for new spacing
        localDot.setPosition(x + 30 - dotRadius, y + sectionY - dotRadius);
        remoteDot.setPosition(nextPacket.x + 30 + sectionWidth - dotRadius, nextPacket.y + sectionY - dotRadius);
        naivePredictedDot.setPosition(naivePredicted.first + 30 + 2 * sectionWidth - dotRadius,
            naivePredicted.second + sectionY - dotRadius);
        advPredictedDot.setPosition(advPredPos.first + 30 + 3 * sectionWidth - dotRadius,
            advPredPos.second + sectionY - dotRadius);
        interpDot.setPosition(interpX + 30 + 4 * sectionWidth - dotRadius, interpY + sectionY - dotRadius);

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
            window.draw(threadingText);
            window.draw(latencyPresetText);
            window.draw(instructionsText);

            for (size_t i = 0; i < presetBoxes.size(); ++i) {
                if (static_cast<int>(i) == presetManager.currentPresetIndex.load()) {
                    presetBoxes[i].setFillColor(sf::Color(80, 80, 80));
                    presetBoxes[i].setOutlineThickness(3);
                }
                else {
                    presetBoxes[i].setFillColor(sf::Color(45, 45, 45));
                    presetBoxes[i].setOutlineThickness(2);
                }
                window.draw(presetBoxes[i]);
                window.draw(presetLabels[i]);
            }
        }

        for (int i = 1; i < 5; ++i) {
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = sf::Vector2f(i * sectionWidth + 10, sectionY);
            line[1].position = sf::Vector2f(i * sectionWidth + 10, sectionY + sectionHeight);
            line[0].color = sf::Color(80, 80, 80);
            line[1].color = sf::Color(80, 80, 80);
            window.draw(line);
        }

        window.display();
    }

    // (11) Cleanup
    std::cout << "[" << getCurrentTimestamp() << "] Shutting down client..." << std::endl;

    networkThreadRunning = false;
    netThread.join();

    std::cout << "Final statistics:" << std::endl;
    std::cout << "  Packets sent: " << networkStats.packetsSent.load() << std::endl;
    std::cout << "  Packets received: " << networkStats.packetsReceived.load() << std::endl;
    std::cout << "  Packets lost (sequence gaps): " << networkStats.packetsLost.load() << std::endl;
    std::cout << "  Invalid packets: " << networkStats.invalidPacketsReceived.load() << std::endl;
    std::cout << "  Send errors: " << networkStats.sendErrors.load() << std::endl;

    int finalSent = networkStats.packetsSent.load();
    int finalReceived = networkStats.packetsReceived.load();
    int finalLost = networkStats.packetsLost.load();

    if (finalSent > 0) {
        std::cout << "  Connection response rate: " << std::setprecision(2) <<
            (100.0f * (float)finalReceived / finalSent) << "%" << std::endl;
    }
    if (finalReceived > 0) {
        std::cout << "  Actual packet loss rate: " << std::setprecision(2) <<
            (100.0f * (float)finalLost / (finalReceived + finalLost)) << "%" << std::endl;
    }

#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    return 0;
}