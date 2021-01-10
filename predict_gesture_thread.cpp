#include "predict_gesture_thread.h"

PredictGestureThread::PredictGestureThread(std::shared_ptr<bool> run, QQueue<cv::Mat> *frameVector,
                                           QMutex *lock):
    running(run), predictingFrames(frameVector), predictingDataLock(lock), extractingKeypoints(false),
    sequenceIdx(0)
{
    handKeypointModel = torch::jit::script::Module(torch::jit::load("./hand.pts", torch::kCUDA));
    gestureClassificationModel = torch::jit::script::Module(torch::jit::load("./model.pt"));
}

void PredictGestureThread::run()
{
    qDebug("predicting thread");

    while (*running) {

        // if it is not extracting keypoints from a frame then dequeue the next frame if it's available
        if (!extractingKeypoints) {
            DequeueSequenceFrame();
        } else {
            ExtractKeypoints();
        }
    }
}

void PredictGestureThread::DequeueSequenceFrame()
{
    // Dequeues a frame if the buffer is not empty
    predictingDataLock->lock();
    if (!predictingFrames->empty()) {
        recordedFrame = predictingFrames->front();
        extractingKeypoints = true;
        predictingFrames->dequeue();
    }
    predictingDataLock->unlock();
}

void PredictGestureThread::ExtractKeypoints()
{
    // Declares a vector to store the handkeypoint locations and performs the inference for each frame
    std::vector<std::map<float, cv::Point2f>> handKeypoints;
    std::vector<cv::Rect> handrect;
    handKeypoints = Utilities::pyramidinference(handKeypointModel, recordedFrame, handrect);

    // Assigns the coordinates of the keypoints to the tensor, for each time step
    for (int i = 0; i < Utilities::NUM_KEYPOINTS; i++) {
        if (!handKeypoints[i].empty()) {
            gestureSequenceTensor[0][2 * i][sequenceIdx] = handKeypoints[i].begin()->second.x;
            gestureSequenceTensor[0][2 * i + 1][sequenceIdx] = handKeypoints[i].begin()->second.y;
        }
    }
    sequenceIdx++;
    extractingKeypoints = false;

    // set to cero after completing a sequence
    if (sequenceIdx > Utilities::FRAMES_PER_SEQUENCE - 1) {
        sequenceIdx = 0;
        passThroughGestureModel();
    }
}

void PredictGestureThread::passThroughGestureModel()
{
    // Forwards the input through the sequence classification model
    gestureClassificationModelInput.push_back(gestureSequenceTensor);
    auto output = gestureClassificationModel.forward(gestureClassificationModelInput).toTensor();

    // signal with the predicted gesture
    emit finishedPrediction(Utilities::GESTURE_NAMES[output.argmax(1).item().toInt()]);

    QThread::sleep(4);

    gestureClassificationModelInput.clear();

    emit resetPrediction(true);
}
