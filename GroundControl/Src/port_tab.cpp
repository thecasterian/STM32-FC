#include "port_tab.hpp"
#include "ui_port_tab.h"

PortTab::PortTab(PortManager *port_mgr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PortTab),
    port_mgr(port_mgr)
{
    this->ui->setupUi(this);

    this->fillPortComboBox();

    connect(this->ui->portSearchPushButton, &QPushButton::clicked, this, &PortTab::fillPortComboBox);
    connect(this->ui->portConnectPushButton, &QPushButton::clicked, this, &PortTab::toggleConnectPushButton);
}

PortTab::~PortTab() {
    delete this->ui;
}

void PortTab::fillPortComboBox(void) {
    this->ui->portComboBox->clear();

    const auto &port_list = QSerialPortInfo::availablePorts();
    for (const auto &port: port_list)
    {
        this->ui->portComboBox->addItem(port.portName());
    }
}

void PortTab::toggleConnectPushButton(void) {
    if (this->port_mgr->isOpen())
    {
        this->port_mgr->close();
    }
    else
    {
        QString port_name = this->ui->portComboBox->currentText().trimmed();
        QString buad_rate = this->ui->baudRateComboBox->currentText().trimmed();

        PortInfo info{port_name, buad_rate};

        if (!this->port_mgr->open(info)) {
            this->ui->portConnectPushButton->setChecked(false);
        }
    }
}
