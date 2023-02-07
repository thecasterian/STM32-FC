#ifndef PLOT_TAB_HPP
#define PLOT_TAB_HPP

#include <QWidget>
#include "plot_manager.hpp"

namespace Ui
{
class PlotTab;
}

class PlotTab : public QWidget
{
    Q_OBJECT

public:
    explicit PlotTab(PlotManager *plot_mgr, QWidget *parent = nullptr);
    ~PlotTab();

private:
    Ui::PlotTab *ui;

    PlotManager *plot_mgr;

private slots:
    void toggleShowMajorGridCheckBox(bool checked);
    void toggleShowMinorGridCheckBox(bool checked);
};

#endif
