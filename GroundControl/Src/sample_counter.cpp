#include "sample_counter.hpp"
#include "moc_sample_counter.cpp"

SampleCounter::SampleCounter(PortManager *port_mgr, QLabel *label) :
    port_mgr(port_mgr),
    label(label),
    cnt(0),
    timer(this)
{
    connect(&this->timer, &QTimer::timeout, this, &SampleCounter::onTimeout);
    this->timer.start(1000);

    connect(this->port_mgr, &PortManager::strmReceived, this, &SampleCounter::incCounter);
}

void SampleCounter::incCounter(uint8_t len, const uint8_t *dat)
{
    Q_UNUSED(len);
    Q_UNUSED(dat);

    this->cnt++;
}

void SampleCounter::onTimeout(void)
{
    this->label->setText(QString::number(this->cnt) + " samples/s");
    this->cnt = 0;

    this->timer.start(1000);
}
