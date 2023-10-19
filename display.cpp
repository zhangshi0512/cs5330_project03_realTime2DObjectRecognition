// display.cpp

#include "display.h"

void displayImages(const cv::Mat& original, const cv::Mat& thresholded) {
    cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
    cv::imshow("Original Image", original);
    int width = 1920 / 2; // half the screen width
    int height = (int)(width * (original.rows / (float)original.cols));
    cv::resizeWindow("Original Image", width, height);

    cv::namedWindow("Thresholded Image", cv::WINDOW_NORMAL);
    cv::imshow("Thresholded Image", thresholded);
    height = (int)(width * (thresholded.rows / (float)thresholded.cols));
    cv::resizeWindow("Thresholded Image", width, height);

    cv::waitKey(0);
}
