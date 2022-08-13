#ifndef HAND_DETECTOR_PROCESS_LAUNCHER_H
#define HAND_DETECTOR_PROCESS_LAUNCHER_H

#include <QThread>

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
