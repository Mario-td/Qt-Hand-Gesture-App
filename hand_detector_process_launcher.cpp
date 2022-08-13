#include "hand_detector_process_launcher.h"

void HandDetectorProcessLauncher::run()
{
    int result = 2;
    std::string
    s("GLOG_logtostderr=1 ~/mediapipe/bazel-bin/Qt-Hand-Gesture-App/hand_tracking_cpu");
    std::system(s.c_str());
}

void HandDetectorProcessLauncher::killHandDetectorProccess()
{
    std::system("kill $(ps aux | grep 'Qt-Hand-Gesture-App/hand_tracking_cpu' | grep -v grep | awk '{print $2}')");
}
