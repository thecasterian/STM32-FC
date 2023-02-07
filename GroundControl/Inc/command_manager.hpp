#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP

#include "port_manager.hpp"

class CommandManager
{
public:
    explicit CommandManager(PortManager *port_mgr);
    ~CommandManager();

    void toggleRedLed(bool on);
    void toggleGreenLed(bool on);
    void toggleBlueLed(bool on);
};

#endif
