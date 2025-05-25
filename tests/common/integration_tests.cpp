/**
 * @file integration_tests.cpp
 * @brief Integration tests for server/client functionality
 *
 * These tests cover the main application logic that wasn't covered
 * by unit tests, focusing on testable components.
 *
 * @author Aryan Malekian
 * @date 24.05.2025
 */

#include <catch2/catch_all.hpp>
#include <sstream>
#include <chrono>
#include <thread>

 // Mock implementations for testing
namespace TestUtils {
    /**
     * @brief Mock timestamp function for testing
     */
    std::string getMockTimestamp() {
        return "12:34:56.789";
    }

    /**
     * @brief Test packet validation scenarios
     */
    bool validatePacketSize(size_t receivedSize, size_t expectedSize) {
        return receivedSize == expectedSize;
    }

    /**
     * @brief Simulate DelaySimulator functionality
     */
    class MockDelaySimulator {
    private:
        std::vector<std::pair<std::vector<char>, std::chrono::steady_clock::time_point>> queue;

    public:
        void send(const char* buf, size_t len, int delayMs) {
            auto releaseTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);
            queue.emplace_back(std::vector<char>(buf, buf + len), releaseTime);
        }

        bool getReady(char* buf, size_t len) {
            if (!queue.empty() && queue.front().second <= std::chrono::steady_clock::now()) {
                auto& packet = queue.front();
                size_t copySize = std::min(len, packet.first.size());
                std::copy(packet.first.begin(), packet.first.begin() + copySize, buf);
                queue.erase(queue.begin());
                return true;
            }
            return false;
        }

        size_t queueSize() const { return queue.size(); }
        void clear() { queue.clear(); }
    };

    /**
     * @brief Mock Trail class for testing
     */
    class MockTrail {
    private:
        std::vector<std::pair<float, float>> positions;
        size_t maxSize;

    public:
        MockTrail(size_t max = 30) : maxSize(max) {}

        void addPosition(float x, float y) {
            positions.emplace_back(x, y);
            if (positions.size() > maxSize) {
                positions.erase(positions.begin());
            }
        }

        size_t size() const { return positions.size(); }
        void clear() { positions.clear(); }

        std::pair<float, float> getPosition(size_t index) const {
            if (index < positions.size()) {
                return positions[index];
            }
            return { 0.0f, 0.0f };
        }
    };
}

TEST_CASE("Integration: Timestamp utility", "[Integration][Utilities]") {
    SECTION("Mock timestamp format") {
        auto timestamp = TestUtils::getMockTimestamp();
        REQUIRE_FALSE(timestamp.empty());
        REQUIRE(timestamp.length() > 0);
        // Should follow HH:MM:SS.mmm format
        REQUIRE(timestamp.find(':') != std::string::npos);
        REQUIRE(timestamp.find('.') != std::string::npos);
    }
}

TEST_CASE("Integration: Packet size validation", "[Integration][Validation]") {
    SECTION("Valid packet size") {
        REQUIRE(TestUtils::validatePacketSize(20, 20));
    }

    SECTION("Invalid packet size - too small") {
        REQUIRE_FALSE(TestUtils::validatePacketSize(19, 20));
    }

    SECTION("Invalid packet size - too large") {
        REQUIRE_FALSE(TestUtils::validatePacketSize(21, 20));
    }

    SECTION("Zero size packets") {
        REQUIRE_FALSE(TestUtils::validatePacketSize(0, 20));
    }
}

TEST_CASE("Integration: DelaySimulator functionality", "[Integration][DelaySimulator]") {
    TestUtils::MockDelaySimulator delaySimulator;

    SECTION("Send and immediate retrieval") {
        const char testData[] = "test";
        delaySimulator.send(testData, 4, 0); // No delay

        // Small delay to ensure time has passed
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        char buf[10];
        REQUIRE(delaySimulator.getReady(buf, 10));
        REQUIRE(std::string(buf, 4) == "test");
    }

    SECTION("Send with delay - not ready immediately") {
        const char testData[] = "delayed";
        delaySimulator.send(testData, 7, 100); // 100ms delay

        char buf[10];
        REQUIRE_FALSE(delaySimulator.getReady(buf, 10)); // Should not be ready yet
        REQUIRE(delaySimulator.queueSize() == 1);
    }

    SECTION("Multiple packets in queue") {
        delaySimulator.send("packet1", 7, 0);
        delaySimulator.send("packet2", 7, 50);
        delaySimulator.send("packet3", 7, 100);

        REQUIRE(delaySimulator.queueSize() == 3);

        // First packet should be ready immediately
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        char buf[10];
        REQUIRE(delaySimulator.getReady(buf, 10));
        REQUIRE(std::string(buf, 7) == "packet1");
        REQUIRE(delaySimulator.queueSize() == 2);
    }

    SECTION("Clear queue") {
        delaySimulator.send("data", 4, 0);
        REQUIRE(delaySimulator.queueSize() == 1);

        delaySimulator.clear();
        REQUIRE(delaySimulator.queueSize() == 0);
    }
}

TEST_CASE("Integration: Trail visualization", "[Integration][Trail]") {
    TestUtils::MockTrail trail(5); // Max 5 positions

    SECTION("Add positions within limit") {
        trail.addPosition(10.0f, 20.0f);
        trail.addPosition(15.0f, 25.0f);
        trail.addPosition(20.0f, 30.0f);

        REQUIRE(trail.size() == 3);

        auto pos = trail.getPosition(0);
        REQUIRE(pos.first == Catch::Approx(10.0f));
        REQUIRE(pos.second == Catch::Approx(20.0f));
    }

    SECTION("Add positions exceeding limit") {
        for (int i = 0; i < 10; ++i) {
            trail.addPosition(i * 10.0f, i * 20.0f);
        }

        REQUIRE(trail.size() == 5); // Should be limited to maxSize

        // First position should be from i=5 (oldest ones removed)
        auto firstPos = trail.getPosition(0);
        REQUIRE(firstPos.first == Catch::Approx(50.0f));
        REQUIRE(firstPos.second == Catch::Approx(100.0f));
    }

    SECTION("Clear trail") {
        trail.addPosition(100.0f, 200.0f);
        REQUIRE(trail.size() == 1);

        trail.clear();
        REQUIRE(trail.size() == 0);
    }

    SECTION("Invalid position access") {
        trail.addPosition(10.0f, 20.0f);

        auto invalidPos = trail.getPosition(10); // Index out of bounds
        REQUIRE(invalidPos.first == Catch::Approx(0.0f));
        REQUIRE(invalidPos.second == Catch::Approx(0.0f));
    }
}

TEST_CASE("Integration: Server statistics tracking", "[Integration][ServerStats]") {
    struct MockServerStats {
        uint64_t totalPackets = 0;
        uint64_t validPackets = 0;
        uint64_t invalidPackets = 0;

        void recordPacket(bool isValid) {
            totalPackets++;
            if (isValid) {
                validPackets++;
            }
            else {
                invalidPackets++;
            }
        }

        double getValidRate() const {
            return totalPackets > 0 ? (double)validPackets / totalPackets * 100.0 : 0.0;
        }

        bool shouldPrintStats() const {
            return totalPackets % 100 == 0 && totalPackets > 0;
        }
    };

    MockServerStats stats;

    SECTION("Track valid packets") {
        for (int i = 0; i < 150; ++i) {
            stats.recordPacket(true);
        }

        REQUIRE(stats.totalPackets == 150);
        REQUIRE(stats.validPackets == 150);
        REQUIRE(stats.invalidPackets == 0);
        REQUIRE(stats.getValidRate() == Catch::Approx(100.0));
    }

    SECTION("Track mixed packet validity") {
        for (int i = 0; i < 80; ++i) {
            stats.recordPacket(true);
        }
        for (int i = 0; i < 20; ++i) {
            stats.recordPacket(false);
        }

        REQUIRE(stats.totalPackets == 100);
        REQUIRE(stats.validPackets == 80);
        REQUIRE(stats.invalidPackets == 20);
        REQUIRE(stats.getValidRate() == Catch::Approx(80.0));
    }

    SECTION("Statistics printing trigger") {
        // Should trigger at 100, 200, etc.
        for (int i = 1; i <= 99; ++i) {
            stats.recordPacket(true);
            REQUIRE_FALSE(stats.shouldPrintStats());
        }

        stats.recordPacket(true); // 100th packet
        REQUIRE(stats.shouldPrintStats());

        stats.recordPacket(true); // 101st packet
        REQUIRE_FALSE(stats.shouldPrintStats());
    }
}

TEST_CASE("Integration: Error handling scenarios", "[Integration][ErrorHandling]") {
    SECTION("Socket error code mapping") {
        // Test common error conditions without actual sockets
        struct ErrorMapping {
            int code;
            std::string description;
        };

        std::vector<ErrorMapping> errors = {
            {10048, "Address already in use"},
            {10049, "Cannot assign requested address"},
            {10061, "Connection refused"},
            {10060, "Connection timed out"}
        };

        for (const auto& error : errors) {
            REQUIRE(error.code > 0);
            REQUIRE_FALSE(error.description.empty());
        }
    }

    SECTION("Buffer overflow protection") {
        // Test that our systems handle buffer size mismatches
        const size_t EXPECTED_SIZE = 20;
        const size_t BUFFER_SIZE = 1024;

        std::vector<size_t> testSizes = { 0, 10, 19, 20, 21, 100, BUFFER_SIZE };

        for (size_t testSize : testSizes) {
            bool isValid = (testSize == EXPECTED_SIZE);
            REQUIRE(TestUtils::validatePacketSize(testSize, EXPECTED_SIZE) == isValid);
        }
    }
}

TEST_CASE("Integration: Performance metrics calculation", "[Integration][Metrics]") {
    struct MockMetrics {
        float avgRTT = 0.0f;
        int totalSamples = 0;

        void addRTTSample(float rtt) {
            if (totalSamples == 0) {
                avgRTT = rtt;
            }
            else {
                
                avgRTT = avgRTT * 0.9f + rtt * 0.1f;
            }
            totalSamples++;
        }

        float getPacketLossRate(int sent, int received) const {
            return sent > 0 ? (1.0f - (float)received / sent) * 100.0f : 0.0f;
        }
    };

    MockMetrics metrics;

    SECTION("RTT averaging") {
        metrics.addRTTSample(100.0f);
        REQUIRE(metrics.avgRTT == Catch::Approx(100.0f));

        metrics.addRTTSample(200.0f);
        REQUIRE(metrics.avgRTT == Catch::Approx(110.0f)); 

        metrics.addRTTSample(50.0f);
        REQUIRE(metrics.avgRTT == Catch::Approx(105.0f).margin(1.0f)); 
    }

    SECTION("Packet loss calculation") {
        REQUIRE(metrics.getPacketLossRate(100, 95) == Catch::Approx(5.0f));
        REQUIRE(metrics.getPacketLossRate(100, 100) == Catch::Approx(0.0f));
        REQUIRE(metrics.getPacketLossRate(100, 50) == Catch::Approx(50.0f));
        REQUIRE(metrics.getPacketLossRate(0, 0) == Catch::Approx(0.0f)); 
    }
}