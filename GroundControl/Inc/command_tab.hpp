#ifndef COMMAND_TAB_HPP
#define COMMAND_TAB_HPP

#include <QWidget>
#include <QListWidget>
#include "command_manager.hpp"
#include "plot_manager.hpp"

namespace Ui
{
class CommandTab;
}

class StrmDataListWidgetItem;

class CommandTab : public QWidget
{
    Q_OBJECT

public:
    explicit CommandTab(CommandManager *cmd_mgr, PlotManager *plot_mgr, QWidget *parent = nullptr);
    ~CommandTab();

private:
    Ui::CommandTab *ui;

    CommandManager *cmd_mgr;
    PlotManager *plot_mgr;
    QVector<StrmDataListWidgetItem *> strm_data_list_items;

private slots:
    void moveToSelected(void);
    void moveToAvailable(void);

    void updateStrm(void);
    void toggleStrm(bool checked);

    void setFrontLeftEdit(int value);
    void setFrontRightEdit(int value);
    void setRearLeftEdit(int value);
    void setRearRightEdit(int value);

    void setFrontLeftSlider(const QString &value);
    void setFrontRightSlider(const QString &value);
    void setRearLeftSlider(const QString &value);
    void setRearRightSlider(const QString &value);

    void updateThrottle(void);
    void togglePwm(bool checked);

    void toggleRedLed(bool checked);
    void toggleGreenLed(bool checked);
    void toggleBlueLed(bool checked);

    void reset(void);
};

class StrmDataListWidgetItem : public QListWidgetItem
{
public:
    StrmDataListWidgetItem(const QString &name, uint8_t id, QListWidget *parent = nullptr, int type = UserType);

    virtual bool operator<(const QListWidgetItem &other) const;

    uint8_t getId(void) const;

private:
    uint8_t id;
};

#endif
