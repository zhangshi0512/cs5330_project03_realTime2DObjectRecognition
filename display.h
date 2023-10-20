#pragma once
#include <opencv2/opencv.hpp>

void displayImages(const cv::Mat& original, const cv::Mat& thresholded, const cv::Mat& morphological, const cv::Mat& segmented);
