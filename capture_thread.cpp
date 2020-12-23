#include "utilities.h"
#include "capture_thread.h"

CaptureThread::CaptureThread(int camera, QMutex *lock):
    cameraID(camera), dataLock(lock)
{
}

CaptureThread::~CaptureThread()
{
}

void CaptureThread::run()
{
    running = true;

    cv::VideoCapture cap(cameraID);
    cv::Mat tmpFrame;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    while (running) {
        cap >> tmpFrame;
        if (tmpFrame.empty()) break;

        cvtColor(tmpFrame, tmpFrame, cv::COLOR_BGR2RGB);
        dataLock->lock();
        frame = tmpFrame;
        dataLock->unlock();
        emit frameCaptured(&frame);
    }
    cap.release();
    running = false;
}
