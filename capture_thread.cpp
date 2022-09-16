#include "capture_thread.h"

CaptureThread::CaptureThread(QMutex *lock):
    displayFrameLock(lock)
{
}

CaptureThread::~CaptureThread()
{
    parallelProcessLauncher.killHandDetectorProccess();
    parallelProcessLauncher.exit();
    cap.release();
}

void CaptureThread::run()
{
    // get the camera ready
    cap = cv::VideoCapture(cv::CAP_V4L2);
    cv::Mat tmpFrame;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

    while (running) {
        cap >> tmpFrame;
        if (tmpFrame.empty()) break;
        cv::flip(tmpFrame, tmpFrame, 1);

        if (recording) {
            if (timer.readyForNextInterval()) {
                recordGesture(tmpFrame);
                timer.nextInterval();
            }
        }

        // frame used to display in the UI
        if (displaying) {
            cvtColor(tmpFrame, tmpFrame, cv::COLOR_BGR2RGB);
            displayFrameLock->lock();
            frame = tmpFrame;
            displayFrameLock->unlock();
            emit frameCaptured(&frame);
        }
    }
}

void CaptureThread::recordGesture(const cv::Mat &frame)
{
    static int sequenceFrameIdx = 0;
    predictingGestureLock->lock();
    shMemoryWriter.writeFrameToMemory(frame, sequenceFrameIdx++);
    predictingGestureLock->unlock();

    // resets the variables when the sequence finishes
    if (sequenceFrameIdx > FRAMES_PER_SEQUENCE - 1) {
        predictGesture();
        sequenceFrameIdx = 0;
    }
}

void CaptureThread::predictGesture()
{
    while (!parallelProcessLauncher.isFinished())
        ;
    emit resultReady(gesturePredictor.runModel()); ;
    setRecording(false);
    setDisplaying(false);
}

void CaptureThread::setRecording(bool record)
{
    recording = record;
    if (record) {
        parallelProcessLauncher.start();
        timer.start();
    }
};
