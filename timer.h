#ifndef TIMER_H
#define TIMER_H

#include <QElapsedTimer>

class Timer
{
private:
    uint durationPerInterval;
    uint currentInteval = 0;
    QElapsedTimer elapsedTimer;
public:
    Timer() = delete;
    Timer(uint duration, uint numberOfIntervals);

    void start();
    bool readyForNextInterval();
    void nextInterval();

};
#endif // TIMER_H
