#ifndef COMMAND_TAB_HPP
#define COMMAND_TAB_HPP

#include <QWidget>
#include <QSerialPort>

namespace Ui {
class CommandTab;
}

class CommandTab: public QWidget {
public:
    explicit CommandTab(QSerialPort *serial_port, QWidget *parent = nullptr);
    ~CommandTab();

private:
    Ui::CommandTab *ui;
    QSerialPort *serial_port;
};

#endif
