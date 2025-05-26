/**
 * @file prediction.cpp
 * @brief Implementation of advanced client-side prediction for the netcode demo.
 *
 * Implements both the legacy predictPosition() function (simple linear extrapolation)
 * and the PredictionSystem class for robust, modern client-side prediction with
 * input buffering, server reconciliation, and smooth error correction.
 *
 * See prediction.hpp for API and documentation.
 *
 * @see prediction.hpp
 * @author Aryan Malekian & Jonathan Skomsøy Hübertz,  w/ use of A.I. Models 
 * @date 23.05.2025
 */

#include "netcode/common/prediction.hpp"
#include <algorithm>
#include <cmath>

 /**
  * @brief Predicts object position via simple linear extrapolation.
  * @see prediction.hpp
  */
std::pair<float, float> predictPosition(const Packet& pkt, float dt) {
    return {
        pkt.x + pkt.vx * dt,
        pkt.y + pkt.vy * dt
    };
}

// --- PredictionSystem implementation ---

/**
 * @brief Constructs a PredictionSystem and initializes all state.
 */
PredictionSystem::PredictionSystem(float initialX, float initialY)
    : predictedX(initialX), predictedY(initialY)
    , predictedVx(0.0f), predictedVy(0.0f)
    , serverX(initialX), serverY(initialY)
    , serverVx(0.0f), serverVy(0.0f)
    , lastAckedSequence(0)
    , errorX(0.0f), errorY(0.0f) {
}

/**
 * @brief Applies a local input immediately, updates predicted state, and buffers input for reconciliation.
 * @param input The InputCommand for this frame
 */
void PredictionSystem::applyInput(const InputCommand& input) {
    // Apply input to predicted state
    applyInputToState(predictedX, predictedY, predictedVx, predictedVy, input);

    // Buffer input for reconciliation with future server packets
    unacknowledgedInputs.push_back(input);

    // Prevent buffer from growing too large
    if (unacknowledgedInputs.size() > MAX_UNACKED_INPUTS) {
        unacknowledgedInputs.pop_front();
    }
}

/**
 * @brief Reconciles predicted state with the authoritative server state,
 * rolling back, replaying unacknowledged inputs, and preparing for error correction.
 * @param serverPacket The most recent Packet from the server
 */
void PredictionSystem::reconcileWithServer(const Packet& serverPacket) {
    // Update sequence and authoritative state from server
    lastAckedSequence = serverPacket.seq;
    serverX = serverPacket.x;
    serverY = serverPacket.y;
    serverVx = serverPacket.vx;
    serverVy = serverPacket.vy;

    // Remove acknowledged inputs (those older or equal to server sequence)
    while (!unacknowledgedInputs.empty() &&
        unacknowledgedInputs.front().sequence <= lastAckedSequence) {
        unacknowledgedInputs.pop_front();
    }

    // Start from server state, replay all remaining inputs in buffer
    float reconciledX = serverX;
    float reconciledY = serverY;
    float reconciledVx = serverVx;
    float reconciledVy = serverVy;

    for (const auto& input : unacknowledgedInputs) {
        applyInputToState(reconciledX, reconciledY, reconciledVx, reconciledVy, input);
    }

    // Calculate prediction error (difference between predicted and true state)
    errorX = reconciledX - predictedX;
    errorY = reconciledY - predictedY;

    // Snap prediction to reconciled state (will be corrected smoothly over time)
    predictedX = reconciledX;
    predictedY = reconciledY;
    predictedVx = reconciledVx;
    predictedVy = reconciledVy;
}

/**
 * @brief Updates the predicted state and applies smooth error correction for visual continuity.
 * @param dt Time since last update (seconds)
 */
void PredictionSystem::update(float dt) {
    // Apply smooth error correction over several frames
    if (std::abs(errorX) > 0.01f || std::abs(errorY) > 0.01f) {
        float correctionX = errorX * ERROR_CORRECTION_RATE * dt;
        float correctionY = errorY * ERROR_CORRECTION_RATE * dt;

        // Prevent over-correction
        if (std::abs(correctionX) > std::abs(errorX)) {
            correctionX = errorX;
        }
        if (std::abs(correctionY) > std::abs(errorY)) {
            correctionY = errorY;
        }

        predictedX += correctionX;
        predictedY += correctionY;

        errorX -= correctionX;
        errorY -= correctionY;
    }
}

/**
 * @brief Returns the current predicted position.
 */
std::pair<float, float> PredictionSystem::getPredictedPosition() const {
    return { predictedX, predictedY };
}

/**
 * @brief Returns the current predicted velocity.
 */
std::pair<float, float> PredictionSystem::getPredictedVelocity() const {
    return { predictedVx, predictedVy };
}

/**
 * @brief Applies a single input command to a given position and velocity.
 * @param[in,out] x  Position X (modified in-place)
 * @param[in,out] y  Position Y (modified in-place)
 * @param[in,out] vx Velocity X (modified in-place)
 * @param[in,out] vy Velocity Y (modified in-place)
 * @param input      Input command to apply
 */
void PredictionSystem::applyInputToState(float& x, float& y, float& vx, float& vy,
    const InputCommand& input) {
    // Calculate velocity based on input
    const float MOVE_SPEED = 120.0f;  // Units per second (tunable)
    vx = input.vx * MOVE_SPEED;
    vy = input.vy * MOVE_SPEED;

    // Advance position by velocity and input duration
    x += vx * input.dt;
    y += vy * input.dt;
}
