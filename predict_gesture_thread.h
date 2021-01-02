#ifndef PREDICT_GESTURE_THREAD_H
#define PREDICT_GESTURE_THREAD_H

#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QDebug>

#include "opencv2/opencv.hpp"

#undef slots
#include "torch/script.h"
#define slots Q_SLOTS

#include "utilities.h"

class PredictGestureThread : public QThread
{
    Q_OBJECT
public:
    PredictGestureThread(bool &run, QQueue<cv::Mat> &frameVector, QMutex *lock);
    void ExtractKeypoints(cv::Mat &frame);

protected:
    void run() override;

private:
    // Deep Learning models for hand keypoints detection and gesture classification
    torch::jit::script::Module *handKeypointModel;
    torch::jit::script::Module *gestureClassificationModel;

    bool *running;
    QQueue<cv::Mat> *predictingFrames;
    QMutex *predictingDataLock;
};

#endif // PREDICT_GESTURE_THREAD_H
