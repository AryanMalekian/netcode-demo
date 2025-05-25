/**
 * @file interpolation.hpp
 * @brief Linear interpolation between two Packet states.
 *
 * @author Aryan Malekian w/ use of A.I. Models
 * @date 21.05.2025
 */

#pragma once

#include "packet.hpp"
#include <utility>

 /**
  * @brief Linearly interpolate between two Packet positions.
  *
  * @param prev First packet (earlier in time)
  * @param next Second packet (later in time)
  * @param t    Interpolation factor in [0,1]: t=0 returns prev; t=1 returns next.
  * @return     Interpolated position (x, y) as a pair
  */
std::pair<float, float> interpolatePosition(const Packet& prev, const Packet& next, float t);
