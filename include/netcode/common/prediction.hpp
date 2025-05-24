/**
 * @file prediction.hpp
 * @brief Advanced client-side prediction interface for the netcode demo.
 *
 * Declares both the legacy prediction function (simple linear extrapolation)
 * and a full-featured PredictionSystem class supporting modern netcode principles.
 *
 * Features:
 * - Simple prediction via linear extrapolation (legacy, kept for comparison/testing)
 * - Advanced prediction system using input buffering, server reconciliation,
 *   and smooth error correction to minimize perceived latency and improve
 *   gameplay experience over networks with variable delay and packet loss.
 *
 * This separation keeps prediction logic reusable, testable, and easy to extend.
 *
 * @author Aryan Malekian w/ use of A.I. Models
 * @date 23.05.2025
 */

#pragma once

#include "packet.hpp"
#include "input.hpp"
#include <utility>
#include <deque>
#include <optional>

 /**
  * @brief Predicts where an object will be after a time delta using simple linear extrapolation.
  *
  * This is the legacy prediction method used for compatibility and comparison with more advanced techniques.
  * It simply projects the position based on the last known velocity and elapsed time.
  *
  *   predictedX = pkt.x + pkt.vx * dt
  *   predictedY = pkt.y + pkt.vy * dt
  *
  * @param pkt Last received Packet containing position (x, y) and velocity (vx, vy).
  * @param dt  Time elapsed since pkt was received (in seconds).
  * @return    A std::pair<float, float> = (predictedX, predictedY).
  */
std::pair<float, float> predictPosition(const Packet& pkt, float dt);

/**
 * @class PredictionSystem
 * @brief Full-featured advanced client-side prediction system for real-time netcode.
 *
 * Maintains a buffer of unacknowledged input commands, applies them instantly for responsive local movement,
 * and performs server reconciliation: upon receiving an authoritative server state, rolls back, reapplies buffered inputs,
 * and corrects errors smoothly. Designed for real-time games and simulations needing robust, modern prediction.
 *
 * Features:
 * - Input prediction: Applies local input immediately for lag-free feeling
 * - Input buffering: Remembers inputs until confirmed by the server
 * - Server reconciliation: Corrects any mispredictions upon new server packets
 * - Smooth error correction: Eliminates visible snapping with gradual adjustment
 * - Built-in safeguards: Input buffer limits, throttle flag to avoid runaway input
 */
class PredictionSystem {
private:
    /** @brief Buffer of inputs not yet acknowledged by the server */
    std::deque<InputCommand> unacknowledgedInputs;

    /** @brief Predicted current position (integrated locally) */
    float predictedX, predictedY;
    float predictedVx, predictedVy;

    /** @brief Last acknowledged position from the server */
    float serverX, serverY;
    float serverVx, serverVy;
    uint32_t lastAckedSequence;

    /** @brief Error accumulator for smooth correction (pixels offset to apply over time) */
    float errorX, errorY;
    static constexpr float ERROR_CORRECTION_RATE = 5.0f;  ///< Units per second

    /** @brief Max number of buffered unacknowledged inputs (protection against network spikes) */
    static constexpr size_t MAX_UNACKED_INPUTS = 120;     ///< ~2 seconds at 60 FPS

public:
    /**
     * @brief Construct a new PredictionSystem.
     * @param initialX Starting X position for the predicted object
     * @param initialY Starting Y position for the predicted object
     */
    PredictionSystem(float initialX, float initialY);

    /**
     * @brief Apply a local input and update predicted state (buffering for reconciliation).
     * @param input The InputCommand representing user action for this frame/tick
     */
    void applyInput(const InputCommand& input);

    /**
     * @brief Reconcile with authoritative server state (rollback, replay, and error correction).
     *
     * When a server packet is received, the system:
     *   1. Discards inputs older than server's sequence/ack.
     *   2. Resets the predicted state to match the server.
     *   3. Reapplies all still-unacknowledged inputs in buffer.
     *   4. Calculates offset (error) and begins smooth correction.
     *
     * @param serverPacket The latest Packet received from server (authoritative state).
     */
    void reconcileWithServer(const Packet& serverPacket);

    /**
     * @brief Progresses error correction and updates predicted state for the current frame.
     * @param dt Time elapsed since last update (in seconds)
     */
    void update(float dt);

    /**
     * @brief Get the current predicted position.
     * @return Predicted position as (x, y) pair
     */
    std::pair<float, float> getPredictedPosition() const;

    /**
     * @brief Get the current predicted velocity.
     * @return Predicted velocity as (vx, vy) pair
     */
    std::pair<float, float> getPredictedVelocity() const;

    /**
     * @brief Number of unacknowledged inputs currently buffered.
     * @return Number of inputs waiting for server acknowledgment
     */
    size_t getUnackedInputCount() const { return unacknowledgedInputs.size(); }

    /**
     * @brief Returns true if too many inputs are unacknowledged and we should throttle sending.
     * @return True if input buffer is more than half full
     */
    bool shouldThrottle() const { return unacknowledgedInputs.size() > MAX_UNACKED_INPUTS / 2; }

private:
    /**
     * @brief Helper: Applies a single input to a given state (used for simulation and reconciliation).
     * @param[in,out] x   X position to update
     * @param[in,out] y   Y position to update
     * @param[in,out] vx  X velocity to update
     * @param[in,out] vy  Y velocity to update
     * @param input       InputCommand to apply
     */
    void applyInputToState(float& x, float& y, float& vx, float& vy, const InputCommand& input);
};
