#ifndef PLOTTING_TAB_HPP
#define PLOTTING_TAB_HPP

#include <QWidget>
#include "qwt_plot.h"

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
};

#endif
