/**
 * @file packet_tests.cpp
 * @brief Comprehensive unit tests for the Packet struct (serialization/deserialization and validation).
 *
 * This file uses Catch2 (May 2025 version) to validate the correctness of the Packet struct's
 * serialization and deserialization routines, including:
 *   - Round-trip accuracy for all fields
 *   - Correct handling of network byte order for seq
 *   - Zero buffer behavior
 *   - Static size checks
 *   - Packet validation for security and robustness
 *   - Negative test cases for invalid data
 *
 * @author Aryan Malekian
 * @date 22.05.2025
 */

#include <catch2/catch_all.hpp>
#include "netcode/common/packet.hpp"
#include <cstring>  // For std::memcmp
#include <cmath>    // For NaN and infinity
#include <limits>   // For numeric limits

TEST_CASE("Packet serialization/deserialization - roundtrip", "[Packet]") {
    Packet original;
    original.seq = 42;
    original.x = 123.45f;
    original.y = -54.321f;
    original.vx = 3.5f;
    original.vy = -2.0f;

    char buf[Packet::size()] = {};
    original.serialize(buf);

    Packet result;
    result.deserialize(buf);

    REQUIRE(result.seq == original.seq);
    REQUIRE(result.x == Catch::Approx(original.x));
    REQUIRE(result.y == Catch::Approx(original.y));
    REQUIRE(result.vx == Catch::Approx(original.vx));
    REQUIRE(result.vy == Catch::Approx(original.vy));
}

TEST_CASE("Packet serialization uses network byte order for seq", "[Packet]") {
    Packet pkt;
    pkt.seq = 0x01020304;
    pkt.x = pkt.y = pkt.vx = pkt.vy = 0.0f;

    char buf[Packet::size()] = {};
    pkt.serialize(buf);

    REQUIRE(static_cast<unsigned char>(buf[0]) == 0x01);
    REQUIRE(static_cast<unsigned char>(buf[1]) == 0x02);
    REQUIRE(static_cast<unsigned char>(buf[2]) == 0x03);
    REQUIRE(static_cast<unsigned char>(buf[3]) == 0x04);

    Packet deserialized;
    deserialized.deserialize(buf);
    REQUIRE(deserialized.seq == 0x01020304);
}

TEST_CASE("Packet deserialization from zero buffer yields zeroes", "[Packet]") {
    char buf[Packet::size()] = {};
    Packet pkt;
    pkt.deserialize(buf);

    REQUIRE(pkt.seq == 0);
    REQUIRE(pkt.x == Catch::Approx(0.0f));
    REQUIRE(pkt.y == Catch::Approx(0.0f));
    REQUIRE(pkt.vx == Catch::Approx(0.0f));
    REQUIRE(pkt.vy == Catch::Approx(0.0f));
}

TEST_CASE("Packet::size() is correct", "[Packet]") {
    REQUIRE(Packet::size() == sizeof(uint32_t) + 4 * sizeof(float));
}

// --- NEW VALIDATION TESTS ---

TEST_CASE("Packet validation - valid packets", "[Packet][Validation]") {
    SECTION("Normal valid packet") {
        Packet pkt(123, 100.0f, 200.0f, 5.0f, -3.0f);
        REQUIRE(pkt.isValid());
    }

    SECTION("Edge case - maximum valid values") {
        Packet pkt(UINT32_MAX, 9999.0f, -9999.0f, 999.0f, -999.0f);
        REQUIRE(pkt.isValid());
    }

    SECTION("Edge case - minimum sequence number") {
        Packet pkt(1, 0.0f, 0.0f, 0.0f, 0.0f);
        REQUIRE(pkt.isValid());
    }
}

TEST_CASE("Packet validation - invalid sequence numbers", "[Packet][Validation]") {
    Packet pkt(0, 100.0f, 100.0f, 1.0f, 1.0f);  // seq = 0 is invalid
    REQUIRE_FALSE(pkt.isValid());
}

TEST_CASE("Packet validation - invalid position values", "[Packet][Validation]") {
    SECTION("NaN position x") {
        Packet pkt(123, std::numeric_limits<float>::quiet_NaN(), 100.0f, 1.0f, 1.0f);
        REQUIRE_FALSE(pkt.isValid());
    }

    SECTION("NaN position y") {
        Packet pkt(123, 100.0f, std::numeric_limits<float>::quiet_NaN(), 1.0f, 1.0f);
        REQUIRE_FALSE(pkt.isValid());
    }

    SECTION("Infinite position x") {
        Packet pkt(123, std::numeric_limits<float>::infinity(), 100.0f, 1.0f, 1.0f);
        REQUIRE_FALSE(pkt.isValid());
    }

    SECTION("Infinite position y") {
        Packet pkt(123, 100.0f, -std::numeric_limits<float>::infinity(), 1.0f, 1.0f);
        REQUIRE_FALSE(pkt.isValid());
    }

    SECTION("Extreme position x - too large") {
        Packet pkt(123, 50000.0f, 100.0f, 1.0f, 1.0f);
        REQUIRE_FALSE(pkt.isValid());
    }

    SECTION("Extreme position y - too small") {
        Packet pkt(123, 100.0f, -50000.0f, 1.0f, 1.0f);
        REQUIRE_FALSE(pkt.isValid());
    }
}

TEST_CASE("Packet validation - invalid velocity values", "[Packet][Validation]") {
    SECTION("NaN velocity x") {
        Packet pkt(123, 100.0f, 100.0f, std::numeric_limits<float>::quiet_NaN(), 1.0f);
        REQUIRE_FALSE(pkt.isValid());
    }

    SECTION("NaN velocity y") {
        Packet pkt(123, 100.0f, 100.0f, 1.0f, std::numeric_limits<float>::quiet_NaN());
        REQUIRE_FALSE(pkt.isValid());
    }

    SECTION("Infinite velocity x") {
        Packet pkt(123, 100.0f, 100.0f, std::numeric_limits<float>::infinity(), 1.0f);
        REQUIRE_FALSE(pkt.isValid());
    }

    SECTION("Infinite velocity y") {
        Packet pkt(123, 100.0f, 100.0f, 1.0f, -std::numeric_limits<float>::infinity());
        REQUIRE_FALSE(pkt.isValid());
    }

    SECTION("Extreme velocity x - too large") {
        Packet pkt(123, 100.0f, 100.0f, 5000.0f, 1.0f);
        REQUIRE_FALSE(pkt.isValid());
    }

    SECTION("Extreme velocity y - too small") {
        Packet pkt(123, 100.0f, 100.0f, 1.0f, -5000.0f);
        REQUIRE_FALSE(pkt.isValid());
    }
}

TEST_CASE("Packet validation - boundary conditions", "[Packet][Validation]") {
    SECTION("Exactly at position boundary") {
        Packet pkt(123, 10000.0f, -10000.0f, 1.0f, 1.0f);
        REQUIRE(pkt.isValid());
    }

    SECTION("Just over position boundary") {
        Packet pkt(123, 10000.1f, 100.0f, 1.0f, 1.0f);
        REQUIRE_FALSE(pkt.isValid());
    }

    SECTION("Exactly at velocity boundary") {
        Packet pkt(123, 100.0f, 100.0f, 1000.0f, -1000.0f);
        REQUIRE(pkt.isValid());
    }

    SECTION("Just over velocity boundary") {
        Packet pkt(123, 100.0f, 100.0f, 1000.1f, 1.0f);
        REQUIRE_FALSE(pkt.isValid());
    }
}

TEST_CASE("Packet constructors", "[Packet][Constructor]") {
    SECTION("Default constructor creates valid packet") {
        Packet pkt;
        // Default constructor creates seq=0 which is invalid
        REQUIRE_FALSE(pkt.isValid());
        REQUIRE(pkt.seq == 0);
        REQUIRE(pkt.x == 0.0f);
        REQUIRE(pkt.y == 0.0f);
        REQUIRE(pkt.vx == 0.0f);
        REQUIRE(pkt.vy == 0.0f);
    }

    SECTION("Parameterized constructor") {
        Packet pkt(42, 10.0f, 20.0f, 3.0f, 4.0f);
        REQUIRE(pkt.isValid());
        REQUIRE(pkt.seq == 42);
        REQUIRE(pkt.x == Catch::Approx(10.0f));
        REQUIRE(pkt.y == Catch::Approx(20.0f));
        REQUIRE(pkt.vx == Catch::Approx(3.0f));
        REQUIRE(pkt.vy == Catch::Approx(4.0f));
    }
}

TEST_CASE("Packet sequence number wraparound", "[Packet][EdgeCase]") {
    // Test maximum sequence number (wraparound condition)
    Packet pkt(UINT32_MAX, 100.0f, 100.0f, 1.0f, 1.0f);
    REQUIRE(pkt.isValid());

    char buf[Packet::size()];
    pkt.serialize(buf);

    Packet deserialized;
    deserialized.deserialize(buf);

    REQUIRE(deserialized.seq == UINT32_MAX);
    REQUIRE(deserialized.isValid());
}