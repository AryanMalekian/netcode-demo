/**
 * @file interpolation.cpp
 * @brief Implementation of linear interpolation utilities for smooth network state transitions.
 *
 * Implements interpolation functions used for creating smooth visual transitions
 * between discrete network updates. This eliminates visual jerkiness in client
 * rendering when server updates arrive infrequently or irregularly.
 *
 * See interpolation.hpp for API documentation.
 *
 * @see interpolation.hpp
 * @author Aryan Malekian w/ use of A.I. Models
 * @date 21.05.2025
 */

#include "netcode/common/interpolation.hpp"

 /**
  * @brief Linearly interpolates between two packet positions for smooth rendering.
  *
  * Performs linear interpolation using the formula: result = prev + (next - prev) * t
  * This creates smooth visual transitions between server state updates.
  *
  * @param prev Previous packet state (t=0 reference point)
  * @param next Next packet state (t=1 reference point)
  * @param t Interpolation factor [0,1] representing position between packets
  * @return Interpolated position coordinates as (x, y) pair
  */
std::pair<float, float> interpolatePosition(const Packet& prev, const Packet& next, float t) {
    float x = prev.x + (next.x - prev.x) * t;
    float y = prev.y + (next.y - prev.y) * t;
    return { x, y };
}