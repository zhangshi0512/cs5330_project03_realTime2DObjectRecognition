#include <opencv2/opencv.hpp>

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


void applyMorphologicalOperations(const cv::Mat& src, cv::Mat& dst) {
    // Define a larger kernel for the morphological operations
    cv::Mat kernel = cv::Mat::ones(5, 5, CV_8U);

    // Erode and then dilate the image
    cv::Mat eroded;
    erode(src, eroded, kernel);
    dilate(eroded, dst, kernel);
}



/***
* // morphological function directly using openCV
void applyMorphologicalOperations(const cv::Mat& src, cv::Mat& dst) {
    // Define a kernel for the morphological operations
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

    // Erode the image to remove small noises
    cv::erode(src, dst, kernel);

    // Dilate the image to restore the eroded parts
    cv::dilate(dst, dst, kernel);
}
***/