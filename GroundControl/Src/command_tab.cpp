#include <QDebug>
#include "command_tab.hpp"
#include "ui_command_tab.h"
#include "moc_command_tab.cpp"

CommandTab::CommandTab(CommandManager *cmd_mgr, PlotManager *plot_mgr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandTab),
    cmd_mgr(cmd_mgr),
    plot_mgr(plot_mgr)
{
    this->ui->setupUi(this);

    /* Fill the data list. */
    for (auto &info : strm_data_infos)
    {
        StrmDataListWidgetItem *item = new StrmDataListWidgetItem(info.name, info.id);
        this->strm_data_list_items.append(item);
        this->ui->availableListWidget->addItem(item);
    }

    connect(this->ui->moveToSelectedPushButton, &QPushButton::clicked, this, &CommandTab::moveToSelected);
    connect(this->ui->moveToAvailablePushButton, &QPushButton::clicked, this, &CommandTab::moveToAvailable);

    connect(this->ui->strmUpdatePushButton, &QPushButton::clicked, this, &CommandTab::updateStrm);
    connect(this->ui->strmPushButton, &QPushButton::toggled, this, &CommandTab::toggleStrm);

    connect(this->ui->frontLeftSlider, &QSlider::valueChanged, this, &CommandTab::setFrontLeftEdit);
    connect(this->ui->frontRightSlider, &QSlider::valueChanged, this, &CommandTab::setFrontRightEdit);
    connect(this->ui->rearLeftSlider, &QSlider::valueChanged, this, &CommandTab::setRearLeftEdit);
    connect(this->ui->rearRightSlider, &QSlider::valueChanged, this, &CommandTab::setRearRightEdit);

    connect(this->ui->frontLeftEdit, &QLineEdit::textChanged, this, &CommandTab::setFrontLeftSlider);
    connect(this->ui->frontRightEdit, &QLineEdit::textChanged, this, &CommandTab::setFrontRightSlider);
    connect(this->ui->rearLeftEdit, &QLineEdit::textChanged, this, &CommandTab::setRearLeftSlider);
    connect(this->ui->rearRightEdit, &QLineEdit::textChanged, this, &CommandTab::setRearRightSlider);

    connect(this->ui->pwmUpdatePushButton, &QPushButton::clicked, this, &CommandTab::updateThrottle);
    connect(this->ui->pwmPushButton, &QPushButton::toggled, this, &CommandTab::togglePwm);

    connect(this->ui->redLedPushButton, &QPushButton::toggled, this, &CommandTab::toggleRedLed);
    connect(this->ui->greenLedPushButton, &QPushButton::toggled, this, &CommandTab::toggleGreenLed);
    connect(this->ui->blueLedPushButton, &QPushButton::toggled, this, &CommandTab::toggleBlueLed);

    connect(this->cmd_mgr->getPortManager(), &PortManager::portClosed, this, &CommandTab::reset);
}

CommandTab::~CommandTab()
{
    delete this->ui;
    for (auto &it : this->strm_data_list_items)
    {
        delete it;
    }
}

void CommandTab::moveToSelected(void)
{
    auto selected = this->ui->availableListWidget->selectedItems();
    for (auto &it : selected)
    {
        StrmDataListWidgetItem *item = dynamic_cast<StrmDataListWidgetItem *>(it);
        if (item != nullptr)
        {
            this->ui->availableListWidget->takeItem(this->ui->availableListWidget->row(item));
            this->ui->selectedListWidget->addItem(item);

            qDebug() << "selected data" << item->text();
        }
    }
}

void CommandTab::moveToAvailable(void)
{
    auto selected = this->ui->selectedListWidget->selectedItems();
    for (auto &it : selected)
    {
        StrmDataListWidgetItem *item = dynamic_cast<StrmDataListWidgetItem *>(it);
        if (item != nullptr)
        {
            this->ui->selectedListWidget->takeItem(this->ui->selectedListWidget->row(item));
            this->ui->availableListWidget->addItem(item);

            qDebug() << "unselected data" << item->text();
        }
    }
}

void CommandTab::updateStrm(void)
{
    const int nitems = this->ui->selectedListWidget->count();
    QVector<uint8_t> ids;
    QVector<QString> field_names;

    for (int i = 0; i < nitems; i++)
    {
        StrmDataListWidgetItem *item = dynamic_cast<StrmDataListWidgetItem *>(this->ui->selectedListWidget->item(i));
        if (item != nullptr)
        {
            ids.append(item->getId());
            field_names.append(strm_data_infos[item->getId()].field_name);
        }
    }

    if (!this->cmd_mgr->setStrm(ids))
    {
        return;
    }
    this->plot_mgr->setCurveNames(field_names);
}

void CommandTab::toggleStrm(bool checked)
{
    if (!this->cmd_mgr->toggleStrm(checked))
    {
        bool old_state = this->ui->strmPushButton->blockSignals(true);
        this->ui->strmPushButton->setChecked(!checked);
        this->ui->strmPushButton->blockSignals(old_state);
    }
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

void CommandTab::reset(void)
{
    bool old_state;

    old_state = this->ui->strmPushButton->blockSignals(true);
    this->ui->strmPushButton->setChecked(false);
    this->ui->strmPushButton->blockSignals(old_state);

    old_state = this->ui->pwmPushButton->blockSignals(true);
    this->ui->pwmPushButton->setChecked(false);
    this->ui->pwmPushButton->blockSignals(old_state);

    old_state = this->ui->redLedPushButton->blockSignals(true);
    this->ui->redLedPushButton->setChecked(false);
    this->ui->redLedPushButton->blockSignals(old_state);

    old_state = this->ui->greenLedPushButton->blockSignals(true);
    this->ui->greenLedPushButton->setChecked(false);
    this->ui->greenLedPushButton->blockSignals(old_state);

    old_state = this->ui->blueLedPushButton->blockSignals(true);
    this->ui->blueLedPushButton->setChecked(false);
    this->ui->blueLedPushButton->blockSignals(old_state);
}

StrmDataListWidgetItem::StrmDataListWidgetItem(const QString &name, uint8_t id, QListWidget *parent, int type) :
    QListWidgetItem(name, parent, type),
    id(id)
{}

bool StrmDataListWidgetItem::operator<(const QListWidgetItem &other) const
{
    return this->id < dynamic_cast<const StrmDataListWidgetItem &>(other).id;
}

uint8_t StrmDataListWidgetItem::getId(void) const
{
    return this->id;
}
