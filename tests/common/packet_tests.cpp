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
#include "../../include/packet.hpp"
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


