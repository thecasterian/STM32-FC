#include "plot_tab.hpp"
#include "ui_plot_tab.h"
#include "moc_plot_tab.cpp"

PlotTab::PlotTab(PlotManager *plot_mgr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotTab),
    plot_mgr(plot_mgr)
{
    this->ui->setupUi(this);

    connect(this->ui->showMajorGridCheckBox, &QCheckBox::toggled, this, &PlotTab::toggleShowMajorGridCheckBox);
    connect(this->ui->showMinorGridCheckBox, &QCheckBox::toggled, this, &PlotTab::toggleShowMinorGridCheckBox);
}

PlotTab::~PlotTab()
{
    delete this->ui;
}

void PlotTab::toggleShowMajorGridCheckBox(bool checked)
{
    this->plot_mgr->showMajorGrid(checked);

    this->ui->showMinorGridCheckBox->setEnabled(checked);
    if (!checked) {
        this->ui->showMinorGridCheckBox->setChecked(false);
    }
}

void PlotTab::toggleShowMinorGridCheckBox(bool checked)
{
    this->plot_mgr->showMinorGrid(checked);
}
