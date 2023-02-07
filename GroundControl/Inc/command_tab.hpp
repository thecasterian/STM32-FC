#ifndef COMMAND_TAB_HPP
#define COMMAND_TAB_HPP

#include <QWidget>
#include "port_manager.hpp"

namespace Ui
{
class CommandTab;
}

class CommandTab : public QWidget
{
    Q_OBJECT

public:
    explicit CommandTab(PortManager *port_mgr, QWidget *parent = nullptr);
    ~CommandTab();

private:
    Ui::CommandTab *ui;

    PortManager *port_mgr;

private slots:
    void toggleRedLed(bool checked);
    void toggleGreenLed(bool checked);
    void toggleBlueLed(bool checked);
};

#endif
