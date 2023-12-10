// display.h
// Description: Provides a function for displaying multiple image processing stages in separate windows.

// CS 5330 Computer Vision
// Fall 2023 Dec 9th
// Author: Shi Zhang

#pragma once
#include <opencv2/opencv.hpp>

// Displays original, thresholded, morphological, and segmented images in separate windows
void displayImages(const cv::Mat& original, const cv::Mat& thresholded, const cv::Mat& morphological, const cv::Mat& segmented);
