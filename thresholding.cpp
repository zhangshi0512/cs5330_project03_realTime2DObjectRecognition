// thresholding.cpp

#include "thresholding.h"

void dynamicThresholding(const cv::Mat& src, cv::Mat& dst) {
    // Pre-process: Apply Gaussian blur
    cv::Mat blurred;
    cv::GaussianBlur(src, blurred, cv::Size(5, 5), 0);

    // Convert to LAB color space
    cv::Mat labImage;
    cv::cvtColor(blurred, labImage, cv::COLOR_BGR2Lab);

    // Reshape the image to be a 1 column matrix
    cv::Mat reshapedImage = labImage.reshape(1, labImage.cols * labImage.rows);
    reshapedImage.convertTo(reshapedImage, CV_32F);

    // Run k-means
    int clusterCount = 2;
    cv::Mat labels;
    cv::Mat centers;
    cv::kmeans(reshapedImage, clusterCount, labels, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_PP_CENTERS, centers);

    // Calculate the average intensity of the two clusters
    float center1 = (centers.at<float>(0, 0) + centers.at<float>(0, 1) + centers.at<float>(0, 2)) / 3.0f;
    float center2 = (centers.at<float>(1, 0) + centers.at<float>(1, 1) + centers.at<float>(1, 2)) / 3.0f;

    // Set the threshold to be the average of the two means
    int thresholdValue = static_cast<int>((center1 + center2) / 2.0f);

    // Threshold the image
    thresholdImage(src, dst, thresholdValue);
}

void thresholdImage(const cv::Mat& src, cv::Mat& dst, int thresholdValue) {
    dst = src.clone();
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            cv::Vec3b& pixel = dst.at<cv::Vec3b>(i, j);
            if (pixel[0] + pixel[1] + pixel[2] < 3 * thresholdValue) {
                pixel[0] = pixel[1] = pixel[2] = 0; // Set to black
            }
            else {
                pixel[0] = pixel[1] = pixel[2] = 255; // Set to white
            }
        }
    }
}
