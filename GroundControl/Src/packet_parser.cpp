#include "packet_parser.hpp"

PacketParser::PacketParser(void) :
    stx(false),
    cnt(0)
{}

void PacketParser::append(const QByteArray &data)
{
    QByteArray::ConstIterator it = data.begin();

    if (!this->stx)
    {
        /* Find STX. */
        while (it != data.end())
        {
            if (*it == PACKET_STX)
            {
                this->stx = true;
                break;
            }
            ++it;
        }

        if (it != data.end() && *it == PACKET_STX)
        {
            /* STX is found. */
            this->stx = true;
            this->cnt++;
            ++it;
        }
        else
        {
            this->stx = false;
            this->cnt = 0;
        }
    }

    if (this->stx)
    {
        while (it != data.end())
        {
            if (this->cnt == 1)
            {
                /* Byte 1 is TYP. */
                this->cur_packet.typ = *it;
            }
            else if (this->cnt == 2)
            {
                /* Byte 2 is LEN. */
                this->cur_packet.len = *it;
            }
            else if (this->cnt < this->cur_packet.len + 3)
            {
                /* Byte 3 .. (LEN + 2) are in DAT. */
                this->cur_packet.dat[this->cnt - 3] = *it;
            }
            else if (this->cnt == this->cur_packet.len + 3)
            {
                /* Byte LEN + 3 is CKS. */
                this->cks = *it;
            }
            else
            {
                /* Last byte is ETX. */
                this->etx = *it;

                /* Calculate the checksum from DAT. */
                uint8_t cks_calc = 0U;
                for (uint8_t i = 0U; i < this->cur_packet.len; i++)
                {
                    cks_calc += this->cur_packet.dat[i];
                }

                if (this->cks == cks_calc && this->etx == PACKET_ETX)
                {
                    /* Packet is valid. */
                    this->packet_queue.enqueue(this->cur_packet);
                }
            }

            this->cnt++;
            ++it;
        }
    }
}

bool PacketParser::isPacketAvailable(void)
{
    return !this->packet_queue.isEmpty();
}

Packet PacketParser::getPacket(void)
{
    return this->packet_queue.dequeue();
}
