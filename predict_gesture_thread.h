#ifndef PREDICT_GESTURE_THREAD_H
#define PREDICT_GESTURE_THREAD_H

#include "utilities.h"

#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QDebug>

class PredictGestureThread : public QThread
{
    Q_OBJECT
public:
    PredictGestureThread(std::shared_ptr<bool> run, QQueue<cv::Mat> *frameVector, QMutex *lock);
    void ExtractKeypoints();
    void DequeueSequenceFrame();
    void passThroughGestureModel();

protected:
    void run() override;

signals:
    void finishedPrediction(const char *);

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

    std::vector<torch::jit::IValue> gestureClassificationModelInput;
    torch::Tensor gestureSequenceTensor = torch::zeros({1, Utilities::NUM_KEYPOINTS * 2, Utilities::FRAMES_PER_SEQUENCE});
};

#endif // PREDICT_GESTURE_THREAD_H
