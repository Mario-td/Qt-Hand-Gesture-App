#ifndef HAND_DETECTOR_PROCESS_LAUNCHER_H
#define HAND_DETECTOR_PROCESS_LAUNCHER_H

#include <QThread>

#define PREDICT_GESTURE_PROCESS_COMMAND "GLOG_logtostderr=1 ./hand_tracking_cpu"
#define KILL_PREDICT_GESTURE_PROCESS_COMMAND "kill $(ps aux | grep 'hand_tracking_cpu' | grep -v grep | awk '{print $2}') > /dev/null 2>&1"

// This thread executes the process that detects handlandmarks
// from the frames in the shared memory
class HandDetectorProcessLauncher : public QThread
{
    Q_OBJECT
public:
    void killHandDetectorProccess();
private:
    void run() override;
};

#endif // HAND_DETECTOR_PROCESS_LAUNCHER_H
