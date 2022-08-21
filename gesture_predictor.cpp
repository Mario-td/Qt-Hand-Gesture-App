#include "gesture_predictor.h"

int GesturePredictor::runModel()
{
    fillInputTensor();
    auto output = gestureClassificationModel.forward(modelInput).toTensor();
    return output.argmax(1).item().toInt();
}

void GesturePredictor::fillInputTensor()
{
    float *gestureCoordinates = shMemoryReader.getCoordinatesMemoryLocation();
    torch::Tensor gestureSequenceTensor = torch::zeros({NUM_KEYPOINTS * 2, FRAMES_PER_SEQUENCE});

    for (int i = 0; i < FRAMES_PER_SEQUENCE; ++i) {
        for (int j = 0; j < NUM_KEYPOINTS * 2; j += 2) {
            gestureSequenceTensor[j][i] =
                gestureCoordinates[i * NUM_KEYPOINTS * 2 + j];
            gestureSequenceTensor[j + 1][i] =
                gestureCoordinates[i * NUM_KEYPOINTS * 2 + j + 1];
        }
    }
    modelInput[0] = gestureSequenceTensor;
}
