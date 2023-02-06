#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QTimer>
#include "command_tab.hpp"
#include "plot_tab.hpp"
#include "port_tab.hpp"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    PortManager *port_mgr;
    PlotManager *plot_mgr;

    PortTab *port_tab;
    CommandTab *command_tab;
    PlotTab *plot_tab;
};

#endif
