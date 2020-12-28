#ifndef PREDICT_GESTURE_THREAD_H
#define PREDICT_GESTURE_THREAD_H

#include <QThread>
#include <QMutex>
#include <QVector>
#include <QDebug>

#include "opencv2/opencv.hpp"

class PredictGestureThread : public QThread
{
    Q_OBJECT
public:
    PredictGestureThread(QVector<cv::Mat> frameVector, QMutex *lock);

protected:
    void run() override;

signals:
    // void frame

private:
    bool predicted;
    QVector<cv::Mat> predictingFrames;
    QMutex *predictingDataLock;
};

#endif // PREDICT_GESTURE_THREAD_H
