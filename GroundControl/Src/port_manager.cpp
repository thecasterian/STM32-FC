#include <QDebug>
#include "port_manager.hpp"
#include "moc_port_manager.cpp"

PortManager::PortManager()
{
    connect(&this->port, &QSerialPort::readyRead, this, &PortManager::receive);
    connect(&this->port, &QSerialPort::errorOccurred, this, &PortManager::onPortError);
}

PortManager::~PortManager()
{
    if (this->isOpen())
    {
        this->close();
    }
}

bool PortManager::isOpen(void)
{
    return this->port.isOpen();
}

bool PortManager::open(const PortInfo &info)
{
    if (info.name.isEmpty())
    {
        qWarning() << "port name is empty";
        return false;
    }

    bool buad_rate_ok = false;
    int buad_rate_int = info.baud_rate.toInt(&buad_rate_ok);
    if (!buad_rate_ok)
    {
        qWarning() << "invalid baud rate:" << info.baud_rate;
        return false;
    }

    this->port.setPortName(info.name);
    this->port.setBaudRate(buad_rate_int);

    if (this->port.open(QIODevice::ReadWrite))
    {
        qDebug() << "port" << this->port.portName() << "opened";
        return true;
    }
    else
    {
        qWarning() << "failed to open port" << this->port.portName();
        return false;
    }
}

void PortManager::close(void)
{
    this->port.close();
    qDebug() << "port" << this->port.portName() << "closed";
}

bool PortManager::send(uint16_t len, const uint8_t *data)
{
    if (!this->isOpen())
    {
        qWarning() << "port is not open";
        return false;
    }

    if (this->port.write((const char *)data, len) != len)
    {
        qWarning() << "failed to write to port" << this->port.portName();
        return false;
    }

    return true;
}

void PortManager::receive(void)
{
    this->parser.append(this->port.readAll());

    while (this->parser.isPacketAvailable())
    {
        Packet packet = this->parser.getPacket();

        if (packet.typ == PACKET_TYP_RESP)
        {
            emit this->respReceived(packet.dat[0], packet.dat[1]);
        }
        else if (packet.typ == PACKET_TYP_STRM)
        {
            emit this->strmReceived(packet.len, packet.dat);
        }
    }
}

void PortManager::onPortError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError)
    {
        qWarning() << "error" << error << "occured on port" << this->port.portName();

        bool old_state = this->port.blockSignals(true);
        this->close();
        this->port.blockSignals(old_state);

        emit this->portClosed();
    }
}
