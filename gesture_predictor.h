#ifndef GESTURE_PREDICTOR_H
#define GESTURE_PREDICTOR_H

#include <torch/script.h>

#include "shared_memory_reader.h"
#include "resources.h"

class GesturePredictor
{
public:
    int runModel();
private:
    SharedMemoryReader shMemoryReader{};
    torch::jit::script::Module gestureClassificationModel =
        torch::jit::script::Module(torch::jit::load(GESTURE_CLASSIFIER_MODEL_PATH));
    std::vector<torch::jit::IValue> modelInput{0};
    void fillInputTensor();
};

#endif // GESTURE_PREDICTOR_HG
