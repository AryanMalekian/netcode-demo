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
 *   - Use Packet::isValid() to validate packet contents after deserialization.
 *
 * @author Aryan Malekian w/ use of A.I. Models
 * @date 20.05.2025
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <cmath>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>  
#endif

 /**
  * @struct Packet
  * @brief A fixed-size UDP packet format carrying sequence, position, and velocity.
  *
  * All fields are laid out in network byte order.
  * Use serialize() before sending and deserialize() after receiving.
  * Always validate with isValid() after deserializing received packets.
  */
struct Packet {
    uint32_t seq;  /**< Packet sequence number (wraps at 2^32) */
    float x, y;    /**< 2D position of the object */
    float vx, vy;  /**< 2D velocity (units per second) */

    /**
     * @brief Default constructor initializes all fields to zero.
     */
    Packet() : seq(0), x(0.0f), y(0.0f), vx(0.0f), vy(0.0f) {}

    /**
     * @brief Constructor with explicit values.
     */
    Packet(uint32_t sequence, float posX, float posY, float velX, float velY)
        : seq(sequence), x(posX), y(posY), vx(velX), vy(velY) {}

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
     * @brief Validates packet contents for reasonable bounds and values.
     * @return True if packet contains valid data, false if corrupted/invalid
     */
    bool isValid() const {
        // A validation check for NaN or otherwise infinite values
        if (!std::isfinite(x) || !std::isfinite(y) ||
            !std::isfinite(vx) || !std::isfinite(vy)) {
            return false;
        }

        
        constexpr float MAX_POSITION = 10000.0f;
        if (std::abs(x) > MAX_POSITION || std::abs(y) > MAX_POSITION) {
            return false;
        }

        
        constexpr float MAX_VELOCITY = 1000.0f;
        if (std::abs(vx) > MAX_VELOCITY || std::abs(vy) > MAX_VELOCITY) {
            return false;
        }

        
        if (seq == 0) {
            return false;
        }

        return true;
    }

    /**
     * @brief Returns the number of bytes required to serialize this Packet.
     * @return size in bytes
     */
    static constexpr size_t size() {
        return sizeof(seq) + 4 * sizeof(float);
    }
};