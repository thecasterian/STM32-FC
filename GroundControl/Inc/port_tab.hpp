#ifndef PORT_TAB_HPP
#define PORT_TAB_HPP

#include <QWidget>
#include "port_manager.hpp"

namespace Ui
{
class PortTab;
}

class PortTab : public QWidget
{
    Q_OBJECT

public:
    explicit PortTab(PortManager *port_mgr, QWidget *parent = nullptr);
    ~PortTab();

private:
    Ui::PortTab *ui;

    PortManager *port_mgr;

private slots:
    void fillPortComboBox(void);
    void toggleConnectPushButton(void);
};


#endif
