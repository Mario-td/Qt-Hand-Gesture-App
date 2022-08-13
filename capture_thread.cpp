#include "capture_thread.h"

CaptureThread::CaptureThread(int camera, QMutex *lock):
    recording(false), displaying(false), cameraID(camera),
    displayFrameLock(lock)
{
    predictingDataLock = new QMutex();
    running = std::make_shared<bool>(true);
}

CaptureThread::~CaptureThread()
{
    worker.killHandDetectorProccess();
    worker.exit();
    cap->release();
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
    cap = new cv::VideoCapture(cv::CAP_V4L2);
    cv::Mat tmpFrame;

    cap->set(cv::CAP_PROP_FRAME_WIDTH, Utilities::FRAME_WIDTH);
    cap->set(cv::CAP_PROP_FRAME_HEIGHT, Utilities::FRAME_HEIGHT);

    while (*running) {
        *cap >> tmpFrame;
        if (tmpFrame.empty()) break;
        cv::flip(tmpFrame, tmpFrame, 1);

        if (recording) {
            if (timer.readyForNextInterval()) {
                recordGesture(tmpFrame);
                timer.nextInterval();
                std::cout << timer.elapsedTimer.elapsed() << std::endl;
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

    *running = false;
}

void CaptureThread::recordGesture(const cv::Mat &frame)
{
    static int sequenceFrameIdx = 0;
    predictingDataLock->lock();
    shMemoryWriter.writeFrameToMemory(frame, sequenceFrameIdx++);
    predictingDataLock->unlock();

    // resets the variables when the sequence finishes
    if (sequenceFrameIdx > Utilities::FRAMES_PER_SEQUENCE - 1) {
        predictGesture();
        sequenceFrameIdx = 0;
    }
}

void CaptureThread::predictGesture()
{
    emit finishedRecording();
    while (!worker.isFinished());
    emit resultReady(gesturePredictor.runModel()); ;
    setRecording(false);
    setDisplaying(false);
}

void CaptureThread::setRecording(bool record)
{
    startIntervalTimer();
    recording = record;
    worker.start();
    timer.start();
};
