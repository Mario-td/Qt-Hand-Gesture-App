#include "predict_gesture_thread.h"

PredictGestureThread::PredictGestureThread(std::shared_ptr<bool> run, QQueue<cv::Mat> *frameVector,
                                           QMutex *lock):
    running(run), predictingFrames(frameVector), predictingDataLock(lock), extractingKeypoints(false),
    sequenceIdx(0)
{
    using namespace boost::interprocess;
    // Remove shared memory on construction and destruction
    struct shm_remove {
        shm_remove()
        {
            shared_memory_object::remove("MySharedMemory");
        }
        ~shm_remove()
        {
            shared_memory_object::remove("MySharedMemory");
        }
    } remover;

    // Create a shared memory object.
    shared_memory_object shm(create_only, "MySharedMemory", read_write);

    // Set size
    size_t image_size_bytes = Utilities::FRAME_WIDTH * Utilities::FRAME_HEIGHT * 3;
    shm.truncate(image_size_bytes * Utilities::FRAMES_PER_SEQUENCE);

    // Map the whole shared memory in this process
    mapped_region region(shm, read_write);

    named_semaphore::remove("Semaphore");
    named_semaphore semaphore(create_only_t(), "Semaphore", 0);

    qDebug("Seems to work\n");

    handKeypointModel = torch::jit::script::Module(torch::jit::load("./hand.pts", torch::kCUDA));
    gestureClassificationModel = torch::jit::script::Module(torch::jit::load("./model.pt"));
}

void PredictGestureThread::run()
{
    while (*running) {

        // if it is not extracting keypoints from a frame then dequeue the next frame if it's available
        if (!extractingKeypoints) {
            dequeueSequenceFrame();
        } else {
            extractKeypoints();
        }
    }
}

void PredictGestureThread::dequeueSequenceFrame()
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

void PredictGestureThread::extractKeypoints()
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

    QThread::sleep(6);

    gestureClassificationModelInput.clear();

    emit resetPrediction(true);
}
