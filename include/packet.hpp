/**
 * @file packet.hpp
 * @brief Defines the Packet struct used for UDP communication in the netcode demo.
 *
 * The Packet struct provides a fixed-size format for sending player state
 * (sequence number, position, and velocity) across the network.
 * All fields are stored in network byte order when serialized.
 *
 * Usage:
 *   - Use Packet::serialize() before sending over the network.
 *   - Use Packet::deserialize() after receiving from the network.
 *
 * @author Aryan Malekian
 * @date 20.05.2025
 */

#pragma once

#include <cstdint>
#include <winsock2.h>
#include <cstring>

 /**
  * @struct Packet
  * @brief A fixed-size UDP packet format carrying sequence, position, and velocity.
  *
  * All fields are laid out in network byte order.
  * Use serialize() before sending and deserialize() after receiving.
  */
struct Packet {
    uint32_t seq;  /**< Packet sequence number (wraps at 2^32) */
    float x, y;    /**< 2D position of the object */
    float vx, vy;  /**< 2D velocity (units per second) */

    /**
     * @brief Serialize this Packet into a raw buffer.
     * @param[out] buf  Pointer to a writable buffer of at least Packet::size() bytes.
     */
    void serialize(char* buf) const {
        uint32_t nseq = htonl(seq);
        memcpy(buf, &nseq, sizeof(nseq));
        memcpy(buf + 4, &x, sizeof(x));
        memcpy(buf + 4 + sizeof(x), &y, sizeof(y));
        memcpy(buf + 4 + 2 * sizeof(x), &vx, sizeof(vx));
        memcpy(buf + 4 + 2 * sizeof(x) + sizeof(vx), &vy, sizeof(vy));
    }

    /**
     * @brief Deserialize this Packet from a raw buffer.
     * @param[in] buf  Pointer to a buffer containing at least Packet::size() bytes.
     */
    void deserialize(const char* buf) {
        uint32_t nseq;
        memcpy(&nseq, buf, sizeof(nseq));
        seq = ntohl(nseq);
        memcpy(&x, buf + 4, sizeof(x));
        memcpy(&y, buf + 4 + sizeof(x), sizeof(y));
        memcpy(&vx, buf + 4 + 2 * sizeof(x), sizeof(vx));
        memcpy(&vy, buf + 4 + 2 * sizeof(x) + sizeof(vx), sizeof(vy));
    }

    /**
     * @brief Returns the number of bytes required to serialize this Packet.
     * @return size in bytes
     */
    static constexpr size_t size() {
        return sizeof(seq) + 4 * sizeof(float);
    }
};
