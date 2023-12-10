// morphologicalOperations.h
// Description: Provides a function to apply morphological operations (such as erosion and dilation) to images.

// CS 5330 Computer Vision
// Fall 2023 Dec 9th
// Author: Shi Zhang

#pragma once
#include <opencv2/opencv.hpp>

// Applies morphological operations (erosion and dilation) to an image
void applyMorphologicalOperations(const cv::Mat& src, cv::Mat& dst);
