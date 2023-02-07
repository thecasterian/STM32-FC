#include <QDebug>
#include "command_manager.hpp"
#include "moc_command_manager.cpp"

static void packet_calculate_checksum(uint8_t *packet);

CommandManager::CommandManager(PortManager *port_mgr) :
    port_mgr(port_mgr)
{
    connect(this->port_mgr, &PortManager::respReceived, this, &CommandManager::receiveResp);
}

bool CommandManager::toggleDebugLed(uint8_t led, bool on)
{
    uint8_t arg = on ? (uint8_t)0x01 : (uint8_t)0x00U;
    uint8_t packet[7] = { PACKET_STX, PACKET_TYP_CMD, 0x02, led, arg, 0x00, PACKET_ETX };

    packet_calculate_checksum(packet);

    return port_mgr->send(7, packet);
}

void CommandManager::receiveResp(uint8_t ack, uint8_t err)
{
    Q_UNUSED(ack);
    Q_UNUSED(err);
}

static void packet_calculate_checksum(uint8_t *packet)
{
    const uint8_t len = packet[2];
    uint8_t checksum = 0U;

    for (uint8_t i = 3U; i < len + 3U; i++)
    {
        checksum += packet[i];
    }

    packet[len + 3U] = checksum;
}
