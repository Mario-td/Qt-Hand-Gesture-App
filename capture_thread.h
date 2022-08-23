#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include "timer.h"
#include "constants.h"
#include "shared_memory_writer.h"
#include "gesture_predictor.h"
#include "hand_detector_process_launcher.h"

#include "opencv2/videoio.hpp"
#include "opencv2/opencv.hpp"

#include <QMutex>

#define GESTURE_DURATION 3200// in ms

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(QMutex *lock);
    ~CaptureThread();
    bool running = true;
    void setRecording(bool record);

private:
    cv::VideoCapture cap;
    void recordGesture(const cv::Mat &frame);
    void predictGesture();

protected:
    void run() override;

public slots:
    void setDisplaying(const bool &display)
    {
        displaying = display;
    };

signals:
    void resultReady(const int &);
    void frameCaptured(cv::Mat *data);

private:
    bool recording = false;
    bool displaying = false;
    Timer timer = Timer(GESTURE_DURATION, FRAMES_PER_SEQUENCE);
    cv::Mat frame{};
    SharedMemoryWriter shMemoryWriter{};
    GesturePredictor gesturePredictor{};
    HandDetectorProcessLauncher parallelProcessLauncher{};
    QMutex *predictingGestureLock = new QMutex();
    QMutex *displayFrameLock;
};

#endif // CAPTURETHREAD_H
