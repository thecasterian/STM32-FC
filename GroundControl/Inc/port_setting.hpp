#ifndef PORT_SETTING_HPP
#define PORT_SETTING_HPP

#include <QDialog>

namespace Ui {
class PortSettingDialog;
}

class PortSetting: public QDialog {
public:
    explicit PortSetting(QWidget *parent = nullptr);
    ~PortSetting();

    QString getBaudRate(void) const;

private:
    Ui::PortSettingDialog *ui;

private slots:
    void on_baudRateComboBox_currentTextChanged(const QString &arg1);
};

#endif
