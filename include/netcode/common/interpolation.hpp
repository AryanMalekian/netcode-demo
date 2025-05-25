/**
 * @file interpolation.hpp
 * @brief Linear interpolation utilities for smooth network state transitions.
 *
 * Provides interpolation functions for creating smooth visual transitions
 * between discrete network updates. Used for client-side rendering to
 * eliminate visual jerkiness caused by infrequent server updates.
 *
 * Usage:
 *   - Call interpolatePosition() between two consecutive server packets
 *   - Use interpolation factor t based on time elapsed since packets
 *   - Combine with prediction for comprehensive lag compensation
 *
 * @author Aryan Malekian w/ use of A.I. Models
 * @date 21.05.2025
 */

#pragma once

#include "packet.hpp"
#include <utility>

 /**
  * @brief Linearly interpolate between two Packet positions for smooth rendering.
  *
  * Creates smooth visual transitions between discrete network updates by
  * blending position data from two consecutive server packets based on timing.
  *
  * Formula: result = prev + (next - prev) * t
  *
  * @param prev First packet (earlier in time, t=0 reference)
  * @param next Second packet (later in time, t=1 reference)
  * @param t    Interpolation factor in [0,1]: t=0 returns prev; t=1 returns next
  * @return     Interpolated position (x, y) as a pair
  */
std::pair<float, float> interpolatePosition(const Packet& prev, const Packet& next, float t);