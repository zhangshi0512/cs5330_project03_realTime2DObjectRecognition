// thresholding.h
// Description: Provides functions for dynamic thresholding of images using OpenCV.
// These functions include dynamic thresholding and simple thresholding based on a specified value.

// CS 5330 Computer Vision
// Fall 2023 Dec 9th
// Author: Shi Zhang


#ifndef THRESHOLDING_H
#define THRESHOLDING_H

#include <opencv2/opencv.hpp>

// Applies dynamic thresholding to an image
void dynamicThresholding(const cv::Mat& src, cv::Mat& dst);

// Applies a simple threshold to an image based on a specified threshold value
void thresholdImage(const cv::Mat& src, cv::Mat& dst, int thresholdValue);

#endif // THRESHOLDING_H
