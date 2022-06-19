#ifndef PREDICT_GESTURE_THREAD_H
#define PREDICT_GESTURE_THREAD_H

#include "utilities.h"
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>

#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QDebug>

class PredictGestureThread : public QThread
{
    Q_OBJECT
public:
    PredictGestureThread(std::shared_ptr<bool> run, QQueue<cv::Mat> *frameVector, QMutex *lock);

private:
    void extractKeypoints();
    void dequeueSequenceFrame();
    void passThroughGestureModel();
    void putFrameInSharedMemory();

protected:
    void run() override;

signals:
    void finishedPrediction(const char *);
    void resetPrediction(bool);

private:

    // Deep Learning models for hand keypoints detection and gesture classification
    torch::jit::script::Module handKeypointModel;
    torch::jit::script::Module gestureClassificationModel;

    std::shared_ptr<bool> running;
    QQueue<cv::Mat> *predictingFrames;
    QMutex *predictingDataLock;

    bool extractingKeypoints;
    cv::Mat recordedFrame;
    int sequenceIdx;

    static constexpr size_t imageSizeBytes = Utilities::FRAME_WIDTH * Utilities::FRAME_HEIGHT * 3;
    boost::interprocess::mapped_region *region;
    uchar *imageBuff = nullptr;
    float *coordinatesBuff = nullptr;
    std::thread *thrd;
    boost::interprocess::named_semaphore *semaphore;

    std::vector<torch::jit::IValue> gestureClassificationModelInput;
    torch::Tensor gestureSequenceTensor = torch::zeros({1, Utilities::NUM_KEYPOINTS * 2, Utilities::FRAMES_PER_SEQUENCE});
};

#endif // PREDICT_GESTURE_THREAD_H
