#include "command_tab.hpp"
#include "ui_command_tab.h"
#include "moc_command_tab.cpp"

CommandTab::CommandTab(CommandManager *cmd_mgr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandTab),
    cmd_mgr(cmd_mgr)
{
    this->ui->setupUi(this);

    connect(this->ui->redLedPushButton, &QPushButton::toggled, this, &CommandTab::toggleRedLed);
    connect(this->ui->greenLedPushButton, &QPushButton::toggled, this, &CommandTab::toggleGreenLed);
    connect(this->ui->blueLedPushButton, &QPushButton::toggled, this, &CommandTab::toggleBlueLed);
}

CommandTab::~CommandTab()
{
    delete this->ui;
}

void CommandTab::toggleRedLed(bool checked)
{
    if (!this->cmd_mgr->toggleDebugLed(PACKET_CMD_LED_RED, checked))
    {
        bool old_state = this->ui->redLedPushButton->blockSignals(true);
        this->ui->redLedPushButton->setChecked(!checked);
        this->ui->redLedPushButton->blockSignals(old_state);
    }
}

void CommandTab::toggleGreenLed(bool checked)
{
    if (!this->cmd_mgr->toggleDebugLed(PACKET_CMD_LED_GREEN, checked))
    {
        bool old_state = this->ui->greenLedPushButton->blockSignals(true);
        this->ui->greenLedPushButton->setChecked(!checked);
        this->ui->greenLedPushButton->blockSignals(old_state);
    }
}

void CommandTab::toggleBlueLed(bool checked)
{
    if (!this->cmd_mgr->toggleDebugLed(PACKET_CMD_LED_BLUE, checked))
    {
        bool old_state = this->ui->blueLedPushButton->blockSignals(true);
        this->ui->blueLedPushButton->setChecked(!checked);
        this->ui->blueLedPushButton->blockSignals(old_state);
    }
}
