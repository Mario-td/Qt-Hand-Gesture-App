#include "capture_thread.h"

CaptureThread::CaptureThread(int camera, QMutex *lock):
    running(false), recording(false), cameraID(camera), displayedDataLock(lock)
{
    predictingDataLock = new QMutex();
}

void CaptureThread::run()
{
    running = true;

    cv::VideoCapture cap(cameraID);
    cv::Mat tmpFrame;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    predictor = new PredictGestureThread(running, predictingFrames, predictingDataLock);
    predictor->start();

    while (running) {
        cap >> tmpFrame;
        if (tmpFrame.empty()) break;
        if (recording) {
            recordGesture(tmpFrame);
        }
        cvtColor(tmpFrame, tmpFrame, cv::COLOR_BGR2RGB);
        displayedDataLock->lock();
        frame = tmpFrame;
        displayedDataLock->unlock();
        emit frameCaptured(&frame);
    }
    cap.release();
    running = false;
}

void CaptureThread::recordGesture(const cv::Mat &frame)
{
    static int sequenceFrameIdx = 0;

    if (timerFlag) {
        timerFlag = false;
        timer.start();
    }

    predictingDataLock->lock();
    predictingFrames.push_back(frame);
    qDebug() << "frame saved " << predictingFrames.size();
    predictingDataLock->unlock();
    sequenceFrameIdx++;

    if (sequenceFrameIdx > 31) {
        int elapsed_ms = timer.elapsed();
        qDebug() << elapsed_ms;
        setRecording(false);
        sequenceFrameIdx = 0;
        predictingFrames.clear();
        timerFlag = true;
    }
}
