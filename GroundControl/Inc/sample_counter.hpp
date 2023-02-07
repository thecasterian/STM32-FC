#ifndef SAMPLE_COUNTER_HPP
#define SAMPLE_COUNTER_HPP

#include <QLabel>
#include <QObject>
#include <QTimer>
#include <stdint.h>
#include "port_manager.hpp"

class SampleCounter : public QObject
{
    Q_OBJECT

public:
    explicit SampleCounter(PortManager *port_mgr, QLabel *label);

private:
    PortManager *port_mgr;
    QLabel *label;
    int cnt;

    QTimer timer;

private slots:
    void incCounter(uint8_t len, const uint8_t *dat);
    void onTimeout(void);
};

#endif
