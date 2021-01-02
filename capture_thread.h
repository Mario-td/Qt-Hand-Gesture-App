#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QMutex>
#include <QDebug> // to be removed
#include <QElapsedTimer> // to be removed
#include <QQueue>

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

#include "predict_gesture_thread.h" // to be removed and include utilities, after constructing the predictor object from mainwindow

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
    QQueue<cv::Mat> predictingFrames;
};

#endif // CAPTURETHREAD_H
