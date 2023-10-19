#include "preprocess.h"

cv::Mat thresholdImage(const cv::Mat& input) {
    cv::Mat gray, thresh;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thresh, 128, 255, cv::THRESH_BINARY);
    return thresh;
}

cv::Mat cleanImage(const cv::Mat& thresh) {
    cv::Mat cleaned;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(thresh, cleaned, cv::MORPH_OPEN, kernel);  // Erosion followed by dilation
    return cleaned;
}
