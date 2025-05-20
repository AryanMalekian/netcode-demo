#pragma once

#include <cstdint>
#include <winsock2.h>  
#include <cstring>     

struct Packet {
    uint32_t seq;  // sequence number
    float x, y;    // position
    float vx, vy;  // velocity

    void serialize(char* buf) const {
        uint32_t nseq = htonl(seq);
        memcpy(buf, &nseq, sizeof(nseq));
        memcpy(buf + 4, &x, sizeof(x));
        memcpy(buf + 4 + sizeof(x), &y, sizeof(y));
        memcpy(buf + 4 + 2 * sizeof(x), &vx, sizeof(vx));
        memcpy(buf + 4 + 2 * sizeof(x) + sizeof(vx), &vy, sizeof(vy));
    }

    // Used to deserialize (read data back from a buffer to a Packet struct)
    void deserialize(const char* buf) {
        uint32_t nseq;
        memcpy(&nseq, buf, sizeof(nseq));
        seq = ntohl(nseq);
        memcpy(&x, buf + 4, sizeof(x));
        memcpy(&y, buf + 4 + sizeof(x), sizeof(y));
        memcpy(&vx, buf + 4 + 2 * sizeof(x), sizeof(vx));
        memcpy(&vy, buf + 4 + 2 * sizeof(x) + sizeof(vx), sizeof(vy));
    }

    // Total size of the serialized packet
    static constexpr size_t size() {
        return sizeof(seq) + 4 * sizeof(float);
    }
};
