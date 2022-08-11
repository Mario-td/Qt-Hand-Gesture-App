#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include "predict_gesture_thread.h"
#include "timer.h"
#include "utilities.h"

#include "opencv2/videoio.hpp"

#include <QElapsedTimer>

#define GESTURE_DURATION 5000// in ms

class WorkerThread : public QThread
{
    Q_OBJECT
    void run() override
    {
        int result = 2;
        std::string
        s("~/mediapipe/Simplified-hand-tracking-with-Mediapipe-CPP/run.sh");
        std::system(s.c_str());
        QThread::sleep(10);
        emit resultReady(result);
    }
public:
signals:
    void resultReady(const int &i);
};


class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    void setRecording(bool record)
    {
        startIntervalTimer();
        recording = record;
        worker.start();
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
    WorkerThread worker;

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
    QElapsedTimer frameIntervalTimer;
    Timer timer = Timer(GESTURE_DURATION, 32);
    cv::Mat frame;

    // for predicting thread
    PredictGestureThread *predictor;
    QMutex *predictingDataLock;
    QQueue<cv::Mat> *predictingFrames;
    QVector<cv::Mat> gestureSequenceFrames = QVector<cv::Mat>(Utilities::FRAMES_PER_SEQUENCE);
};

#endif // CAPTURETHREAD_H
