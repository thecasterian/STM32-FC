#ifndef PLOT_MANAGER_HPP
#define PLOT_MANAGER_HPP

#include <QObject>
#include <qwt_legend.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include "port_manager.hpp"

class PlotManager : public QObject
{
    Q_OBJECT

public:
    explicit PlotManager(QwtPlot *plot, PortManager *port_mgr);
    ~PlotManager();

    void showMajorGrid(bool show);
    void showMinorGrid(bool show);

    void setBufferSize(int size);
    void setCurveNames(const QVector<QString> &names);
    void appendCurveData(const QVector<float> &data);

private:
    QwtPlot *plot;
    PortManager *port_mgr;

    QwtPlotGrid *grid;
    QwtPlotZoomer *zoom;
    QwtLegend *legend;

    QVector<QwtPlotCurve *> curves;
    QVector<QVector<float>> curve_data;
    int buffer_size;

private slots:
    void onZoomed(const QRectF &rect);
    void receiveStrm(uint8_t len, const uint8_t *dat);
};

#endif
