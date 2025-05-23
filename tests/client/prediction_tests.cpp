/**
 * @file prediction_tests.cpp
 * @brief Unit tests for client-side prediction and reconciliation in netcode demo.
 *
 * Validates that the prediction function returns correct positions for different start states,
 * and that the advanced PredictionSystem correctly reconciles server updates,
 * removes acknowledged inputs, and replays unacknowledged inputs.
 *
 * @author Aryan Malekian
 * @date 22.05.2025
 */

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

TEST_CASE("PredictionSystem: reconcile removes acknowledged inputs and resets predicted state", "[PredictionSystem]") {
    // Start at origin
    PredictionSystem sys(0.0f, 0.0f);

    // Apply a single input (move right for dt=1s)
    InputCommand in{ 1, 1.0f, 0.0f, 1.0f };
    sys.applyInput(in);
    REQUIRE(sys.getUnackedInputCount() == 1);

    // Before reconciliation, predicted X = 120 (MOVE_SPEED * 1 * dt)
    auto before = sys.getPredictedPosition();
    REQUIRE(before.first == Catch::Approx(120.0f));
    REQUIRE(before.second == Catch::Approx(0.0f));

    // Simulate server ack of seq=1 at position (0,0)
    Packet serverPkt{ 1, 0.0f, 0.0f, 0.0f, 0.0f };
    sys.reconcileWithServer(serverPkt);

    // After reconciliation, predicted resets to server state
    auto after = sys.getPredictedPosition();
    REQUIRE(after.first == Catch::Approx(0.0f));
    REQUIRE(after.second == Catch::Approx(0.0f));
    REQUIRE(sys.getUnackedInputCount() == 0);
}

TEST_CASE("PredictionSystem: reconcile replays unacknowledged inputs", "[PredictionSystem]") {
    PredictionSystem sys(0.0f, 0.0f);

    // Apply two inputs: both move right dt=1s
    InputCommand in1{ 1, 1.0f, 0.0f, 1.0f };
    InputCommand in2{ 2, 1.0f, 0.0f, 1.0f };
    sys.applyInput(in1);
    sys.applyInput(in2);
    REQUIRE(sys.getUnackedInputCount() == 2);

    // Predicted after two inputs: 240 units to the right
    auto pre = sys.getPredictedPosition();
    REQUIRE(pre.first == Catch::Approx(240.0f));
    REQUIRE(pre.second == Catch::Approx(0.0f));

    // Server acknowledges only seq=1 at position 120
    Packet serverPkt{ 1, 120.0f, 0.0f, 0.0f, 0.0f };
    sys.reconcileWithServer(serverPkt);

    // One input (seq=1) removed, second input replayed => final X = 120 + 120 = 240
    auto post = sys.getPredictedPosition();
    REQUIRE(post.first == Catch::Approx(240.0f));
    REQUIRE(post.second == Catch::Approx(0.0f));
    REQUIRE(sys.getUnackedInputCount() == 1);
}
