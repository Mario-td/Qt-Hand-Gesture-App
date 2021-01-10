#include "capture_thread.h"

CaptureThread::CaptureThread(int camera, QMutex *lock):
    recording(false), displaying(true), cameraID(camera), displayedDataLock(lock)
{
    predictingFrames = new QQueue<cv::Mat>();
    predictingDataLock = new QMutex();
    running = std::make_shared<bool>(true);
}

void CaptureThread::run()
{
    qDebug("capturing thread");

    // get the camera ready
    cv::VideoCapture cap(cameraID);
    cv::Mat tmpFrame;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    QThread::sleep(6);
    emit cameraReady();
    QThread::sleep(8);
    emit howToUseInfo();

    while (*running) {
        cap >> tmpFrame;
        if (tmpFrame.empty()) break;

        // frame used to predict the gesture
        if (recording) {
            recordGesture(tmpFrame);
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
