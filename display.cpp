// display.cpp
// This file contains functions for displaying multiple images in a single window. 
// It provides utilities to organize and visualize different stages of image processing.

// Author: Shi Zhang

#include "display.h"

// Displays multiple images (original, thresholded, morphological, and segmented) in separate windows
void displayImages(const cv::Mat& original, const cv::Mat& thresholded, const cv::Mat& morphological, const cv::Mat& segmented) {
    int width = 1920 / 4; // divide screen width into four for four images

    // Display Original Image
    cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
    cv::imshow("Original Image", original);
    int height = (int)(width * (original.rows / (float)original.cols));
    cv::resizeWindow("Original Image", width, height);

    // Display Thresholded Image
    cv::namedWindow("Thresholded Image", cv::WINDOW_NORMAL);
    cv::imshow("Thresholded Image", thresholded);
    cv::moveWindow("Thresholded Image", width, 0);  // move the window to the right of the original image
    height = (int)(width * (thresholded.rows / (float)thresholded.cols));
    cv::resizeWindow("Thresholded Image", width, height);

    // Display Morphological Image
    cv::namedWindow("Morphological Image", cv::WINDOW_NORMAL);
    cv::imshow("Morphological Image", morphological);
    cv::moveWindow("Morphological Image", 2 * width, 0);  // move the window to the right of the thresholded image
    height = (int)(width * (morphological.rows / (float)morphological.cols));
    cv::resizeWindow("Morphological Image", width, height);

    // Display Segmented Image
    if (!segmented.empty()) {
        cv::namedWindow("Segmented Image", cv::WINDOW_NORMAL);
        cv::imshow("Segmented Image", segmented);
        cv::moveWindow("Segmented Image", 3 * width, 0);  // move the window to the right of the morphological image
        height = (int)(width * (segmented.rows / (float)segmented.cols));
        cv::resizeWindow("Segmented Image", width, height);
    }
    else {
        std::cerr << "Error: Segmented Image is empty and cannot be displayed." << std::endl;
    }

    cv::waitKey(0);
}


