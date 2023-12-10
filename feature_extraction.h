// feature_extraction.h
// Description: Contains functions and structures for extracting and analyzing features from labeled regions in images.
// This includes computing region contours, oriented bounding boxes, aspect ratios, Hu moments, and the Hausdorff distance.

// CS 5330 Computer Vision
// Fall 2023 Dec 9th
// Author: Shi Zhang

#ifndef FEATURE_EXTRACTION_H
#define FEATURE_EXTRACTION_H

#include <opencv2/opencv.hpp>
#include <vector>

// Structure to hold region features
struct RegionFeatures {
    cv::RotatedRect orientedBoundingBox;
    cv::Vec2f axisOfLeastMoment;
    cv::Vec2f orthogonalVector;
    float percentFilled;
    float bboxAspectRatio;
    double huMoments[7];
    std::vector<cv::Point> contour;
};

// Computes various features for a specific region in an image
RegionFeatures computeRegionFeatures(const cv::Mat& labels, int regionID);

// Computes the contour of a specific region in an image
std::vector<cv::Point> computeRegionContour(const cv::Mat& labels, int regionID);

// Computes the Hausdorff distance between two contours
float hausdorffDistance(const std::vector<cv::Point>& contour1, const std::vector<cv::Point>& contour2);

#endif // FEATURE_EXTRACTION_H
