/**
 * @file packet_tests.cpp
 * @brief Comprehensive unit tests for the Packet struct (serialization/deserialization).
 *
 * This file uses Catch2 (May 2025 version) to validate the correctness of the Packet struct's
 * serialization and deserialization routines, including:
 *   - Round-trip accuracy for all fields
 *   - Correct handling of network byte order for seq
 *   - Zero buffer behavior
 *   - Static size checks
 *
 * @author Aryan Malekian
 * @date 22.05.2025
 */

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "netcode/common/packet.hpp"
#include <cstring>  // For std::memcmp

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
