#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QMutex>

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    ~CaptureThread();
    void setRunning(bool run)
    {
        running = run;
    };

protected:
    void run() override;

signals:
    void frameCaptured(cv::Mat *data);

private:
    bool running;
    int cameraID;
    QMutex *dataLock;
    cv::Mat frame;
};

#endif // CAPTURETHREAD_H
