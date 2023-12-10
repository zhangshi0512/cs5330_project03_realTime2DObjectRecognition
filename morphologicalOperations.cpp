// MorphologicalOperations.cpp
// This file contains custom implementations of morphological operations (erosion, dilation)
// and their application to process images using OpenCV.

// CS 5330 Computer Vision
// Fall 2023 Dec 9th
// Author: Shi Zhang

#include <opencv2/opencv.hpp>

// Erodes an image using a specified kernel
void erode(const cv::Mat& src, cv::Mat& dst, const cv::Mat& kernel) {
    dst = src.clone();
    int kRowsHalf = kernel.rows / 2;
    int kColsHalf = kernel.cols / 2;

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            bool erodePixel = true;
            for (int x = -kRowsHalf; x <= kRowsHalf && erodePixel; x++) {
                for (int y = -kColsHalf; y <= kColsHalf; y++) {
                    int newX = i + x;
                    int newY = j + y;
                    if (newX >= 0 && newX < src.rows && newY >= 0 && newY < src.cols) {
                        if (kernel.at<uchar>(x + kRowsHalf, y + kColsHalf) == 1 && src.at<uchar>(newX, newY) == 0) {
                            erodePixel = false;
                            break;
                        }
                    }
                }
            }
            dst.at<uchar>(i, j) = erodePixel ? 255 : 0;
        }
    }
}

// Dilates an image using a specified kernel
void dilate(const cv::Mat& src, cv::Mat& dst, const cv::Mat& kernel) {
    dst = src.clone();
    int kRowsHalf = kernel.rows / 2;
    int kColsHalf = kernel.cols / 2;

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            bool dilatePixel = false;
            for (int x = -kRowsHalf; x <= kRowsHalf && !dilatePixel; x++) {
                for (int y = -kColsHalf; y <= kColsHalf; y++) {
                    int newX = i + x;
                    int newY = j + y;
                    if (newX >= 0 && newX < src.rows && newY >= 0 && newY < src.cols) {
                        if (kernel.at<uchar>(x + kRowsHalf, y + kColsHalf) == 1 && src.at<uchar>(newX, newY) == 255) {
                            dilatePixel = true;
                            break;
                        }
                    }
                }
            }
            dst.at<uchar>(i, j) = dilatePixel ? 255 : 0;
        }
    }
}

// Applies a series of morphological operations (erosion followed by dilation) to an image
void applyMorphologicalOperations(const cv::Mat& src, cv::Mat& dst) {
    // Define a larger kernel for the morphological operations
    cv::Mat kernel = cv::Mat::ones(5, 5, CV_8U);

    // Erode and then dilate the image
    cv::Mat eroded;
    erode(src, eroded, kernel);
    dilate(eroded, dst, kernel);
}
