#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QMutex>
#include <QDebug> // to be removed
#include <QElapsedTimer> // to be removed
#include <QVector>

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

#include "predict_gesture_thread.h"

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    void setRecording(bool record)
    {
        recording = record;
    };
    void recordGesture(const cv::Mat &frame);

protected:
    void run() override;

signals:
    void frameCaptured(cv::Mat *data);

private:
    bool running;
    bool recording;
    int cameraID;
    QMutex *displayedDataLock;
    cv::Mat frame;

    // for debugging
    QElapsedTimer timer;
    bool timerFlag = true;

    // for predicting thread
    PredictGestureThread *predictor;
    QMutex *predictingDataLock;
    QVector<cv::Mat> predictingFrames;
};

#endif // CAPTURETHREAD_H
