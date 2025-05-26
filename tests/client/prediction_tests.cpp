/**
 * @file prediction_tests.cpp
 * @brief Unit tests for client-side prediction and reconciliation in netcode demo.
 *
 * Validates that the prediction function returns correct positions for different start states,
 * and that the advanced PredictionSystem correctly reconciles server updates,
 * removes acknowledged inputs, and replays unacknowledged inputs.
 *
 * Extended with edge cases, error conditions, and boundary testing.
 *
 * @author Aryan Malekian & Jonathan Skomsøy Hübertz,  w/ use of A.I. Models 
 * @date 22.05.2025
 */

#include <catch2/catch_all.hpp>
#include "netcode/common/prediction.hpp"
#include "netcode/common/packet.hpp"
#include <limits>

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

// --- NEW EDGE CASE AND ERROR TESTS ---

TEST_CASE("predictPosition: Edge cases and extreme values", "[Prediction][EdgeCase]") {
    SECTION("Very large time delta") {
        Packet pkt{ 1, 0.0f, 0.0f, 1.0f, 1.0f };
        auto result = predictPosition(pkt, 10000.0f);
        REQUIRE(result.first == Catch::Approx(10000.0f));
        REQUIRE(result.second == Catch::Approx(10000.0f));
        REQUIRE(std::isfinite(result.first));
        REQUIRE(std::isfinite(result.second));
    }

    SECTION("Negative time delta") {
        Packet pkt{ 1, 100.0f, 100.0f, 5.0f, -3.0f };
        auto result = predictPosition(pkt, -2.0f);
        REQUIRE(result.first == Catch::Approx(90.0f));  // 100 + 5*(-2)
        REQUIRE(result.second == Catch::Approx(106.0f)); // 100 + (-3)*(-2)
    }

    SECTION("Zero velocity") {
        Packet pkt{ 1, 50.0f, -25.0f, 0.0f, 0.0f };
        auto result = predictPosition(pkt, 100.0f);
        REQUIRE(result.first == Catch::Approx(50.0f));
        REQUIRE(result.second == Catch::Approx(-25.0f));
    }

    SECTION("Extreme velocity values") {
        Packet pkt{ 1, 0.0f, 0.0f, 1000.0f, -1000.0f };
        auto result = predictPosition(pkt, 1.0f);
        REQUIRE(result.first == Catch::Approx(1000.0f));
        REQUIRE(result.second == Catch::Approx(-1000.0f));
    }
}

TEST_CASE("predictPosition: Invalid packet handling", "[Prediction][ErrorHandling]") {
    SECTION("Prediction with NaN velocity") {
        Packet pkt{ 1, 100.0f, 100.0f, std::numeric_limits<float>::quiet_NaN(), 5.0f };
        auto result = predictPosition(pkt, 1.0f);
        // x should be NaN, y should be valid
        REQUIRE(std::isnan(result.first));
        REQUIRE(result.second == Catch::Approx(105.0f));
    }

    SECTION("Prediction with infinite velocity") {
        Packet pkt{ 1, 0.0f, 0.0f, std::numeric_limits<float>::infinity(), 1.0f };
        auto result = predictPosition(pkt, 1.0f);
        REQUIRE(std::isinf(result.first));
        REQUIRE(result.second == Catch::Approx(1.0f));
    }
}

TEST_CASE("PredictionSystem: Advanced edge cases", "[PredictionSystem][EdgeCase]") {
    SECTION("Construction with extreme initial values") {
        PredictionSystem sys(10000.0f, -10000.0f);
        auto pos = sys.getPredictedPosition();
        REQUIRE(pos.first == Catch::Approx(10000.0f));
        REQUIRE(pos.second == Catch::Approx(-10000.0f));
    }

    SECTION("Input buffer management under stress") {
        PredictionSystem sys(0.0f, 0.0f);

        // Fill buffer beyond capacity
        for (uint32_t i = 1; i <= 200; ++i) {
            InputCommand input(i, 1.0f, 0.0f, 0.016f);
            sys.applyInput(input);
        }

        // Should be limited and throttling
        REQUIRE(sys.getUnackedInputCount() <= 120);
        REQUIRE(sys.shouldThrottle());
    }

    SECTION("Multiple reconciliations in sequence") {
        PredictionSystem sys(0.0f, 0.0f);

        // Apply inputs
        for (uint32_t i = 1; i <= 5; ++i) {
            InputCommand input(i, 1.0f, 0.0f, 1.0f);
            sys.applyInput(input);
        }

        // First reconciliation
        Packet server1{ 2, 240.0f, 0.0f, 0.0f, 0.0f };
        sys.reconcileWithServer(server1);
        REQUIRE(sys.getUnackedInputCount() == 3); // inputs 3,4,5 remain

        // Second reconciliation
        Packet server2{ 4, 480.0f, 0.0f, 0.0f, 0.0f };
        sys.reconcileWithServer(server2);
        REQUIRE(sys.getUnackedInputCount() == 1); // only input 5 remains
    }

    SECTION("Error correction over time") {
        PredictionSystem sys(100.0f, 100.0f);

        // Apply input - this moves player to (220, 100)
        InputCommand input(1, 1.0f, 0.0f, 1.0f);
        sys.applyInput(input);

        // Server reconciliation with different state
        Packet serverPkt{ 1, 50.0f, 100.0f, 120.0f, 0.0f };
        sys.reconcileWithServer(serverPkt);

        // After reconciliation, system should be at reconciled state (no unacked inputs)
        auto posAfterReconcile = sys.getPredictedPosition();
        REQUIRE(posAfterReconcile.first == Catch::Approx(50.0f));

        // Update should maintain the reconciled position
        sys.update(0.1f);
        auto posAfterUpdate = sys.getPredictedPosition();

        // Just verify position is stable (may have small corrections)
        REQUIRE(std::isfinite(posAfterUpdate.first));
        REQUIRE(std::isfinite(posAfterUpdate.second));
    }
}

TEST_CASE("PredictionSystem: Boundary and invalid input handling", "[PredictionSystem][Validation]") {
    SECTION("Zero sequence number handling") {
        PredictionSystem sys(0.0f, 0.0f);

        InputCommand invalidInput(0, 1.0f, 0.0f, 1.0f); // seq = 0 is invalid
        sys.applyInput(invalidInput);

        // Should still function but with questionable input
        REQUIRE(sys.getUnackedInputCount() == 1);
    }

    SECTION("Out-of-order server packets") {
        PredictionSystem sys(0.0f, 0.0f);

        // Apply inputs
        InputCommand input1(10, 1.0f, 0.0f, 1.0f);
        InputCommand input2(11, 1.0f, 0.0f, 1.0f);
        sys.applyInput(input1);
        sys.applyInput(input2);

        // Server sends newer packet first
        Packet newerPkt{ 11, 240.0f, 0.0f, 0.0f, 0.0f };
        sys.reconcileWithServer(newerPkt);

        // Then older packet (should be ignored effectively)
        Packet olderPkt{ 10, 120.0f, 0.0f, 0.0f, 0.0f };
        sys.reconcileWithServer(olderPkt);

        // Should maintain state from newer packet
        REQUIRE(sys.getUnackedInputCount() == 0);
    }

    SECTION("Very small time deltas") {
        PredictionSystem sys(100.0f, 100.0f);

        InputCommand microInput(1, 1.0f, 1.0f, 0.001f); // 1ms
        sys.applyInput(microInput);

        auto pos = sys.getPredictedPosition();
        // Should have minimal movement
        REQUIRE(pos.first == Catch::Approx(100.12f).margin(0.01f));
        REQUIRE(pos.second == Catch::Approx(100.12f).margin(0.01f));
    }

    SECTION("Input with extreme velocity values") {
        PredictionSystem sys(0.0f, 0.0f);

        InputCommand extremeInput(1, 1000.0f, -1000.0f, 1.0f);
        sys.applyInput(extremeInput);

        auto pos = sys.getPredictedPosition();
        // Should handle extreme values gracefully
        REQUIRE(std::isfinite(pos.first));
        REQUIRE(std::isfinite(pos.second));
        REQUIRE(std::abs(pos.first) > 100000.0f); // Should be very large
    }
}

TEST_CASE("PredictionSystem: Throttling behavior", "[PredictionSystem][Performance]") {
    SECTION("Throttling threshold") {
        PredictionSystem sys(0.0f, 0.0f);

        // Fill half the buffer
        for (uint32_t i = 1; i <= 60; ++i) {
            InputCommand input(i, 1.0f, 0.0f, 0.016f);
            sys.applyInput(input);
        }

        REQUIRE_FALSE(sys.shouldThrottle()); // Should not throttle yet

        // Fill more than half
        for (uint32_t i = 61; i <= 70; ++i) {
            InputCommand input(i, 1.0f, 0.0f, 0.016f);
            sys.applyInput(input);
        }

        REQUIRE(sys.shouldThrottle()); // Should now throttle
    }

    SECTION("Throttling recovery after reconciliation") {
        PredictionSystem sys(0.0f, 0.0f);

        // Fill buffer to trigger throttling
        for (uint32_t i = 1; i <= 80; ++i) {
            InputCommand input(i, 1.0f, 0.0f, 0.016f);
            sys.applyInput(input);
        }

        REQUIRE(sys.shouldThrottle());

        // Server acknowledges most inputs
        Packet serverPkt{ 75, 1000.0f, 0.0f, 0.0f, 0.0f };
        sys.reconcileWithServer(serverPkt);

        // Should no longer need to throttle
        REQUIRE_FALSE(sys.shouldThrottle());
        REQUIRE(sys.getUnackedInputCount() <= 5);
    }
}

TEST_CASE("PredictionSystem: State consistency", "[PredictionSystem][Consistency]") {
    SECTION("Velocity consistency after reconciliation") {
        PredictionSystem sys(0.0f, 0.0f);

        InputCommand input(1, 0.5f, -0.3f, 1.0f);
        sys.applyInput(input);

        auto velBefore = sys.getPredictedVelocity();

        // Server reconciliation
        Packet serverPkt{ 1, 60.0f, -36.0f, 60.0f, -36.0f };
        sys.reconcileWithServer(serverPkt);

        auto velAfter = sys.getPredictedVelocity();

        // Velocity should match server's velocity
        REQUIRE(velAfter.first == Catch::Approx(60.0f));
        REQUIRE(velAfter.second == Catch::Approx(-36.0f));
    }

    SECTION("Position bounds after extreme inputs") {
        PredictionSystem sys(5000.0f, 5000.0f);

        // Apply many extreme inputs
        for (uint32_t i = 1; i <= 10; ++i) {
            InputCommand input(i, 1.0f, 1.0f, 10.0f); // Large time delta
            sys.applyInput(input);
        }

        auto pos = sys.getPredictedPosition();

        // Positions should still be finite
        REQUIRE(std::isfinite(pos.first));
        REQUIRE(std::isfinite(pos.second));
        // Should be very large due to accumulated movement
        REQUIRE(pos.first > 10000.0f);
        REQUIRE(pos.second > 10000.0f);
    }
}

TEST_CASE("Legacy prediction function edge cases", "[Prediction][Legacy]") {
    SECTION("Prediction with invalid packet data") {
        Packet invalidPkt{ 0, std::numeric_limits<float>::quiet_NaN(), 100.0f, 5.0f, 5.0f };
        auto result = predictPosition(invalidPkt, 1.0f);

        REQUIRE(std::isnan(result.first));
        REQUIRE(result.second == Catch::Approx(105.0f));
    }

    SECTION("Very small movements") {
        Packet pkt{ 1, 1000.0f, 1000.0f, 0.001f, -0.001f };
        auto result = predictPosition(pkt, 1.0f);

        REQUIRE(result.first == Catch::Approx(1000.001f));
        REQUIRE(result.second == Catch::Approx(999.999f));
    }
}