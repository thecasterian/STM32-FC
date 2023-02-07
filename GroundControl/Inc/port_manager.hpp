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
    Q_OBJECT

public:
    PortManager();
    ~PortManager();

    bool isOpen(void);
    bool open(const PortInfo &info);
    void close(void);

signals:
    void respReceived(uint8_t ack, uint8_t err);
    void strmReceived(uint8_t len, const uint8_t *dat);

private:
    QSerialPort port;
    PacketParser parser;

private slots:
    void receive(void);
};

#endif
