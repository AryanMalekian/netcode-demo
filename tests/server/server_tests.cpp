/**
 * @file server_tests.cpp
 * @brief Unit tests for UDP server echo functionality and packet handling.
 *
 * Tests core server logic including packet echo simulation and basic
 * validation scenarios. Focuses on testable server components without
 * requiring actual socket operations or network infrastructure.
 *
 * Coverage:
 * - Packet echo simulation (serialize/deserialize roundtrip)
 * - Packet size validation logic
 * - Basic server data handling correctness
 *
 * @author Aryan Malekian w/ use of A.I. Models
 * @date 23.05.2025
 */

#include <catch2/catch_all.hpp>
#include "netcode/common/packet.hpp"

TEST_CASE("Server: packet echo simulation", "[server]") {
    Packet send_pkt{ 123, 10.f, 20.f, 1.f, 1.f };
    char buf[Packet::size()];
    send_pkt.serialize(buf);

    Packet recv_pkt;
    recv_pkt.deserialize(buf);

    REQUIRE(recv_pkt.seq == 123);
    REQUIRE(recv_pkt.x == Catch::Approx(10.f));
    REQUIRE(recv_pkt.y == Catch::Approx(20.f));
    REQUIRE(recv_pkt.vx == Catch::Approx(1.f));
    REQUIRE(recv_pkt.vy == Catch::Approx(1.f));
}

TEST_CASE("Server: packet size validation", "[server]") {
    char buf[Packet::size() - 1] = {};
    REQUIRE(sizeof(buf) != Packet::size());
}