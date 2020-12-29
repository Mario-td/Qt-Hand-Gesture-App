// Functions extracted from https://www.yangangwang.com/papers/WANG-SRH-2019-07.html, accessed on 07/11/2020

#include "utilities.h"

void Utilities::nmslocation(cv::Mat &src,
                            std::map<float, cv::Point2f, std::greater<float>> &location, float threshold)
{
    // clear all the points
    location.clear();

    // set the local window size: 5*5
    int blockwidth = 2;

    // for each pixel window, search the local maximum
    #pragma omp parallel for
    for (int i = blockwidth; i < src.cols - blockwidth; i++) {
        for (int j = blockwidth; j < src.rows - blockwidth; j++) {
            cv::Point2i tmploc(i, j);

            // candidate keypoint point
            float localvalue = src.at<float>(tmploc);
            if (localvalue < threshold)
                continue;

            // check whether it is local maximum
            bool localmaximum = true;
            for (int m = std::max(tmploc.x - blockwidth, 0); m <= std::min(tmploc.x + blockwidth, src.cols - 1);
                    m++) {
                for (int n = std::max(tmploc.y - blockwidth, 0); n <= std::min(tmploc.y + blockwidth, src.rows - 1);
                        n++) {
                    if (src.at<float>(cv::Point2i(m, n)) > localvalue) {
                        localmaximum = false;
                        break;
                    }
                }
                if (!localmaximum)
                    break;
            }

            // output the location
            if (localmaximum) {
                #pragma omp critical
                {
                    if (localmaximum)
                    {
                        location.insert(std::make_pair(localvalue, tmploc));
                    }
                }
            }
        }
    }
}

float Utilities::transformNetInput(at::Tensor &inputTensor, const cv::Mat &src_img,
                                   int tensor_index)
{
    // lmbada expression
    auto fastmin = [](float a, float b) {
        return a < b ? a : b;
    };

    // convert the input image
    cv::Mat dst;
    float ratio = fastmin(float(inputTensor.size(2)) / src_img.rows,
                          float(inputTensor.size(3)) / src_img.cols);
    cv::Mat M = (cv::Mat_<float>(2, 3) << ratio, 0, 0, 0, ratio, 0);
    warpAffine(src_img, dst, M, cv::Size(inputTensor.size(3), inputTensor.size(2)),
               cv::INTER_CUBIC, cv::BORDER_CONSTANT, cv::Scalar(128, 128, 128));

    dst.convertTo(dst, CV_32F);
    dst = dst / 255.f - 0.5f;
    std::vector<cv::Mat> chn_img;
    split(dst, chn_img);

    size_t total_bytes = sizeof(float) * inputTensor.size(2) * inputTensor.size(3);
    if (chn_img.size() == 1) {
        memcpy(inputTensor[tensor_index][0].data_ptr<float>(), (float *)chn_img[0].data, total_bytes);
        memcpy(inputTensor[tensor_index][1].data_ptr<float>(), (float *)chn_img[0].data, total_bytes);
        memcpy(inputTensor[tensor_index][2].data_ptr<float>(), (float *)chn_img[0].data, total_bytes);
    } else {
        memcpy(inputTensor[tensor_index][0].data_ptr<float>(), (float *)chn_img[0].data, total_bytes);
        memcpy(inputTensor[tensor_index][1].data_ptr<float>(), (float *)chn_img[1].data, total_bytes);
        memcpy(inputTensor[tensor_index][2].data_ptr<float>(), (float *)chn_img[2].data, total_bytes);
    }
    return ratio;
}

void Utilities::detectBbox(std::vector<cv::Rect> &handrect, torch::jit::script::Module &model,
                           const cv::Mat &inputImage)
{
    // init the tensor
    auto inputTensor = torch::zeros({1, 3, TRAIN_IMAGE_HEIGHT, TRAIN_IMAGE_WIDTH});

    // transform the input data
    float ratio_input_to_net = transformNetInput(inputTensor, inputImage);

    // run the network
    auto heatmap = model.forward({inputTensor.cuda()}).toTuple()->elements()[3].toTensor();

    // copy the 3-channel rect map
    std::vector<cv::Mat> rectmap(3);
    float ratio_net_downsample = TRAIN_IMAGE_HEIGHT / float(heatmap.size(2));
    int rect_map_idx = heatmap.size(1) - 3;
    for (int i = 0; i < 3; i++) {
        rectmap[i] = cv::Mat::zeros(heatmap.size(2), heatmap.size(3), CV_32FC1);
        auto ptr = heatmap[0][i + rect_map_idx].cpu().data_ptr<float>();
        memcpy((float *)rectmap[i].data, ptr, sizeof(float) * heatmap.size(2) * heatmap.size(3));
    }
    std::map<float, cv::Point2f, std::greater<float>> locations;
    nmslocation(rectmap[0], locations, LABEL_MIN);
    handrect.clear();
    for (auto iter = locations.begin(); iter != locations.end(); iter++) {
        cv::Point2f points = iter->second;
        int pos_x = points.x;
        int pos_y = points.y;
        float ratio_width = 0.f, ratio_height = 0.f;
        int pixelcount = 0;
        for (int m = std::max(pos_y - 2, 0); m < std::min(pos_y + 3, (int)heatmap.size(2)); m++) {
            for (int n = std::max(pos_x - 2, 0); n < std::min(pos_x + 3, (int)heatmap.size(3)); n++) {
                ratio_width += rectmap[1].at<float>(m, n);
                ratio_height += rectmap[2].at<float>(m, n);
                pixelcount++;
            }
        }
        if (pixelcount > 0) {
            ratio_width = std::min(std::max(ratio_width / pixelcount, 0.f), 1.f);
            ratio_height = std::min(std::max(ratio_height / pixelcount, 0.f), 1.f);

            points = points * ratio_net_downsample / ratio_input_to_net;
            float rect_w = ratio_width * TRAIN_IMAGE_WIDTH / ratio_input_to_net;
            float rect_h = ratio_height * TRAIN_IMAGE_HEIGHT / ratio_input_to_net;
            cv::Point2f l_t = points - cv::Point2f(rect_w / 2.f, rect_h / 2.f);
            cv::Point2f r_b = points + cv::Point2f(rect_w / 2.f, rect_h / 2.f);
            l_t.x = std::max(l_t.x, 0.f);
            l_t.y = std::max(l_t.y, 0.f);
            r_b.x = std::min(r_b.x, inputImage.cols - 1.f);
            r_b.y = std::min(r_b.y, inputImage.rows - 1.f);
            handrect.push_back(cv::Rect(l_t.x, l_t.y, r_b.x - l_t.x, r_b.y - l_t.y));
        }
    }
}

void Utilities::detecthand(std::vector<std::map<float, cv::Point2f>> &manypoints,
                           torch::jit::script::Module &model, const cv::Mat &inputImage, const std::vector<cv::Rect> &handrect)
{
    // transform the input data and copy to the gpu
    auto inputTensor = torch::zeros({(int)handrect.size(), 3, TRAIN_IMAGE_HEIGHT, TRAIN_IMAGE_WIDTH});
    std::vector<float> ratio_input_to_net((int)handrect.size());
    for (int i = 0; i < handrect.size(); i++) {
        ratio_input_to_net[i] = transformNetInput(inputTensor, inputImage(handrect[i]), i);
    }

    // run the network
    auto net_result = model.forward({inputTensor.cuda()}).toTuple()->elements()[3].toTensor();

    // determine the joint position
    float ratio_net_downsample = TRAIN_IMAGE_HEIGHT / float(net_result.size(2));
    size_t total_bytes = sizeof(float) * net_result.size(2) * net_result.size(3);
    for (int rectIdx = 0; rectIdx < handrect.size(); rectIdx++) {

        for (int i = 0; i < net_result.size(1) - 3; i++) {
            cv::Mat heatmap = cv::Mat::zeros(net_result.size(2), net_result.size(3), CV_32FC1);
            memcpy((float *)heatmap.data, net_result[rectIdx][i].cpu().data_ptr<float>(), total_bytes);
            std::map<float, cv::Point2f, std::greater<float>> points;
            nmslocation(heatmap, points, LABEL_MIN);

            // convert to the original image
            int count = 0;
            for (auto iter = points.begin(); iter != points.end(); iter++, count++) {
                // we only detect less than 2 hands in current implementation
                if (count >= 2)
                    break;
                cv::Point2f points = iter->second * ratio_net_downsample / ratio_input_to_net[rectIdx] +
                                     cv::Point2f(handrect[rectIdx].x, handrect[rectIdx].y);
                manypoints[i].insert(std::make_pair(iter->first, points));
            }
        }
    }
}

std::vector<std::map<float, cv::Point2f> > Utilities::pyramidinference(
    torch::jit::script::Module &model, cv::Mat &inputImage, std::vector<cv::Rect> &handrect)
{
    std::vector<std::map<float, cv::Point2f>> many_keypoints(21);

    // first step to determine the rough hand position in the image if not input the handrect
    if (handrect.size() == 0) {
        handrect.push_back(cv::Rect(0, 0, inputImage.cols, inputImage.rows));
        detectBbox(handrect, model, inputImage);

        if (handrect.size() == 0)
            return many_keypoints;
    }

    // for each small image detect the joint points
    detecthand(many_keypoints, model, inputImage, handrect);

    return many_keypoints;
}
