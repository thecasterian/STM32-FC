#ifndef PORT_MANAGER_HPP
#define PORT_MANAGER_HPP

#include <QObject>
#include <QtSerialPort>
#include "packet_parser.hpp"

class PortInfo
{
public:
    QString name;
    QString baud_rate;
};

class PortManager : public QObject
{
public:
    PortManager();
    ~PortManager();

    bool isOpen(void);
    bool open(const PortInfo &info);
    void close(void);

private:
    QSerialPort port;
    PacketParser parser;

private slots:
    void receive(void);
};

#endif
