/**
 * @file client_tests.cpp
 * @brief Unit tests for client-side logic and basic game mechanics.
 *
 * Tests fundamental client functions including position updates, velocity calculations,
 * and packet serialization workflows. Focuses on core mathematical operations and
 * data handling rather than complex networking or rendering components.
 *
 * Coverage:
 * - Basic physics simulation (position/velocity updates)
 * - Packet serialization roundtrip accuracy
 * - Mathematical correctness of movement calculations
 *
 * @author Aryan Malekian w/ use of A.I. Models
 * @date 23.05.2025
 */

#include <catch2/catch_all.hpp>
#include "netcode/common/packet.hpp"
#include "netcode/common/prediction.hpp"

 /**
  * @brief Helper function to update position based on velocity and time delta.
  *
  * Implements basic kinematic equation: position += velocity * time
  * Used for testing fundamental physics calculations.
  *
  * @param[in,out] x X position to update
  * @param[in,out] y Y position to update
  * @param vx X velocity (units per second)
  * @param vy Y velocity (units per second)
  * @param dt Time delta (seconds)
  */
static void update_position(float& x, float& y, float vx, float vy, float dt) {
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

    // Simulate another tick with zero velocity
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