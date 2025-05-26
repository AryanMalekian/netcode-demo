/**
 * @file input.hpp
 * @brief Input command structure for client-side prediction and reconciliation.
 *
 * Defines the InputCommand struct that represents timestamped user input
 * for netcode prediction systems. Each command contains normalized input
 * values and timing information needed for input replay during reconciliation.
 *
 * Usage:
 *   - Create InputCommand for each frame's user input
 *   - Buffer commands until server acknowledgment
 *   - Replay buffered commands during reconciliation
 *   - Use sequence numbers for ordering and acknowledgment
 *
 * @author Aryan Malekian & Jonathan Skomsøy Hübertz,  w/ use of A.I. Models 
 * @date 23.05.2025
 */

#pragma once

#include <cstdint>

 /**
  * @struct InputCommand
  * @brief Represents a timestamped input command for prediction systems.
  *
  * Each input command captures user input state at a specific moment,
  * tagged with sequence number and timing information. This enables
  * the prediction system to replay exact input sequences during
  * server reconciliation for deterministic state reconstruction.
  *
  * Features:
  * - Sequence-based ordering and acknowledgment
  * - Normalized velocity inputs for consistent behavior
  * - Frame timing for accurate replay simulation
  */
struct InputCommand {
    uint32_t sequence;  /**< Sequence number for ordering and server acknowledgment */
    float vx;          /**< X-axis velocity input (-1 to 1, normalized from user input) */
    float vy;          /**< Y-axis velocity input (-1 to 1, normalized from user input) */
    float dt;          /**< Delta time for this input frame (seconds) */

    /**
     * @brief Default constructor initializes all values to zero.
     */
    InputCommand() : sequence(0), vx(0.0f), vy(0.0f), dt(0.0f) {}

    /**
     * @brief Construct an input command with specific values.
     *
     * @param seq Input sequence number for ordering
     * @param velocityX X-axis velocity (-1 to 1, normalized)
     * @param velocityY Y-axis velocity (-1 to 1, normalized)
     * @param deltaTime Time delta for this frame (seconds)
     */
    InputCommand(uint32_t seq, float velocityX, float velocityY, float deltaTime)
        : sequence(seq), vx(velocityX), vy(velocityY), dt(deltaTime) {}
};