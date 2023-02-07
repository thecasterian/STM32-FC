#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP

#include "port_manager.hpp"

class CommandManager : public QObject
{
    Q_OBJECT

public:
    explicit CommandManager(PortManager *port_mgr);

    bool toggleDebugLed(uint8_t led, bool on);

private:
    PortManager *port_mgr;

private slots:
    void receiveResp(uint8_t ack, uint8_t err);
};

#endif
