#include "main_window.hpp"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);

    this->port_mgr = new PortManager();
    this->plot_mgr = new PlotManager(this->ui->strmQwtPlot, this->port_mgr);
    this->cmd_mgr = new CommandManager(this->port_mgr);

    /* Tabs. */
    this->ui->tabWidget->clear();

    this->port_tab = new PortTab(this->port_mgr);
    this->command_tab = new CommandTab(this->cmd_mgr, this->plot_mgr);
    this->plot_tab = new PlotTab(this->plot_mgr);

    this->ui->tabWidget->addTab(this->port_tab, "Port");
    this->ui->tabWidget->addTab(this->command_tab, "Command");
    this->ui->tabWidget->addTab(this->plot_tab, "Plot");

    /* Status bar. */
    err_label = new QLabel("");
    smp_label = new QLabel("0 samples/s");

    this->ui->statusbar->addWidget(err_label);
    this->ui->statusbar->addPermanentWidget(smp_label);

    this->smp_counter = new SampleCounter(this->port_mgr, this->smp_label);
}

MainWindow::~MainWindow()
{
    delete this->ui;

    delete this->port_mgr;
    delete this->plot_mgr;
    delete this->cmd_mgr;

    delete this->port_tab;
    delete this->command_tab;
    delete this->plot_tab;

    delete this->err_label;
    delete this->smp_label;

    delete this->smp_counter;
}
