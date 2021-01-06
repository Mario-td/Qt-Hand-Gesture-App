#include "capture_thread.h"

CaptureThread::CaptureThread(int camera, QMutex *lock):
    recording(false), cameraID(camera), displayedDataLock(lock)
{
    predictingFrames = new QQueue<cv::Mat>();
    predictingDataLock = new QMutex();
    running = std::make_shared<bool>();
}

void CaptureThread::run()
{
    qDebug("capturing thread");

    *running = true;

    cv::VideoCapture cap(cameraID);
    cv::Mat tmpFrame;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    while (*running) {
        cap >> tmpFrame;
        if (tmpFrame.empty()) break;

        // frame used to predict the gesture
        if (recording) {
            recordGesture(tmpFrame);
        }
        cvtColor(tmpFrame, tmpFrame, cv::COLOR_BGR2RGB);

        // frame used to display in the UI
        displayedDataLock->lock();
        frame = tmpFrame;
        displayedDataLock->unlock();
        emit frameCaptured(&frame);
    }
    cap.release();
    *running = false;
}

void CaptureThread::recordGesture(const cv::Mat &frame)
{
    // starts the timer when the first frame is enqueued
    if (timerFlag) {
        timerFlag = false;
        timer.start();
    }

    static int sequenceFrameIdx = 0;
    predictingDataLock->lock();
    predictingFrames->enqueue(frame.clone());
    predictingDataLock->unlock();
    sequenceFrameIdx++;

    // resets the variables when the sequence finishes
    if (sequenceFrameIdx > Utilities::FRAMES_PER_SEQUENCE - 1) {
        *elapsedTime = timer.elapsed();
        emit finishedRecording(elapsedTime);
        setRecording(false);
        sequenceFrameIdx = 0;
        timerFlag = true;
    }
}


