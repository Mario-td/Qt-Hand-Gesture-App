#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include "predict_gesture_thread.h"
#include "timer.h"
#include "utilities.h"

#include "opencv2/videoio.hpp"

#include <QElapsedTimer>

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    void setRecording(bool record)
    {
        startIntervalTimer();
        recording = record;
        timer.start();
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
    QMutex *displayFrameLock;
    int gestureDuration;
    QElapsedTimer frameIntervalTimer;
    Timer timer = Timer(5000, 32);
    cv::Mat frame;

    // for predicting thread
    PredictGestureThread *predictor;
    QMutex *predictingDataLock;
    QQueue<cv::Mat> *predictingFrames;
    QVector<cv::Mat> gestureSequenceFrames = QVector<cv::Mat>(Utilities::FRAMES_PER_SEQUENCE);
};

#endif // CAPTURETHREAD_H
