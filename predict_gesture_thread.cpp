#include "predict_gesture_thread.h"

PredictGestureThread::PredictGestureThread(QVector<cv::Mat> frameVector, QMutex *lock):
    predictingFrames(frameVector), predictingDataLock(lock)
{
    predicted = false;
}

void PredictGestureThread::run()
{
    qDebug() << "predicting";
}
