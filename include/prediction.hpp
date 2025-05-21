/**
 * @file prediction.hpp
 * @brief Client-side prediction routines for netcode demo.
 *
 * Provides a simple extrapolation function that,
 * given the last known state (position + velocity) and elapsed time,
 * returns the predicted position.
 *
 * This is separated for clarity and unit-testing.
 *
 * @author Aryan Malekian
 * @date 21.05.2025
 */

#pragma once

#include "packet.hpp"
#include <utility>    
#include <cstdint>

 /**
  * @brief Predicts the new position of an object given its last state.
  *
  * Uses linear extrapolation:
  *   predictedX = pkt.x + pkt.vx * dt
  *   predictedY = pkt.y + pkt.vy * dt
  *
  * @param pkt Last received Packet containing x, y, vx, vy
  * @param dt  Time elapsed since pkt was received (in seconds)
  * @return    A std::pair<float,float> = (predictedX, predictedY)
  */
inline std::pair<float, float> predictPosition(const Packet& pkt, float dt) {
    return { pkt.x + pkt.vx * dt,
             pkt.y + pkt.vy * dt };
}
