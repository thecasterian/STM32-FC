#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QtSerialPort>
#include "command_tab.hpp"
#include "port_setting.hpp"
#include "plotting_tab.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial_port;
    PortSetting *port_setting;
    CommandTab *command_tab;
    PlottingTab *plotting_tab;

    void fill_portComboBox(void);

private slots:
    void on_portSearchPushButton_clicked(void);
    void on_portConnectPushButton_clicked(void);
    void readData(void);
};

#endif
