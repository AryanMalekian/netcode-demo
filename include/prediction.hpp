/**
 * @file prediction.hpp
 * @brief Client-side prediction interface for the netcode demo.
 *
 * Declares a function to predict an object’s position given its
 * last known state (position + velocity) and elapsed time.
 * This separation makes prediction logic testable and reusable.
 *
 * @author Aryan Malekian
 * @date 21.05.2025
 */

#pragma once

#include "packet.hpp"
#include <utility>  // for std::pair

 /**
  * @brief Predicts where an object will be after a time delta.
  *
  * Uses simple linear extrapolation:
  *   predictedX = pkt.x + pkt.vx * dt
  *   predictedY = pkt.y + pkt.vy * dt
  *
  * @param pkt Last received Packet containing position (x,y) and velocity (vx,vy).
  * @param dt  Time elapsed since pkt was received (in seconds).
  * @return    A std::pair<float,float> = (predictedX, predictedY).
  */
std::pair<float, float> predictPosition(const Packet& pkt, float dt);
