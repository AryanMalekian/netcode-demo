/**
 * @file input.hpp
 * @brief Input command structure for client-side prediction.
 *
 * Defines the InputCommand struct that represents player input at a specific time.
 * This is used for input buffering and reconciliation in the prediction system.
 *
 * @author Aryan Malekian w/ use of A.I. Models
 * @date 23.05.2025
 */

#pragma once

#include <cstdint>

 /**
  * @struct InputCommand
  * @brief Represents a single input command from the player.
  *
  * Each input command is timestamped with a sequence number and contains
  * the velocity inputs that were active at that time. This allows the
  * prediction system to replay inputs after receiving server corrections.
  */
struct InputCommand {
    uint32_t sequence;  /**< Sequence number of this input */
    float vx;          /**< X-axis velocity input (-1 to 1, normalized) */
    float vy;          /**< Y-axis velocity input (-1 to 1, normalized) */
    float dt;          /**< Delta time for this input frame */

    /**
     * @brief Default constructor initializes all values to zero.
     */
    InputCommand() : sequence(0), vx(0.0f), vy(0.0f), dt(0.0f) {}

    /**
     * @brief Construct an input command with specific values.
     *
     * @param seq Input sequence number
     * @param velocityX X-axis velocity
     * @param velocityY Y-axis velocity
     * @param deltaTime Time delta for this frame
     */
    InputCommand(uint32_t seq, float velocityX, float velocityY, float deltaTime)
        : sequence(seq), vx(velocityX), vy(velocityY), dt(deltaTime) {}
};