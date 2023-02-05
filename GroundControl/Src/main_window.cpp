#include <QDebug>
#include "main_window.hpp"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    this->ui->setupUi(this);

    /* Port list. */
    this->fill_portComboBox();
    connect(this->ui->portSearchPushButton, &QPushButton::clicked, this, &MainWindow::on_portSearchPushButton_clicked);

    /* Port setting. */
    this->port_setting = new PortSetting(this);
    connect(this->ui->portSettingsPushButton, &QPushButton::clicked, this->port_setting, &PortSetting::show);

    /* Port connection. */
    this->serial_port = new QSerialPort(this);
    connect(this->ui->portConnectPushButton, &QPushButton::clicked, this,
            &MainWindow::on_portConnectPushButton_clicked);
    connect(this->serial_port, &QSerialPort::readyRead, this, &MainWindow::readData);

    /* Tabs. */
    this->ui->tabWidget->clear();

    this->command_tab = new CommandTab(this->serial_port);
    this->plotting_tab = new PlottingTab(this->ui->strmQwtPlot);

    this->ui->tabWidget->addTab(this->command_tab, "Command");
    this->ui->tabWidget->addTab(this->plotting_tab, "Plotting");
}

MainWindow::~MainWindow() {
    delete this->ui;
    delete this->port_setting;
    delete this->serial_port;
}

void MainWindow::fill_portComboBox(void) {
    this->ui->portComboBox->clear();

    auto port_list = QSerialPortInfo::availablePorts();
    for (auto &port: port_list) {
        this->ui->portComboBox->addItem(port.portName());
    }
}

void MainWindow::on_portSearchPushButton_clicked(void) {
    this->fill_portComboBox();
}

void MainWindow::on_portConnectPushButton_clicked(void) {
    if (this->serial_port->isOpen()) {
        this->serial_port->close();
        qDebug() << "port " << this->serial_port->portName() << " closed";
    } else {
        QString port_name = this->ui->portComboBox->currentText().trimmed();
        if (port_name.isEmpty()) {
            qWarning() << "port name is empty";
            this->ui->portConnectPushButton->setChecked(false);
            return;
        }

        QString buad_rate = this->port_setting->getBaudRate();
        bool baud_rate_ok = true;
        int buad_rate_int = buad_rate.toInt(&baud_rate_ok);
        if (!baud_rate_ok) {
            qWarning() << "baud rate " << buad_rate << " is not a number";
            this->ui->portConnectPushButton->setChecked(false);
            return;
        }

        this->serial_port->setPortName(port_name);
        this->serial_port->setBaudRate(buad_rate_int);

        if (this->serial_port->open(QIODevice::ReadWrite)) {
        } else {
            qWarning() << "port " << this->serial_port->portName() << " failed to open";
        }
    }
}

void MainWindow::readData(void) {
    QByteArray data = this->serial_port->readAll();
    qDebug() << "read data: " << data;
}
