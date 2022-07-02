#include "timer.h"

Timer::Timer(uint duration, uint numberOfIntervals)
{
    durationPerInterval = duration / numberOfIntervals;
}

void Timer::start()
{
    currentInteval = 0;
    elapsedTimer.start();
}

bool Timer::readyForNextInterval()
{
    return durationPerInterval * currentInteval < elapsedTimer.elapsed();
}

void Timer::nextInterval()
{
    ++currentInteval;
}

