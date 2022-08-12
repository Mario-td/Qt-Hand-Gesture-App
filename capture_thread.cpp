#include "capture_thread.h"

CaptureThread::CaptureThread(int camera, QMutex *lock):
    recording(false), displaying(false), cameraID(camera),
    displayFrameLock(lock)
{
    predictingFrames = new QQueue<cv::Mat>();
    predictingDataLock = new QMutex();
    running = std::make_shared<bool>(true);
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
    cv::VideoCapture cap(cv::CAP_V4L2);
    cv::Mat tmpFrame;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, Utilities::FRAME_WIDTH);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, Utilities::FRAME_HEIGHT);

    while (*running) {
        cap >> tmpFrame;
        if (tmpFrame.empty()) break;

        if (recording) {
            if (timer.readyForNextInterval()) {
                recordGesture(tmpFrame);
                timer.nextInterval();
                std::cout << timer.elapsedTimer.elapsed() << std::endl;
            }
        }
        cvtColor(tmpFrame, tmpFrame, cv::COLOR_BGR2RGB);

        // frame used to display in the UI
        if (displaying) {
            displayFrameLock->lock();
            frame = tmpFrame;
            displayFrameLock->unlock();
            emit frameCaptured(&frame);
        }
    }

    cap.release();
    *running = false;
}

void CaptureThread::recordGesture(const cv::Mat &frame)
{
    static int sequenceFrameIdx = 0;
    gestureSequenceFrames[sequenceFrameIdx] = frame.clone();
    predictingDataLock->lock();
    predictingFrames->enqueue(frame.clone());
    predictingDataLock->unlock();
    sequenceFrameIdx++;

    if (sequenceFrameIdx == 1)
        shMemoryWriter.writeFrameToMemory(frame, 0);

    // resets the variables when the sequence finishes
    if (sequenceFrameIdx > Utilities::FRAMES_PER_SEQUENCE - 1) {
        emit finishedRecording();
        setRecording(false);
        setDisplaying(false);
        sequenceFrameIdx = 0;
    }
}
