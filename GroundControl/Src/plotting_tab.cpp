#include <QPen>
#include "plotting_tab.hpp"
#include "ui_plotting_tab.h"

PlottingTab::PlottingTab(QwtPlot *plot, QWidget *parent):
    QWidget(parent),
    ui(new Ui::PlottingTab),
    plot(plot) {
    this->ui->setupUi(this);

    /* Add grid. */
    this->grid = new QwtPlotGrid();
    this->grid->enableX(false);
    this->grid->enableY(false);
    this->grid->setMajorPen(Qt::lightGray, 0.0, Qt::SolidLine);
    this->grid->setMinorPen(Qt::lightGray, 0.0, Qt::DotLine);
    this->grid->attach(this->plot);

    connect(this->ui->showMajorGridCheckBox, &QCheckBox::toggled, this, &PlottingTab::on_showMajorGridCheckBox_toggled);
    connect(this->ui->showMinorGridCheckBox, &QCheckBox::toggled, this, &PlottingTab::on_showMinorGridCheckBox_toggled);

    /* Add zoomer. */
    this->zoom = new QwtPlotZoomer(this->plot->canvas());
    this->zoom->setRubberBandPen(QPen(Qt::darkGray));
}

PlottingTab::~PlottingTab() {
    delete this->ui;
    delete this->grid;
    delete this->zoom;
}

void PlottingTab::on_showMajorGridCheckBox_toggled(bool checked) {
    this->ui->showMinorGridCheckBox->setEnabled(checked);
    if (!checked) {
        this->ui->showMinorGridCheckBox->setChecked(false);
    }

    this->grid->enableX(checked);
    this->grid->enableY(checked);
    this->plot->replot();
}

void PlottingTab::on_showMinorGridCheckBox_toggled(bool checked) {
    this->grid->enableXMin(checked);
    this->grid->enableYMin(checked);
    this->plot->replot();
}
