#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include "timer.h"
#include "utilities.h"
#include "shared_memory_writer.h"
#include "gesture_predictor.h"
#include "hand_detector_process_launcher.h"

#include "opencv2/videoio.hpp"

#include <QElapsedTimer>
#include <QMutex>
#include <QQueue>

#define GESTURE_DURATION 5000// in ms

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    ~CaptureThread();
    bool getRunning() const
    {
        return *running;
    };
    QMutex *getPredictingDataLock() const
    {
        return predictingDataLock;
    };
    void startIntervalTimer();
    int getIntervalElapsedTime() const;
    void setRecording(bool record);
    void predictGesture();
    SharedMemoryWriter shMemoryWriter{};
    GesturePredictor gesturePredictor{};
    HandDetectorProcessLauncher worker{};

private:
    cv::VideoCapture *cap;
    void recordGesture(const cv::Mat &frame);

protected:
    void run() override;

public:
signals:
    void resultReady(const int &i);

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
    Timer timer = Timer(GESTURE_DURATION, Utilities::FRAMES_PER_SEQUENCE);
    cv::Mat frame;

    // for predicting thread
    QMutex *predictingDataLock;
};

#endif // CAPTURETHREAD_H
