#include "capture_thread.h"

CaptureThread::CaptureThread(int camera, QMutex *lock):
    recording(false), displaying(true), cameraID(camera), displayedDataLock(lock)
{
    predictingFrames = new QQueue<cv::Mat>();
    predictingDataLock = new QMutex();
    running = std::make_shared<bool>(true);
    gestureDuration = 5000; // ms
}

void CaptureThread::startIntervalTimer()
{
    frameIntervalTimer.start();
}

int CaptureThread::getIntervalElapsedTime() const
{
    return frameIntervalTimer.elapsed();
}

void CaptureThread::run()
{
    // get the camera ready
    cv::VideoCapture cap(cameraID);
    cv::Mat tmpFrame;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    introUI();

    // time between frames and offset because of image processing
    int intervalOffset = 40; //ms
    int frameInterval = gestureDuration / Utilities::FRAMES_PER_SEQUENCE - intervalOffset;

    while (*running) {
        cap >> tmpFrame;
        if (tmpFrame.empty()) break;

        // frame used to predict the gesture
        if (recording) {
            // makes the gesture duration deterministic
            if (getIntervalElapsedTime() > frameInterval) {
                recordGesture(tmpFrame);
                startIntervalTimer();
            }
        }
        cvtColor(tmpFrame, tmpFrame, cv::COLOR_BGR2RGB);

        // frame used to display in the UI
        if (displaying) {
            displayedDataLock->lock();
            frame = tmpFrame;
            displayedDataLock->unlock();
            emit frameCaptured(&frame);
        }
    }
    cap.release();
    *running = false;
}

void CaptureThread::recordGesture(const cv::Mat &frame)
{
    static int sequenceFrameIdx = 0;
    predictingDataLock->lock();
    predictingFrames->enqueue(frame.clone());
    predictingDataLock->unlock();
    sequenceFrameIdx++;

    // resets the variables when the sequence finishes
    if (sequenceFrameIdx > Utilities::FRAMES_PER_SEQUENCE - 1) {
        emit finishedRecording();
        setRecording(false);
        setDisplaying(false);
        sequenceFrameIdx = 0;
    }
}

void CaptureThread::introUI()
{
    QThread::sleep(6);
    emit hiMessage();
    QThread::sleep(8);
    emit howToUseInfo();
}
