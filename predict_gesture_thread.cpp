#include "predict_gesture_thread.h"

PredictGestureThread::PredictGestureThread(bool &run, QVector<cv::Mat> &frameVector, QMutex *lock):
    running(&run), predicted(false), predictingFrames(&frameVector), predictingDataLock(lock)
{
}

void PredictGestureThread::run()
{
    qDebug() << "predicting";
    while (*running) {
        // qDebug() << predictingFrames->size();
    }
}
