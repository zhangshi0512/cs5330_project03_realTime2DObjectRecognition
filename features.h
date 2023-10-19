#ifndef FEATURES_H
#define FEATURES_H

#include <opencv2/opencv.hpp>

struct Features {
    double aspectRatio;
    // ... any other feature you'd like to extract
};

Features extractFeatures(const cv::Mat& region);

#endif // FEATURES_H
