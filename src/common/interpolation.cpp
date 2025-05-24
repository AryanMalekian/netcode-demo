/**
 * @file interpolation.cpp
 * @brief Implementation of interpolation for the netcode demo.
 *
 * @author Aryan Malekian
 * @date 21.05.2025
 */

#include "netcode/common/interpolation.hpp"

std::pair<float, float> interpolatePosition(const Packet& prev, const Packet& next, float t) {
    float x = prev.x + (next.x - prev.x) * t;
    float y = prev.y + (next.y - prev.y) * t;
    return { x, y };
}
