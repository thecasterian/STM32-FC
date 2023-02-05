#include "command_tab.hpp"
#include "ui_command_tab.h"

CommandTab::CommandTab(QSerialPort *serial_port, QWidget *parent):
    QWidget(parent),
    ui(new Ui::CommandTab),
    serial_port(serial_port) {
    this->ui->setupUi(this);
}

CommandTab::~CommandTab() {
    delete this->ui;
}
