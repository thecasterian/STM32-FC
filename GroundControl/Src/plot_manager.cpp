#include <QDebug>
#include <QPen>
#include <qwt_scale_div.h>
#include "plot_manager.hpp"
#include "moc_plot_manager.cpp"

PlotManager::PlotManager(QwtPlot *plot, PortManager *port_mgr) :
    plot(plot),
    port_mgr(port_mgr)
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

    connect(this->zoom, &QwtPlotZoomer::zoomed, this, &PlotManager::onZoomed);
    connect(this->port_mgr, &PortManager::strmReceived, this, &PlotManager::receiveStrm);
}

PlotManager::~PlotManager()
{
    delete this->grid;
    // delete this->zoom;
    delete this->legend;
    for (int i = 0; i < this->curves.size(); i++)
    {
        delete this->curves[i];
    }
}

void PlotManager::showMajorGrid(bool checked)
{
    this->grid->enableX(checked);
    this->grid->enableY(checked);
    this->plot->replot();
}

void PlotManager::showMinorGrid(bool checked)
{
    this->grid->enableXMin(checked);
    this->grid->enableYMin(checked);
    this->plot->replot();
}

void PlotManager::setBufferSize(int size)
{
    this->buffer_size = size;
    this->plot->setAxisScale(QwtPlot::xBottom, 0, this->buffer_size);
    this->plot->replot();
}

void PlotManager::setCurveNames(const QVector<QString> &names)
{
    for (int i = 0; i < this->curves.size(); i++)
    {
        delete this->curves[i];
    }
    this->curves.clear();
    this->curve_data.clear();

    for (int i = 0; i < names.size(); i++)
    {
        QwtPlotCurve *c = new QwtPlotCurve(names[i]);

        c->setRenderHint(QwtPlotItem::RenderAntialiased, true);

        c->attach(this->plot);
        this->curves.append(c);
    }
    this->curve_data.resize(names.size());

    this->plot->replot();
}

void PlotManager::appendCurveData(const QVector<float> &data)
{
    if (this->curves.size() == 0)
    {
        qWarning() << "no curves to append data to";
        return;
    }
    if (this->curves.size() != data.size())
    {
        qWarning() << "number of curves and data do not match";
        return;
    }

    if (this->curve_data[0].size() >= this->buffer_size)
    {
        for (int i = 0; i < this->curves.size(); i++)
        {
            this->curve_data[i].remove(0, this->curve_data[i].size() - this->buffer_size + 1);
        }
    }

    for (int i = 0; i < this->curves.size(); i++)
    {
        this->curve_data[i].append(data[i]);
        this->curves[i]->setSamples(this->curve_data[i]);
    }
    this->plot->replot();
}

void PlotManager::onZoomed(const QRectF &rect)
{
    if (this->zoom->zoomRectIndex() == 0)
    {
        this->plot->setAxisAutoScale(QwtPlot::yLeft);
        this->zoom->setZoomBase(true);
    }
}

void PlotManager::receiveStrm(uint8_t len, const uint8_t *dat)
{
    QVector<float> dat_float;
    float x;

    for (int i = 0U; i < len / 4; i++)
    {
        memcpy(&x, &dat[i * 4], 4);
        dat_float.append(x);
    }
    this->appendCurveData(dat_float);
}
