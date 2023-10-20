// main.cpp

#include <iostream>
#include <windows.h>
#include <vector>
#include "thresholding.h"
#include "display.h"
#include "morphologicalOperations.h"
#include <opencv2/opencv.hpp>

// function to select image
std::vector<std::string> openFileDialog() {
    OPENFILENAME ofn;
    WCHAR szFile[260] = { 0 };
    std::vector<std::string> fileNames;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"JPEG (*.jpg)\0*.jpg\0All (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileName(&ofn) == TRUE) {
        WCHAR* p = ofn.lpstrFile;
        std::wstring directory = p;
        p += directory.size() + 1;
        while (*p) {
            std::wstring file = directory + L"\\" + p;
            fileNames.push_back(std::string(file.begin(), file.end()));
            p += wcslen(p) + 1;
        }
        if (fileNames.empty()) {
            fileNames.push_back(std::string(directory.begin(), directory.end()));
        }
    }
    return fileNames;
}

BOOL saveFileDialog(std::wstring& outFilePath) {
    OPENFILENAME ofn;
    WCHAR szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"JPEG (*.jpg)\0*.jpg\0All (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt = L"jpg";

    if (GetSaveFileName(&ofn) == TRUE) {
        outFilePath = ofn.lpstrFile;
        return TRUE;
    }
    return FALSE;
}

void displayAndOptionallySave(const cv::Mat& original, const cv::Mat& thresholded, const cv::Mat& cleaned, const cv::Mat& segmented) {
    displayImages(original, thresholded, cleaned, segmented);

    // Prompt for the thresholded image
    int userChoice = MessageBox(NULL, L"Would you like to save the thresholded image?", L"Save Image", MB_YESNO);
    if (userChoice == IDYES) {
        std::wstring savePath;
        if (saveFileDialog(savePath)) {
            if (!cv::imwrite(std::string(savePath.begin(), savePath.end()), thresholded)) {
                MessageBox(NULL, L"Error: Could not save the thresholded image to the specified path.", L"Error", MB_OK);
            }
            else {
                MessageBox(NULL, L"Thresholded image saved successfully!", L"Success", MB_OK);
            }
        }
    }

    // Prompt for the morphological image
    userChoice = MessageBox(NULL, L"Would you like to save the morphological image?", L"Save Image", MB_YESNO);
    if (userChoice == IDYES) {
        std::wstring savePath;
        if (saveFileDialog(savePath)) {
            if (!cv::imwrite(std::string(savePath.begin(), savePath.end()), cleaned)) {
                MessageBox(NULL, L"Error: Could not save the morphological image to the specified path.", L"Error", MB_OK);
            }
            else {
                MessageBox(NULL, L"Morphological image saved successfully!", L"Success", MB_OK);
            }
        }
    }

    // Prompt for the segmented image
    userChoice = MessageBox(NULL, L"Would you like to save the segmented image?", L"Save Image", MB_YESNO);
    if (userChoice == IDYES) {
        std::wstring savePath;
        if (saveFileDialog(savePath)) {
            if (!cv::imwrite(std::string(savePath.begin(), savePath.end()), segmented)) {
                MessageBox(NULL, L"Error: Could not save the segmented image to the specified path.", L"Error", MB_OK);
            }
            else {
                MessageBox(NULL, L"Segmented image saved successfully!", L"Success", MB_OK);
            }
        }
    }
}

bool isBinaryImage(const cv::Mat& src) {
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            uchar pixel = src.at<uchar>(i, j);
            if (pixel != 0 && pixel != 255) {
                return false;
            }
        }
    }
    return true;
}

cv::Mat labelConnectedComponents(const cv::Mat& src, int minSize, cv::Mat& stats) {
    cv::Mat labels, centroids, invertedSrc;

    // Check if src is empty
    if (src.empty()) {
        std::cerr << "Error: The source image is empty!" << std::endl;
        return labels;
    }

    if (src.channels() != 1) {
        std::cerr << "Error: The source image is not single-channel." << std::endl;
        return labels;
    }

    if (!isBinaryImage(src)) {
        std::cerr << "Error: The source image is not binary." << std::endl;
        return labels;
    }

    // Invert the binary image
    cv::bitwise_not(src, invertedSrc);

    // Connected components analysis
    int nLabels = cv::connectedComponentsWithStats(invertedSrc, labels, stats, centroids);

    // Check boundaries
    if (nLabels > stats.rows || nLabels > centroids.rows) {
        std::cerr << "Error: Mismatch between nLabels and stats/centroids rows." << std::endl;
        return labels;
    }

    for (int i = 1; i < nLabels; i++) {
        if (stats.at<int>(i, cv::CC_STAT_AREA) < minSize) {
            for (int j = 0; j < src.rows; j++) {
                for (int k = 0; k < src.cols; k++) {
                    if (labels.at<int>(j, k) == i) {
                        labels.at<int>(j, k) = 0;  // set the label to 0 (background) for regions below minSize
                    }
                }
            }
        }
    }
    return labels;
}

cv::Mat colorConnectedComponents(const cv::Mat& labels) {
    // A vector to hold the colors for each label
    std::vector<cv::Vec3b> colors;

    // Find the maximum label value in the labels image.
    double minVal, maxVal;
    cv::minMaxLoc(labels, &minVal, &maxVal);

    // Generate a random color for each label
    for (int i = 0; i <= maxVal; i++) {
        colors.push_back(cv::Vec3b(rand() & 255, rand() & 255, rand() & 255));
    }

    // Set the color for the background (using top-left pixel as reference for background) to black
    int backgroundLabel = labels.at<int>(0, 0);
    colors[backgroundLabel] = cv::Vec3b(0, 0, 0);

    // Create a new image to store the colored version of the labels image
    cv::Mat colored = cv::Mat(labels.size(), CV_8UC3);

    // Populate the colored image using the colors vector
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            int label = labels.at<int>(i, j);
            colored.at<cv::Vec3b>(i, j) = colors[label];
        }
    }

    return colored;
}



int main() {
    // Open the default camera
    cv::VideoCapture cap(0);
    cv::waitKey(2000);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    // adjust the exposure and brightness of the external camera
    cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 0);
    cap.set(cv::CAP_PROP_EXPOSURE, 3);      
    cap.set(cv::CAP_PROP_BRIGHTNESS, 70);

    while (true) {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "Error: Could not read frame from camera." << std::endl;
            break;
        }

        cv::Mat thresholded;
        dynamicThresholding(frame, thresholded);

        // Debug output
        std::cout << "Number of channels in 'thresholded' image: " << thresholded.channels() << std::endl;

        cv::Mat cleaned;
        applyMorphologicalOperations(thresholded, cleaned);

        // Ensure the cleaned image is strictly binary
        cv::threshold(cleaned, cleaned, 127, 255, cv::THRESH_BINARY);

        // Debug output
        std::cout << "Number of channels in 'cleaned' image: " << cleaned.channels() << std::endl;

        cv::Mat stats;
        cv::Mat labels = labelConnectedComponents(cleaned, 500, stats); // 500 is a sample minimum size threshold
        cv::Mat coloredLabels = colorConnectedComponents(labels);

        // Displaying the original, thresholded, morphological and segmented frames
        displayImages(frame, thresholded, cleaned, coloredLabels);

        int key = cv::waitKey(100);
        if (key == 's' || key == 'S') {
            displayAndOptionallySave(frame, thresholded, cleaned, coloredLabels);
        }
        else if (cv::getWindowProperty("Original", cv::WND_PROP_VISIBLE) < 1 ||
            cv::getWindowProperty("Thresholded", cv::WND_PROP_VISIBLE) < 1 ||
            cv::getWindowProperty("Cleaned", cv::WND_PROP_VISIBLE) < 1) {

            int msgboxID = MessageBox(
                NULL,
                L"Do you want to save the frames?",
                L"Save Frames",
                MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2
            );

            if (msgboxID == IDYES) {
                displayAndOptionallySave(frame, thresholded, cleaned, coloredLabels);
            }

            break;
        }

    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}

/***
int main(int argc, char* argv[]) {
    // Get list of selected image paths
    std::vector<std::string> filePaths = openFileDialog();

    for (const std::string& path : filePaths) {
        // Read the image from the path
        cv::Mat original = cv::imread(path);
        if (original.empty()) {
            std::cout << "Failed to load image from path: " << path << std::endl;
            continue;
        }

        // Threshold the image
        cv::Mat thresholded;
        thresholdImage(original, thresholded, 100); // you can change the threshold value

        // Display original and thresholded images
        displayAndOptionallySave(original, thresholded);
    }

    return 0;
}
***/
