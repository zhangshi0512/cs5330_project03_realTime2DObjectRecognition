// thresholding.h

#ifndef THRESHOLDING_H
#define THRESHOLDING_H

#include <opencv2/opencv.hpp>

void dynamicThresholding(const cv::Mat& src, cv::Mat& dst);

void thresholdImage(const cv::Mat& src, cv::Mat& dst, int thresholdValue);

#endif // THRESHOLDING_H
