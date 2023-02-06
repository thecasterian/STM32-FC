#include "command_tab.hpp"
#include "ui_command_tab.h"

CommandTab::CommandTab(PortManager *port_mgr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandTab),
    port_mgr(port_mgr)
{
    this->ui->setupUi(this);
}

CommandTab::~CommandTab()
{
    delete this->ui;
}
