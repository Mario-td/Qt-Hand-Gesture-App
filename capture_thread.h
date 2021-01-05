#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include "predict_gesture_thread.h"

#include "opencv2/videoio.hpp"

#include <QElapsedTimer> // to be removed

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    void setRecording(bool record)
    {
        recording = record;
    };
    std::shared_ptr<bool> getRunning() const
    {
        return running;
    };
    QQueue<cv::Mat> *getPredictingFrames() const
    {
        return predictingFrames;
    };
    QMutex *getPredictingDataLock() const
    {
        return predictingDataLock;
    };
    void recordGesture(const cv::Mat &frame);

protected:
    void run() override;

signals:
    void frameCaptured(cv::Mat *data);
    void finishedRecording(int *elapsedTime);

private:
    std::shared_ptr<bool> running;
    bool recording;
    int cameraID;
    QMutex *displayedDataLock;
    cv::Mat frame;

    // for debugging
    QElapsedTimer timer;
    bool timerFlag = true;
    int *elapsedTime;

    // for predicting thread
    PredictGestureThread *predictor;
    QMutex *predictingDataLock;
    QQueue<cv::Mat> *predictingFrames;
};

#endif // CAPTURETHREAD_H
