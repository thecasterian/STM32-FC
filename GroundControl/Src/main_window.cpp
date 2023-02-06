#include <QDebug>
#include "main_window.hpp"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);

    this->port_mgr = new PortManager();
    this->plot_mgr = new PlotManager(this->ui->strmQwtPlot);

    /* Tabs. */
    this->ui->tabWidget->clear();

    this->port_tab = new PortTab(this->port_mgr);
    this->command_tab = new CommandTab(this->port_mgr);
    this->plot_tab = new PlotTab(this->plot_mgr);

    this->ui->tabWidget->addTab(this->port_tab, "Port");
    this->ui->tabWidget->addTab(this->command_tab, "Command");
    this->ui->tabWidget->addTab(this->plot_tab, "Plot");
}

MainWindow::~MainWindow()
{
    delete this->ui;
    delete this->port_mgr;
    delete this->plot_mgr;
    delete this->port_tab;
    delete this->command_tab;
    delete this->plot_tab;
}
