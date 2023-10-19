#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <opencv2/opencv.hpp>

cv::Mat thresholdImage(const cv::Mat& input);
cv::Mat cleanImage(const cv::Mat& thresh);

#endif // PREPROCESS_H