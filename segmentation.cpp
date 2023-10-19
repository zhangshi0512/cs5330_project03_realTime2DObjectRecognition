#include "segmentation.h"

cv::Mat segmentImage(const cv::Mat& cleaned) {
    cv::Mat labels, stats, centroids;
    int nLabels = cv::connectedComponentsWithStats(cleaned, labels, stats, centroids);
    return labels;
}
