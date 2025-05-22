/**
 * @file prediction.cpp
 * @brief Implementation of client-side prediction for the netcode demo.
 *
 * Defines predictPosition() declared in prediction.hpp.
 *
 * @see prediction.hpp
 * @author Aryan Malekian
 * @date 21.05.2025
 */

#include "netcode/common/prediction.hpp"

std::pair<float, float> predictPosition(const Packet& pkt, float dt) {
    return {
        pkt.x + pkt.vx * dt,
        pkt.y + pkt.vy * dt
    };
}
