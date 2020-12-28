#include <QElapsedTimer>

#include "capture_thread.h"

CaptureThread::CaptureThread(int camera, QMutex *lock):
    cameraID(camera), displayedDataLock(lock)
{
    recording = false;
    predictingDataLock = new QMutex();
}

void CaptureThread::run()
{
    running = true;

    cv::VideoCapture cap(cameraID);
    cv::Mat tmpFrame;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    predictor = new PredictGestureThread(predictingFrames, predictingDataLock);
    predictor->start();

    int seqFrameIdx = 0;

    QElapsedTimer timer;
    bool timerFlag = true;

    while (running) {
        cap >> tmpFrame;
        if (tmpFrame.empty()) break;
        if (recording) {
            if (timerFlag) {
                timerFlag = false;
                timer.start();
            }
            // TODO: implementing a method
            predictingDataLock->lock();
            predictingFrames.push_back(tmpFrame);
            predictingDataLock->unlock();
            seqFrameIdx++;
            if (seqFrameIdx > 31) {
                int elapsed_ms = timer.elapsed();
                qDebug() << elapsed_ms;
                setRecording(false);
                seqFrameIdx = 0;
                predictingFrames.clear();
                timerFlag = true;
            }
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
