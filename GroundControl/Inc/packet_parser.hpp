#ifndef PACKET_PARSER_HPP
#define PACKET_PARSER_HPP

#include <QQueue>
#include <QByteArray>
#include "protocol.hpp"

class PacketParser
{
public:
    PacketParser(void);

    void append(const QByteArray &data);
    bool isPacketAvailable(void);
    Packet getPacket(void);

private:
    bool stx;
    int cnt;
    uint8_t cks;
    uint8_t etx;
    Packet cur_packet;

    QQueue<Packet> packet_queue;
};

#endif
