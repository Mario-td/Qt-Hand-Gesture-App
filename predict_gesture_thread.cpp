#include "predict_gesture_thread.h"

PredictGestureThread::PredictGestureThread(bool &run, QQueue<cv::Mat> &frameVector,
                                           QMutex *lock):
    running(&run), predictingFrames(&frameVector), predictingDataLock(lock)
{
    handKeypointModel = new torch::jit::script::Module(torch::jit::load("./hand.pts", torch::kCUDA));
    gestureClassificationModel = new torch::jit::script::Module(torch::jit::load("./model.pt"));
}

void PredictGestureThread::ExtractKeypoints(cv::Mat &frame)
{

}

void PredictGestureThread::run()
{
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
            handKeypoints = Utilities::pyramidinference(*handKeypointModel, recordedFrame, handrect);

            // Assigns the coordinates of the keypoints to the tensor, for each time step
            for (int i = 0; i < Utilities::KEYPOINTS; i++) {
                if (!handKeypoints[i].empty()) {
                    inputTensor[0][2 * i][sequenceIdx] = handKeypoints[i].begin()->second.x;
                    inputTensor[0][2 * i + 1][sequenceIdx] = handKeypoints[i].begin()->second.y;
                }
            }
            sequenceIdx++;
            extractingKeypoints = false;

            // set to cero after completing a sequence
            if (sequenceIdx > Utilities::FRAMES_PER_SEQUENCE - 1) {
                qDebug() << "Finished with the sequence";

                sequenceIdx = 0;

                // Forwards the input throught the model
                inputs.push_back(inputTensor);
                auto output = gestureClassificationModel->forward(inputs).toTensor();

                qDebug() << "You performed " << output.argmax(1).item().toInt();

                inputs.clear();
            }
        }
    }
}
