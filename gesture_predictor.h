#ifndef GESTURE_PREDICTOR_H
#define GESTURE_PREDICTOR_H

#include "shared_memory_reader.h"

class GesturePredictor
{
public:
    int runModel()
    {
        shMemoryReader.readCoordinatesFromMemory();
        return 4;
    }
private:
    SharedMemoryReader shMemoryReader{};
};

#endif // GESTURE_PREDICTOR_HG
