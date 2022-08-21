#ifndef CONSTANTS_H
#define CONSTANTS_H

static constexpr int FRAME_WIDTH = 640;
static constexpr int FRAME_HEIGHT = 480;

// Number of frames per sequence and number of keypoints detected in each frame
static constexpr unsigned short FRAMES_PER_SEQUENCE = 32;
static constexpr unsigned short NUM_KEYPOINTS = 21;

// Gesture labels
static constexpr const char *GESTURE_NAMES[] {"WAVING", "SCISSORS", "FLIP", "PUSH&PULL", "OPEN&CLOSE"};
static constexpr int NUMBER_OF_GESTURES {sizeof(GESTURE_NAMES) / sizeof(char *)};

#endif // CONSTANTS_H
