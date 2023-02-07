#include "command_tab.hpp"
#include "ui_command_tab.h"
#include "moc_command_tab.cpp"

CommandTab::CommandTab(CommandManager *cmd_mgr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandTab),
    cmd_mgr(cmd_mgr)
{
    this->ui->setupUi(this);

    connect(this->ui->frontLeftSlider, &QSlider::valueChanged, this, &CommandTab::setFrontLeftEdit);
    connect(this->ui->frontRightSlider, &QSlider::valueChanged, this, &CommandTab::setFrontRightEdit);
    connect(this->ui->rearLeftSlider, &QSlider::valueChanged, this, &CommandTab::setRearLeftEdit);
    connect(this->ui->rearRightSlider, &QSlider::valueChanged, this, &CommandTab::setRearRightEdit);

    connect(this->ui->frontLeftEdit, &QLineEdit::textChanged, this, &CommandTab::setFrontLeftSlider);
    connect(this->ui->frontRightEdit, &QLineEdit::textChanged, this, &CommandTab::setFrontRightSlider);
    connect(this->ui->rearLeftEdit, &QLineEdit::textChanged, this, &CommandTab::setRearLeftSlider);
    connect(this->ui->rearRightEdit, &QLineEdit::textChanged, this, &CommandTab::setRearRightSlider);

    connect(this->ui->updatePushButton, &QPushButton::clicked, this, &CommandTab::updateThrottle);
    connect(this->ui->pwmPushButton, &QPushButton::toggled, this, &CommandTab::togglePwm);

    connect(this->ui->redLedPushButton, &QPushButton::toggled, this, &CommandTab::toggleRedLed);
    connect(this->ui->greenLedPushButton, &QPushButton::toggled, this, &CommandTab::toggleGreenLed);
    connect(this->ui->blueLedPushButton, &QPushButton::toggled, this, &CommandTab::toggleBlueLed);
}

CommandTab::~CommandTab()
{
    delete this->ui;
}

void CommandTab::setFrontLeftEdit(int value)
{
    bool old_state = this->ui->frontLeftEdit->blockSignals(true);
    this->ui->frontLeftEdit->setText(QString::number(value));
    this->ui->frontLeftEdit->blockSignals(old_state);
}

void CommandTab::setFrontRightEdit(int value)
{
    bool old_state = this->ui->frontRightEdit->blockSignals(true);
    this->ui->frontRightEdit->setText(QString::number(value));
    this->ui->frontRightEdit->blockSignals(old_state);
}

void CommandTab::setRearLeftEdit(int value)
{
    bool old_state = this->ui->rearLeftEdit->blockSignals(true);
    this->ui->rearLeftEdit->setText(QString::number(value));
    this->ui->rearLeftEdit->blockSignals(old_state);
}

void CommandTab::setRearRightEdit(int value)
{
    bool old_state = this->ui->rearRightEdit->blockSignals(true);
    this->ui->rearRightEdit->setText(QString::number(value));
    this->ui->rearRightEdit->blockSignals(old_state);
}

void CommandTab::setFrontLeftSlider(const QString &value)
{
    bool success = false;
    int value_int = value.toInt(&success);
    if (success)
    {
        if (value_int > 100)
        {
            value_int = 100;
            bool old_state = this->ui->frontLeftEdit->blockSignals(true);
            this->ui->frontLeftEdit->setText(QString::number(value_int));
            this->ui->frontLeftEdit->blockSignals(old_state);
        }

        bool old_state = this->ui->frontLeftSlider->blockSignals(true);
        this->ui->frontLeftSlider->setValue(value_int);
        this->ui->frontLeftSlider->blockSignals(old_state);
    }
    else
    {
        bool old_state = this->ui->frontLeftEdit->blockSignals(true);
        this->ui->frontLeftEdit->setText(QString::number(this->ui->frontLeftSlider->value()));
        this->ui->frontLeftEdit->blockSignals(old_state);
    }
}

void CommandTab::setFrontRightSlider(const QString &value)
{
    bool success = false;
    int value_int = value.toInt(&success);
    if (success)
    {
        if (value_int > 100)
        {
            value_int = 100;
            bool old_state = this->ui->frontRightEdit->blockSignals(true);
            this->ui->frontRightEdit->setText(QString::number(value_int));
            this->ui->frontRightEdit->blockSignals(old_state);
        }

        bool old_state = this->ui->frontRightSlider->blockSignals(true);
        this->ui->frontRightSlider->setValue(value_int);
        this->ui->frontRightSlider->blockSignals(old_state);
    }
    else
    {
        bool old_state = this->ui->frontRightEdit->blockSignals(true);
        this->ui->frontRightEdit->setText(QString::number(this->ui->frontRightSlider->value()));
        this->ui->frontRightEdit->blockSignals(old_state);
    }
}

void CommandTab::setRearLeftSlider(const QString &value)
{
    bool success = false;
    int value_int = value.toInt(&success);
    if (success)
    {
        if (value_int > 100)
        {
            value_int = 100;
            bool old_state = this->ui->rearLeftEdit->blockSignals(true);
            this->ui->rearLeftEdit->setText(QString::number(value_int));
            this->ui->rearLeftEdit->blockSignals(old_state);
        }

        bool old_state = this->ui->rearLeftSlider->blockSignals(true);
        this->ui->rearLeftSlider->setValue(value_int);
        this->ui->rearLeftSlider->blockSignals(old_state);
    }
    else
    {
        bool old_state = this->ui->rearLeftEdit->blockSignals(true);
        this->ui->rearLeftEdit->setText(QString::number(this->ui->rearLeftSlider->value()));
        this->ui->rearLeftEdit->blockSignals(old_state);
    }
}

void CommandTab::setRearRightSlider(const QString &value)
{
    bool success = false;
    int value_int = value.toInt(&success);
    if (success)
    {
        if (value_int > 100)
        {
            value_int = 100;
            bool old_state = this->ui->rearRightEdit->blockSignals(true);
            this->ui->rearRightEdit->setText(QString::number(value_int));
            this->ui->rearRightEdit->blockSignals(old_state);
        }

        bool old_state = this->ui->rearRightSlider->blockSignals(true);
        this->ui->rearRightSlider->setValue(value_int);
        this->ui->rearRightSlider->blockSignals(old_state);
    }
    else
    {
        bool old_state = this->ui->rearRightEdit->blockSignals(true);
        this->ui->rearRightEdit->setText(QString::number(this->ui->rearRightSlider->value()));
        this->ui->rearRightEdit->blockSignals(old_state);
    }
}

void CommandTab::updateThrottle(void)
{
    this->cmd_mgr->setThrottle(this->ui->frontLeftSlider->value() * 100U, this->ui->frontRightSlider->value() * 100U,
                               this->ui->rearLeftSlider->value() * 100U, this->ui->rearRightSlider->value() * 100U);
}

void CommandTab::togglePwm(bool checked)
{
    if (!this->cmd_mgr->togglePwm(checked))
    {
        bool old_state = this->ui->pwmPushButton->blockSignals(true);
        this->ui->pwmPushButton->setChecked(!checked);
        this->ui->pwmPushButton->blockSignals(old_state);
    }
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
