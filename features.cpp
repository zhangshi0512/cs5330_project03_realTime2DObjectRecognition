#include "features.h"

Features extractFeatures(const cv::Mat& region) {
    Features f;
    // For simplicity, let's calculate the aspect ratio
    f.aspectRatio = static_cast<double>(region.cols) / region.rows;
    // ... extract other features as needed
    return f;
}
