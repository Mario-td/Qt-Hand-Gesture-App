#ifndef TIMER_H
#define TIMER_H

#include <QElapsedTimer>

class Timer
{
public:
    Timer() = delete;
    Timer(uint duration, uint numberOfIntervals);

    void start();
    bool readyForNextInterval();
    void nextInterval();

private:
    uint durationPerInterval{};
    uint currentInteval{0};
    QElapsedTimer elapsedTimer{};
};
#endif // TIMER_H
