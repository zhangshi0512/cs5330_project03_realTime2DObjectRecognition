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

RegionFeatures computeRegionFeatures(const cv::Mat& labels, int regionID);
std::vector<cv::Point> computeRegionContour(const cv::Mat& labels, int regionID);
float hausdorffDistance(const std::vector<cv::Point>& contour1, const std::vector<cv::Point>& contour2);

#endif // FEATURE_EXTRACTION_H
