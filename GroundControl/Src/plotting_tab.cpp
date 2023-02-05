#include "plotting_tab.hpp"
#include "ui_plotting_tab.h"

PlottingTab::PlottingTab(QwtPlot *plot, QWidget *parent):
    QWidget(parent),
    ui(new Ui::PlottingTab),
    plot(plot) {
    this->ui->setupUi(this);
}

PlottingTab::~PlottingTab() {
    delete this->ui;
}
