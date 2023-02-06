#ifndef PORT_MANAGER_HPP
#define PORT_MANAGER_HPP

#include <QtSerialPort>

class PortInfo
{
public:
    QString name;
    QString baud_rate;
};

class PortManager
{
public:
    PortManager();
    ~PortManager();

    bool isOpen(void);
    bool open(const PortInfo &info);
    void close(void);

private:
    QSerialPort port;
};

#endif
