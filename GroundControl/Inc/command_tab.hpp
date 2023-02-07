#ifndef COMMAND_TAB_HPP
#define COMMAND_TAB_HPP

#include <QWidget>
#include "command_manager.hpp"

namespace Ui
{
class CommandTab;
}

class CommandTab : public QWidget
{
    Q_OBJECT

public:
    explicit CommandTab(CommandManager *cmd_mgr, QWidget *parent = nullptr);
    ~CommandTab();

private:
    Ui::CommandTab *ui;

    CommandManager *cmd_mgr;

private slots:
    void toggleRedLed(bool checked);
    void toggleGreenLed(bool checked);
    void toggleBlueLed(bool checked);
};

#endif
