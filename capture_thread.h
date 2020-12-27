#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QMutex>
#include <QDebug>

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    ~CaptureThread();
    void setRecording(bool record)
    {
        recording = record;
    };

protected:
    void run() override;

signals:
    void frameCaptured(cv::Mat *data);

private:
    bool running;
    bool recording;
    int cameraID;
    QMutex *dataLock;
    cv::Mat frame;
};

#endif // CAPTURETHREAD_H
