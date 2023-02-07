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
    void setFrontLeftEdit(int value);
    void setFrontRightEdit(int value);
    void setRearLeftEdit(int value);
    void setRearRightEdit(int value);

    void setFrontLeftSlider(const QString &value);
    void setFrontRightSlider(const QString &value);
    void setRearLeftSlider(const QString &value);
    void setRearRightSlider(const QString &value);

    void updateThrottle(void);
    void togglePwm(bool checked);

    void toggleRedLed(bool checked);
    void toggleGreenLed(bool checked);
    void toggleBlueLed(bool checked);
};

#endif
