#include "command_tab.hpp"
#include "ui_command_tab.h"
#include "moc_command_tab.cpp"

CommandTab::CommandTab(PortManager *port_mgr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandTab),
    port_mgr(port_mgr)
{
    this->ui->setupUi(this);

    connect(this->ui->redLedPushButton, &QPushButton::toggled, this, &CommandTab::toggleRedLed);
}

CommandTab::~CommandTab()
{
    delete this->ui;
}

void CommandTab::toggleRedLed(bool checked)
{

}

void CommandTab::toggleGreenLed(bool checked)
{

}

void CommandTab::toggleBlueLed(bool checked)
{

}
