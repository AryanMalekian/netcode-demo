/**
 * @file client_tests.cpp
 * @brief Basic tests for UDP client logic in netcode demo.
 *
 * Tests core client functions: packet creation and simulated input velocity updates.
 * Does not test rendering or real socket communication (mock needed for that).
 *
 * @author Aryan Malekian w/ use of A.I. Models
 * @date 23.05.2025
 */

#include <catch2/catch_all.hpp>
#include "netcode/common/packet.hpp"
#include "netcode/common/prediction.hpp"

static void update_position(float& x, float& y, float& vx, float& vy, float dt) {
    x += vx * dt;
    y += vy * dt;
}

TEST_CASE("Client: position updates correctly with velocity", "[client]") {
    float x = 0.f, y = 0.f;
    float vx = 10.f, vy = -5.f;
    float dt = 1.f;

    update_position(x, y, vx, vy, dt);
    REQUIRE(x == Catch::Approx(10.f));
    REQUIRE(y == Catch::Approx(-5.f));

    // simulate another tick with zero velocity
    vx = 0.f; vy = 0.f;
    update_position(x, y, vx, vy, dt);
    REQUIRE(x == Catch::Approx(10.f));
    REQUIRE(y == Catch::Approx(-5.f));
}

TEST_CASE("Client: packet serialization roundtrip after simulated update", "[client]") {
    float x = 1.f, y = 2.f;
    float vx = 3.f, vy = 4.f;
    uint32_t seq = 5;

    Packet p{ seq, x, y, vx, vy };
    char buf[Packet::size()];
    p.serialize(buf);

    Packet deserialized;
    deserialized.deserialize(buf);

    REQUIRE(deserialized.seq == seq);
    REQUIRE(deserialized.x == Catch::Approx(x));
    REQUIRE(deserialized.y == Catch::Approx(y));
    REQUIRE(deserialized.vx == Catch::Approx(vx));
    REQUIRE(deserialized.vy == Catch::Approx(vy));
}
