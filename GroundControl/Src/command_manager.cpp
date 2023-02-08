#include <QDebug>
#include <cstring>
#include "command_manager.hpp"
#include "moc_command_manager.cpp"

static void packet_calculate_checksum(uint8_t *packet);

CommandManager::CommandManager(PortManager *port_mgr) :
    port_mgr(port_mgr)
{
    connect(this->port_mgr, &PortManager::respReceived, this, &CommandManager::receiveResp);
}

bool CommandManager::setStrm(const QVector<uint8_t> &ids)
{
    uint8_t packet[512] = { PACKET_STX, PACKET_TYP_CMD, 0x00, PACKET_CMD_STRM_DAT, 0x00, 0x00 };

    for (int i = 0; i < ids.size(); i++)
    {
        packet[i + 4] = ids[i];
    }
    packet[2] = ids.size() + 1;
    packet[ids.size() + 5] = PACKET_ETX;

    packet_calculate_checksum(packet);

    return port_mgr->send(packet[2] + 5U, packet);
}

bool CommandManager::toggleStrm(bool on)
{
    uint8_t arg = on ? (uint8_t)0x01 : (uint8_t)0x00U;
    uint8_t packet[7] = { PACKET_STX, PACKET_TYP_CMD, 0x02, PACKET_CMD_STRM, arg, 0x00, PACKET_ETX };

    packet_calculate_checksum(packet);

    return port_mgr->send(sizeof(packet), packet);
}

bool CommandManager::setThrottle(uint16_t front_left, uint16_t front_right, uint16_t rear_left, uint16_t rear_right)
{
    uint8_t packet[14] = {
        PACKET_STX, PACKET_TYP_CMD, 0x09, PACKET_CMD_THROT,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, PACKET_ETX
    };

    memcpy(&packet[4], &front_left, 2U);
    memcpy(&packet[6], &front_right, 2U);
    memcpy(&packet[8], &rear_left, 2U);
    memcpy(&packet[10], &rear_right, 2U);

    packet_calculate_checksum(packet);

    return port_mgr->send(sizeof(packet), packet);
}

bool CommandManager::togglePwm(bool on)
{
    uint8_t arg = on ? (uint8_t)0x01 : (uint8_t)0x00U;
    uint8_t packet[7] = { PACKET_STX, PACKET_TYP_CMD, 0x02, PACKET_CMD_PWM, arg, 0x00, PACKET_ETX };

    packet_calculate_checksum(packet);

    return port_mgr->send(sizeof(packet), packet);
}

bool CommandManager::toggleDebugLed(uint8_t led, bool on)
{
    uint8_t arg = on ? (uint8_t)0x01 : (uint8_t)0x00U;
    uint8_t packet[7] = { PACKET_STX, PACKET_TYP_CMD, 0x02, led, arg, 0x00, PACKET_ETX };

    packet_calculate_checksum(packet);

    return port_mgr->send(sizeof(packet), packet);
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
