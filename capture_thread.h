#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include "predict_gesture_thread.h"

#include "opencv2/videoio.hpp"

#include <QElapsedTimer>

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
    void startIntervalTimer();
    int getIntervalElapsedTime() const;

private:
    void recordGesture(const cv::Mat &frame);
    void introUI();

protected:
    void run() override;

public slots:
    void setDisplaying(const bool &display)
    {
        displaying = display;
    };

signals:
    void hiMessage();
    void howToUseInfo();
    void frameCaptured(cv::Mat *data);
    void finishedRecording();

private:
    std::shared_ptr<bool> running;
    bool recording;
    bool displaying;
    int cameraID;
    QMutex *displayedDataLock;
    cv::Mat frame;
    int gestureDuration;
    QElapsedTimer frameIntervalTimer;

    // for predicting thread
    PredictGestureThread *predictor;
    QMutex *predictingDataLock;
    QQueue<cv::Mat> *predictingFrames;
};

#endif // CAPTURETHREAD_H
