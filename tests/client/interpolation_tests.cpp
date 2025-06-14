/**
 * @file interpolation_tests.cpp
 * @brief Unit tests for linear interpolation functions in netcode system.
 *
 * Validates the mathematical correctness of interpolation algorithms used
 * for smooth visual transitions between discrete network state updates.
 * Tests boundary conditions, midpoint calculations, and directional movement.
 *
 * Coverage:
 * - Boundary value testing (t=0, t=1)
 * - Midpoint interpolation accuracy
 * - Negative direction movement handling
 * - Mathematical precision validation
 *
 * @author Aryan Malekian & Jonathan Skoms�y H�bertz,  w/ use of A.I. Models 
 * @date 22.05.2025
 */

#include <catch2/catch_all.hpp>
#include "netcode/common/interpolation.hpp"
#include "netcode/common/packet.hpp"

TEST_CASE("interpolatePosition: t=0 returns first packet position", "[Interpolation]") {
    Packet a{ 0, 10.f, 20.f, 0.f, 0.f };
    Packet b{ 1, 50.f, 80.f, 0.f, 0.f };
    auto pos = interpolatePosition(a, b, 0.f);
    REQUIRE(pos.first == Catch::Approx(10.f));
    REQUIRE(pos.second == Catch::Approx(20.f));
}

TEST_CASE("interpolatePosition: t=1 returns second packet position", "[Interpolation]") {
    Packet a{ 0, -3.f, 7.f, 0.f, 0.f };
    Packet b{ 1, 27.f, 77.f, 0.f, 0.f };
    auto pos = interpolatePosition(a, b, 1.f);
    REQUIRE(pos.first == Catch::Approx(27.f));
    REQUIRE(pos.second == Catch::Approx(77.f));
}

TEST_CASE("interpolatePosition: t=0.5 returns midpoint", "[Interpolation]") {
    Packet a{ 0, 0.f, 0.f, 0.f, 0.f };
    Packet b{ 1, 8.f, 4.f, 0.f, 0.f };
    auto pos = interpolatePosition(a, b, 0.5f);
    REQUIRE(pos.first == Catch::Approx(4.f));
    REQUIRE(pos.second == Catch::Approx(2.f));
}

TEST_CASE("interpolatePosition: Negative direction interpolation", "[Interpolation]") {
    Packet a{ 0, 100.f, 100.f, 0.f, 0.f };
    Packet b{ 1, 80.f, 60.f, 0.f, 0.f };
    auto pos = interpolatePosition(a, b, 0.25f);
    REQUIRE(pos.first == Catch::Approx(95.f));
    REQUIRE(pos.second == Catch::Approx(90.f));
}