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

    const std::vector<float> HAND_COLORS_RENDER {
        100.f, 100.f, 100.f,
        100.f, 0.f, 0.f,
        150.f, 0.f, 0.f,
        200.f, 0.f, 0.f,
        255.f, 0.f, 0.f,
        100.f, 100.f, 0.f,
        150.f, 150.f, 0.f,
        200.f, 200.f, 0.f,
        255.f, 255.f, 0.f,
        0.f, 100.f, 50.f,
        0.f, 150.f, 75.f,
        0.f, 200.f, 100.f,
        0.f, 255.f, 125.f,
        0.f, 50.f, 100.f,
        0.f, 75.f, 150.f,
        0.f, 100.f, 200.f,
        0.f, 125.f, 255.f,
        100.f, 0.f, 100.f,
        150.f, 0.f, 150.f,
        200.f, 0.f, 200.f,
        255.f, 0.f, 255.f};

    void nmslocation(cv::Mat &src, std::map<float, cv::Point2f, std::greater<float>> &location,
                     float threshold);
    float transformNetInput(torch::Tensor &inputTensor, const cv::Mat &src_img, int tensor_index = 0);
    void detectBbox(std::vector<cv::Rect> &handrect, torch::jit::script::Module &model,
                    const cv::Mat &inputImage);
    void detecthand(std::vector<std::map<float, cv::Point2f>> &manypoints,
                    torch::jit::script::Module &model, const cv::Mat &inputImage,
                    const std::vector<cv::Rect> &handrect);
    std::vector<std::map<float, cv::Point2f>> pyramidinference(torch::jit::script::Module &model,
                                                               cv::Mat &inputImage, std::vector<cv::Rect> &handrect);
};

#endif // UTILITIES_H
