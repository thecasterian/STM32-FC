#ifndef PLOT_MANAGER_HPP
#define PLOT_MANAGER_HPP

#include <qwt_legend.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>

class PlotManager {
public:
    explicit PlotManager(QwtPlot *plot);
    ~PlotManager();

    void showMajorGrid(bool show);
    void showMinorGrid(bool show);

    void setBufferSize(int size);
    void setCurveNames(const QVector<QString> &names);
    void appendCurveData(const QVector<float> &data);

private:
    static constexpr QColor MajorGridColor{192, 192, 192};
    static constexpr QColor MinorGridColor{232, 232, 232};

    QwtPlot *plot;
    QwtPlotGrid *grid;
    QwtPlotZoomer *zoom;
    QwtLegend *legend;

    QVector<QwtPlotCurve *> curves;
    QVector<QVector<float>> curve_data;
    int buffer_size;
};

#endif
