#include <opencv2/opencv.hpp>
#include "preprocess.h"
#include "segmentation.h"
#include "features.h"
#include "classification.h"

int main() {
    cv::VideoCapture cap(0);  // open the default camera
    if (!cap.isOpened()) {
        std::cerr << "Error opening camera!" << std::endl;
        return -1;
    }

    while (true) {
        cv::Mat frame;
        cap >> frame;  // get a new frame from camera

        cv::Mat thresh = thresholdImage(frame);
        cv::Mat cleaned = cleanImage(thresh);
        cv::Mat labels = segmentImage(cleaned);

        // ... Extract features and classify

        cv::imshow("Original", frame);

        if (cv::waitKey(30) >= 0) break;
    }
    return 0;
}
