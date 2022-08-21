#include "hand_detector_process_launcher.h"

void HandDetectorProcessLauncher::run()
{
    std::system(PREDICT_GESTURE_PROCESS_COMMAND);
}

void HandDetectorProcessLauncher::killHandDetectorProccess()
{
    std::system(KILL_PREDICT_GESTURE_PROCESS_COMMAND);
}
