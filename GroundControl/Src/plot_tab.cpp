#include "plot_tab.hpp"
#include "ui_plot_tab.h"
#include "moc_plot_tab.cpp"

PlotTab::PlotTab(PlotManager *plot_mgr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotTab),
    plot_mgr(plot_mgr)
{
    this->ui->setupUi(this);

    this->ui->bufferSizeLabel->setText(QString::number(this->ui->bufferSizeSlider->value()));

    connect(this->ui->showMajorGridCheckBox, &QCheckBox::toggled, this, &PlotTab::toggleShowMajorGridCheckBox);
    connect(this->ui->showMinorGridCheckBox, &QCheckBox::toggled, this, &PlotTab::toggleShowMinorGridCheckBox);
    connect(this->ui->bufferSizeSlider, &QSlider::valueChanged, this, &PlotTab::setBufferSize);
}

PlotTab::~PlotTab()
{
    delete this->ui;
}

void PlotTab::toggleShowMajorGridCheckBox(bool checked)
{
    this->plot_mgr->showMajorGrid(checked);

    this->ui->showMinorGridCheckBox->setEnabled(checked);
    if (!checked)
    {
        this->ui->showMinorGridCheckBox->setChecked(false);
    }
}

void PlotTab::toggleShowMinorGridCheckBox(bool checked)
{
    this->plot_mgr->showMinorGrid(checked);
}

void PlotTab::setBufferSize(int value)
{
    const int min_size = this->ui->bufferSizeSlider->minimum();
    const int max_size = this->ui->bufferSizeSlider->maximum();
    const int step_size = this->ui->bufferSizeSlider->tickInterval();

    for (int i = min_size; i <= max_size; i += step_size)
    {
        if (value <= i + step_size / 2)
        {
            this->ui->bufferSizeSlider->setValue(i);
            this->ui->bufferSizeLabel->setText(QString::number(i));

            this->plot_mgr->setBufferSize(i);
            break;
        }
    }
}
