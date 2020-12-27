#ifndef PREDICT_GESTURE_THREAD_H
#define PREDICT_GESTURE_THREAD_H

#include <QThread>

class PredictGestureThread : public QThread
{
    Q_OBJECT
public:
    PredictGestureThread();
};

#endif // PREDICT_GESTURE_THREAD_H
