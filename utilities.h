#ifndef UTILITIES_H
#define UTILITIES_H

#include <torch/script.h>
#include <opencv2/opencv.hpp>

class Utilities
{
public:

    static constexpr int TRAIN_IMAGE_HEIGHT = 256;
    static constexpr int TRAIN_IMAGE_WIDTH = 256;
    static constexpr double LABEL_MIN = 0.2;
    static constexpr int NUMBER_GESTURES = 5;

    // Number of frames per sequence and number of keypoints detected in each frame
    static constexpr unsigned short FRAMES_PER_SEQUENCE = 32;
    static constexpr unsigned short NUM_KEYPOINTS = 21;

    static void nmslocation(cv::Mat &src, std::map<float, cv::Point2f, std::greater<float>> &location,
                            float threshold);
    static float transformNetInput(torch::Tensor &inputTensor, const cv::Mat &src_img,
                                   int tensor_index = 0);
    static void detectBbox(std::vector<cv::Rect> &handrect, torch::jit::script::Module &model,
                           const cv::Mat &inputImage);
    static void detecthand(std::vector<std::map<float, cv::Point2f>> &manypoints,
                           torch::jit::script::Module &model, const cv::Mat &inputImage,
                           const std::vector<cv::Rect> &handrect);
    static std::vector<std::map<float, cv::Point2f>> pyramidinference(torch::jit::script::Module &model,
                                                                      cv::Mat &inputImage, std::vector<cv::Rect> &handrect);
};

#endif // UTILITIES_H
