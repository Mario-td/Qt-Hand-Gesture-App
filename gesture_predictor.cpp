#include "gesture_predictor.h"

int GesturePredictor::runModel()
{
    fillInputTensor();
    //shMemoryReader.readCoordinatesFromMemory();
    auto output = gestureClassificationModel.forward(modelInput).toTensor();
    return output.argmax(1).item().toInt();
}

void GesturePredictor::fillInputTensor()
{
    float *gestureCoordinates = shMemoryReader.getCoordinatesMemoryLocation();
    torch::Tensor gestureSequenceTensor = torch::zeros({Utilities::NUM_KEYPOINTS * 2, Utilities::FRAMES_PER_SEQUENCE});

    for (int i = 0; i < Utilities::FRAMES_PER_SEQUENCE; ++i) {
        for (int j = 0; j < Utilities::NUM_KEYPOINTS * 2; j += 2) {
            gestureSequenceTensor[j][i] =
                gestureCoordinates[i * Utilities::NUM_KEYPOINTS * 2 + j];
            gestureSequenceTensor[j + 1][i] =
                gestureCoordinates[i * Utilities::NUM_KEYPOINTS * 2 + j + 1];

            std::cout << "\n Tensor x:" << gestureSequenceTensor[j][i] << std::endl;

            std::cout << "\n Reader Frame " << i << " Landmark " << j / 2 << ":" << std::endl;
            std::cout << "\tx:" << gestureCoordinates[i * Utilities::NUM_KEYPOINTS * 2 + j] << std::endl;
            std::cout << "\ty:" << gestureCoordinates[i * Utilities::NUM_KEYPOINTS * 2 + j + 1] << std::endl;
        }
    }
    modelInput[0] = gestureSequenceTensor;
}
