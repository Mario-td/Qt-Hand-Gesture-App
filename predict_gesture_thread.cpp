#include "predict_gesture_thread.h"

PredictGestureThread::PredictGestureThread(bool &run, bool &predict, QQueue<cv::Mat> &frameVector,
                                           QMutex *lock):
    running(&run), predicted(&predict), predictingFrames(&frameVector), predictingDataLock(lock)
{
}

void PredictGestureThread::run()
{
    // Loads the model for hand keypoints detection and gesture classification
    torch::jit::script::Module handModel = torch::jit::load("./hand.pts", torch::kCUDA);

    torch::jit::script::Module gestureModel = torch::jit::load("./model.pt");

    std::vector<torch::jit::IValue> inputs;
    torch::Tensor inputTensor = torch::zeros({1, Utilities::KEYPOINTS * 2, Utilities::FRAMES_PER_SEQUENCE});

    qDebug() << "predicting";

    bool extractingKeypoints = false;

    cv::Mat recordedFrame;

    int sequenceIdx = 0;

    while (*running) {

        // if it is not extracting keypoints from a frame then dequeue the next frame if it's available
        if (!extractingKeypoints) {
            predictingDataLock->lock();
            if (!predictingFrames->empty()) {
                recordedFrame = predictingFrames->front();
                extractingKeypoints = true;
                predictingFrames->dequeue();
            }
            predictingDataLock->unlock();
        } else {
            // TODO implement a method
            std::vector<std::map<float, cv::Point2f>> handKeypoints;
            std::vector<cv::Rect> handrect;
            handKeypoints = Utilities::pyramidinference(handModel, recordedFrame, handrect);

            // Assigns the coordinates of the keypoints to the tensor, for each time step
            for (int i = 0, n = handKeypoints.size(); i < n; i++) {
                if (!handKeypoints[i].empty()) {
                    inputTensor[0][2 * i][sequenceIdx] = handKeypoints[i].begin()->second.x;
                    inputTensor[0][2 * i + 1][sequenceIdx] = handKeypoints[i].begin()->second.y;
                }
            }
            sequenceIdx++;
            extractingKeypoints = false;

            qDebug() << "extracted keypoints from frame " << sequenceIdx << ", input tensor: ";
            // std::cout << inputTensor;

            // set to cero after completing a sequence
            if (sequenceIdx > Utilities::FRAMES_PER_SEQUENCE - 1) {
                qDebug() << "Finished with the sequence";

                sequenceIdx = 0;

                // Forwards the input throught the model
                inputs.push_back(inputTensor);
                auto output = gestureModel.forward(inputs).toTensor();

                qDebug() << "You performed " << output.argmax(1).item().toInt();

                inputs.clear();

                *predicted = true;
            }
        }
    }
}
