#include "port_setting.hpp"
#include "ui_port_setting.h"

PortSetting::PortSetting(QWidget *parent):
    QDialog(parent),
    ui(new Ui::PortSettingDialog) {
    this->setWindowTitle(QStringLiteral("Port Setting"));
    this->ui->setupUi(this);
}

PortSetting::~PortSetting() {
    delete this->ui;
}

QString PortSetting::getBaudRate(void) const {
    return this->ui->baudRateComboBox->currentText();
}
