/**
 * @file prediction.cpp
 * @brief Implementation of client-side prediction for the netcode demo.
 *
 * Defines predictPosition() declared in prediction.hpp.
 *
 * @see prediction.hpp
 * @author Aryan Malekian
 * @date 2025-05-21
 */

#include "../include/prediction.hpp"

std::pair<float, float> predictPosition(const Packet& pkt, float dt) {
    return {
        pkt.x + pkt.vx * dt,
        pkt.y + pkt.vy * dt
    };
}
