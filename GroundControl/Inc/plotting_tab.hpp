#ifndef PLOTTING_TAB_HPP
#define PLOTTING_TAB_HPP

#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>

namespace Ui {
class PlottingTab;
}

class PlottingTab : public QWidget {
public:
    explicit PlottingTab(QwtPlot *plot, QWidget *parent = nullptr);
    ~PlottingTab();

private:
    Ui::PlottingTab *ui;
    QwtPlot *plot;
    QwtPlotGrid *grid;
    QwtPlotZoomer *zoom;

private slots:
    void on_showMajorGridCheckBox_toggled(bool checked);
    void on_showMinorGridCheckBox_toggled(bool checked);
};

#endif
