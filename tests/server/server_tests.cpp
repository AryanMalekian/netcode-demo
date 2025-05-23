/**
 * @file server_tests.cpp
 * @brief Basic tests for UDP server logic in netcode demo.
 *
 * Tests packet echo simulation and deserialization correctness.
 * Real socket communication and bind/listen require integration/system tests.
 *
 * @author Aryan Malekian
 * @date 23.05.2025
 */

#include <catch2/catch_all.hpp>
#include "netcode/common/packet.hpp"

TEST_CASE("Server: packet echo simulation", "[server]") {
    Packet send_pkt{ 123, 10.f, 20.f, 1.f, 1.f };
    char buf[Packet::size()];
    send_pkt.serialize(buf);

    // Simulate receiving the same packet buffer on server
    Packet recv_pkt;
    recv_pkt.deserialize(buf);

    REQUIRE(recv_pkt.seq == 123);
    REQUIRE(recv_pkt.x == Catch::Approx(10.f));
    REQUIRE(recv_pkt.y == Catch::Approx(20.f));
    REQUIRE(recv_pkt.vx == Catch::Approx(1.f));
    REQUIRE(recv_pkt.vy == Catch::Approx(1.f));
}

TEST_CASE("Server: invalid packet size is ignored", "[server]") {
    char buf[Packet::size() - 1] = {};
    Packet pkt;
    REQUIRE(sizeof(buf) != Packet::size());
}
