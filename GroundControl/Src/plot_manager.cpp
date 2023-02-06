#include <QPen>
#include <qwt_scale_div.h>
#include "plot_manager.hpp"

PlotManager::PlotManager(QwtPlot *plot):
    plot(plot)
{
    /* Set axes. */
    this->buffer_size = 1000;
    this->plot->setAxisScale(QwtPlot::xBottom, 0, this->buffer_size);
    this->plot->setAxisAutoScale(QwtPlot::yLeft);

    /* Add grid. */
    this->grid = new QwtPlotGrid();
    this->grid->enableX(false);
    this->grid->enableY(false);
    this->grid->setMajorPen(PlotManager::MajorGridColor, 0.0, Qt::SolidLine);
    this->grid->setMinorPen(PlotManager::MinorGridColor, 0.0, Qt::SolidLine);
    this->grid->attach(this->plot);

    /* Add zoomer. */
    this->zoom = new QwtPlotZoomer(this->plot->canvas());
    this->zoom->setRubberBandPen(QPen(Qt::darkGray));

    /* Add legend. */
    this->legend = new QwtLegend();
    this->plot->insertLegend(this->legend);
}

PlotManager::~PlotManager() {
    delete this->grid;
    delete this->zoom;
    delete this->legend;
    for (int i = 0; i < this->curves.size(); i++) {
        delete this->curves[i];
    }
}

void PlotManager::showMajorGrid(bool checked) {
    this->grid->enableX(checked);
    this->grid->enableY(checked);
    this->plot->replot();
}

void PlotManager::showMinorGrid(bool checked) {
    this->grid->enableXMin(checked);
    this->grid->enableYMin(checked);
    this->plot->replot();
}

void PlotManager::setBufferSize(int size) {
    this->buffer_size = size;
    this->plot->setAxisScale(QwtPlot::xBottom, 0, this->buffer_size);
}

void PlotManager::setCurveNames(const QVector<QString> &names) {
    for (int i = 0; i < this->curves.size(); i++) {
        delete this->curves[i];
    }
    this->curves.clear();
    this->curve_data.clear();

    for (int i = 0; i < names.size(); i++) {
        QwtPlotCurve *c = new QwtPlotCurve(names[i]);

        c->setRenderHint(QwtPlotItem::RenderAntialiased, true);

        c->attach(this->plot);
        this->curves.append(c);
    }
    this->curve_data.resize(names.size());
}

void PlotManager::appendCurveData(const QVector<float> &data) {
    Q_ASSERT(this->curves.size() > 0);
    Q_ASSERT(this->curves.size() == data.size());

    if (this->curve_data[0].size() == this->buffer_size) {
        for (int i = 0; i < this->curves.size(); i++) {
            this->curve_data[i].remove(0);
        }
    }

    for (int i = 0; i < this->curves.size(); i++) {
        this->curve_data[i].append(data[i]);
        this->curves[i]->setSamples(this->curve_data[i]);
    }
    this->plot->replot();
}
