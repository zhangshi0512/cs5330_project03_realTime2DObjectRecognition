// feature_extraction.cpp
// This file contains functions for extracting features from regions in an image, such as contours,
// oriented bounding boxes, aspect ratios, and Hu moments, as well as computing the Hausdorff distance between contours.

// CS 5330 Computer Vision
// Fall 2023 Dec 9th
// Author: Shi Zhang

#include "feature_extraction.h"

// Function to compute the contour of a specific region in an image based on label IDs
std::vector<cv::Point> computeRegionContour(const cv::Mat& labels, int regionID) {
    std::vector<std::vector<cv::Point>> contours;
    cv::Mat regionMask;
    cv::compare(labels, regionID, regionMask, cv::CMP_EQ);

    cv::findContours(regionMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    if (contours.size() > 0) {
        return contours[0];
    }
    else {
        return {};
    }
}

// Function to compute various features for a specific region in an image, including oriented bounding box, 
// aspect ratio, percent filled, and image moments
RegionFeatures computeRegionFeatures(const cv::Mat& labels, int regionID) {
    RegionFeatures features;
    std::vector<cv::Point2f> regionPoints;

    // Extract points belonging to the region
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            if (labels.at<int>(i, j) == regionID) {
                regionPoints.push_back(cv::Point2f(j, i));  // Note: (j, i) because j is x (column) and i is y (row)
            }
        }
    }

    // Skip if there are not enough points
    if (regionPoints.size() < 2) {
        std::cerr << "Error: Not enough distinct points in region " << regionID << " to compute features." << std::endl;
        return features;
    }

    try {
        // Compute the oriented bounding box
        features.orientedBoundingBox = cv::minAreaRect(regionPoints);

        // Compute the aspect ratio of the oriented bounding box
        features.bboxAspectRatio = std::max(features.orientedBoundingBox.size.width, features.orientedBoundingBox.size.height) /
            std::min(features.orientedBoundingBox.size.width, features.orientedBoundingBox.size.height);

        // Compute percent filled within the oriented bounding box
        cv::Mat mask = cv::Mat::zeros(labels.size(), CV_8U);
        for (const auto& point : regionPoints) {
            mask.at<uchar>(point.y, point.x) = 255;
        }
        cv::RotatedRect rect = features.orientedBoundingBox;
        cv::Rect boundingBox = rect.boundingRect();
        // Clip the bounding box to ensure it's within image boundaries
        boundingBox &= cv::Rect(0, 0, mask.cols, mask.rows);
        cv::Mat croppedMask = mask(boundingBox);
        double areaFilled = cv::countNonZero(croppedMask);
        double totalArea = boundingBox.width * boundingBox.height;
        features.percentFilled = areaFilled / totalArea;

        // Compute the image moments for the region
        cv::Moments m = cv::moments(regionPoints);

        // Calculate the orientation (or angle) of the region
        double delta_x = 2 * m.mu11;
        double delta_y = m.mu20 - m.mu02;
        double orientation = 0.5 * std::atan2(delta_x, delta_y);

        // The axis of least moment is perpendicular to the orientation
        features.axisOfLeastMoment[0] = std::cos(orientation + CV_PI / 2);
        features.axisOfLeastMoment[1] = std::sin(orientation + CV_PI / 2);

        // Calculate the orthogonal vector
        features.orthogonalVector[0] = -features.axisOfLeastMoment[1];
        features.orthogonalVector[1] = features.axisOfLeastMoment[0];

        // Compute the Hu Moments
        cv::HuMoments(m, features.huMoments);

        // Compute the contour for the region and store it in features
        features.contour = computeRegionContour(labels, regionID);

    }
    catch (const cv::Exception& e) {
        std::cerr << "Error computing features for region " << regionID << ": " << e.what() << std::endl;
    }

    return features;
}

// Function to compute the Hausdorff Distance between two contours, a measure of shape similarity
float hausdorffDistance(const std::vector<cv::Point>& contour1, const std::vector<cv::Point>& contour2) {
    auto h = [](const std::vector<cv::Point>& A, const std::vector<cv::Point>& B) -> float {
        float maxDistA = 0.0f;
        for (const auto& a : A) {
            float minDistB = std::numeric_limits<float>::max();
            for (const auto& b : B) {
                float dist = cv::norm(a - b);
                minDistB = std::min(minDistB, dist);
            }
            maxDistA = std::max(maxDistA, minDistB);
        }
        return maxDistA;
    };

    return std::max(h(contour1, contour2), h(contour2, contour1));
}
