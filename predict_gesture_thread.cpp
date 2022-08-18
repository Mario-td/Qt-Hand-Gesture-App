#include "predict_gesture_thread.h"

PredictGestureThread::PredictGestureThread(std::shared_ptr<bool> run, QQueue<cv::Mat> *frameVector,
                                           QMutex *lock):
    running(run), predictingFrames(frameVector), predictingDataLock(lock), extractingKeypoints(false),
    sequenceIdx(0)
{
    using namespace boost::interprocess;
    // Create a shared memory object.
    shared_memory_object::remove("MySharedMemory");
    shared_memory_object shm(create_only, "MySharedMemory", read_write);

    // Set size
    shm.truncate(imageSizeBytes * Utilities::FRAMES_PER_SEQUENCE);

    // Map the whole shared memory in this process
    region = new mapped_region(shm, read_write);

    void *region_address = region->get_address();
    std::memset(region_address, 0, region->get_size());

    coordinatesBuff = static_cast<float *>(region_address);
    imageBuff = static_cast<uchar *>(region_address);

    named_semaphore::remove("Semaphore");
    semaphore = new named_semaphore (create_only_t(), "Semaphore", 0);
    /*
        QString program =
            "~/mediapipe/Simplified-hand-tracking-with-Mediapipe-CPP/run.sh";

        QProcess *myProcess = new QProcess(this);
        myProcess->start(program);
    */
    thrd = new std::thread ([]() {
        std::cout << "Launch process\n";
        std::string
        s("~/mediapipe/Simplified-hand-tracking-with-Mediapipe-CPP/run.sh");
        std::system(s.c_str());
    });
    handKeypointModel = torch::jit::script::Module(torch::jit::load("./hand.pts", torch::kCUDA));
    gestureClassificationModel = torch::jit::script::Module(torch::jit::load("./model.pt"));
}

void PredictGestureThread::run()
{
    while (*running) {
        putFrameInSharedMemory();
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
        qDebug("Here");
        passThroughGestureModel();
        thrd->join();
        qDebug("Here2f");
        for (int i = 0; i < Utilities::FRAMES_PER_SEQUENCE; ++i) {
            for (int j = 0; j < Utilities::NUM_KEYPOINTS * 2; j += 2) {
                std::cout << "\nLandmark " << j / 2 << ":" << std::endl;
                std::cout << "\tx:" << coordinatesBuff[i * Utilities::NUM_KEYPOINTS * 2 + j] << std::endl;
                std::cout << "\ty:" << coordinatesBuff[i * Utilities::NUM_KEYPOINTS * 2 + j + 1] << std::endl;
            }
        }

        delete thrd;
        thrd = new std::thread ([]() {
            std::string
            s("GLOG_logtostderr=1 ~/mediapipe/bazel-bin/Simplified-hand-tracking-with-Mediapipe-CPP/hand_tracking_gpu");
            //std::system(s.c_str());
        });
    }
}

void PredictGestureThread::passThroughGestureModel()
{
    // Forwards the input through the sequence classification model
    gestureClassificationModelInput.push_back(gestureSequenceTensor);
    auto output = gestureClassificationModel.forward(gestureClassificationModelInput).toTensor();

    // signal with the predicted gesture
    //emit finishedPrediction(Utilities::GESTURE_NAMES[output.argmax(1).item().toInt()]);

    QThread::sleep(6);

    gestureClassificationModelInput.clear();

    emit resetPrediction(true);
}

void PredictGestureThread::putFrameInSharedMemory()
{
    predictingDataLock->lock();
    if (!predictingFrames->empty()) {
        recordedFrame = predictingFrames->front();
        std::memcpy(&imageBuff[sequenceIdx * imageSizeBytes], recordedFrame.data, imageSizeBytes);
        semaphore->post();
        predictingFrames->dequeue();
        predictingDataLock->unlock();
        extractKeypoints();
    } else {
        predictingDataLock->unlock();
        return;
    }
}
