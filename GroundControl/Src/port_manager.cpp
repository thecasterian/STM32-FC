#include <QDebug>
#include "port_manager.hpp"

PortManager::PortManager()
{

}

PortManager::~PortManager()
{

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
