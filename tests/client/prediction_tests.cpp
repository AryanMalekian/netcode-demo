/**
 * @file prediction_tests.cpp
 * @brief Unit tests for client-side prediction (predictPosition) in netcode demo.
 *
 * Validates that the prediction function returns correct positions for different start states,
 * with zero and positive/negative elapsed time.
 *
 * @author Aryan Malekian
 * @date 22.05.2025
 */

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "netcode/common/prediction.hpp"
#include "netcode/common/packet.hpp"

TEST_CASE("predictPosition: Zero delta time returns same position", "[Prediction]") {
    Packet pkt{ 12, 100.0f, 50.0f, 4.0f, -3.0f };
    auto result = predictPosition(pkt, 0.0f);
    REQUIRE(result.first == Catch::Approx(100.0f));
    REQUIRE(result.second == Catch::Approx(50.0f));
}

TEST_CASE("predictPosition: Simple linear prediction", "[Prediction]") {
    Packet pkt{ 99, 0.0f, 0.0f, 10.0f, 5.0f };
    auto result = predictPosition(pkt, 2.0f);
    REQUIRE(result.first == Catch::Approx(20.0f));
    REQUIRE(result.second == Catch::Approx(10.0f));
}

TEST_CASE("predictPosition: Negative velocity and positive time", "[Prediction]") {
    Packet pkt{ 5, -10.0f, 30.0f, -2.5f, -7.0f };
    auto result = predictPosition(pkt, 4.0f);
    REQUIRE(result.first == Catch::Approx(-20.0f));
    REQUIRE(result.second == Catch::Approx(2.0f));
}
