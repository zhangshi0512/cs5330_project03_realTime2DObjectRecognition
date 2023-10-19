// main.cpp

#include <iostream>
#include <windows.h>
#include <vector>
#include "thresholding.h"
#include "display.h"
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

void displayAndOptionallySave(const cv::Mat& original, const cv::Mat& thresholded) {
    displayImages(original, thresholded);

    int userChoice = MessageBox(NULL, L"Would you like to save the thresholded image?", L"Save Image", MB_YESNO);

    if (userChoice == IDYES) {
        std::wstring savePath;
        if (saveFileDialog(savePath)) {
            if (!cv::imwrite(std::string(savePath.begin(), savePath.end()), thresholded)) {
                MessageBox(NULL, L"Error: Could not save the image to the specified path.", L"Error", MB_OK);
            }
            else {
                MessageBox(NULL, L"Image saved successfully!", L"Success", MB_OK);
            }
        }
    }
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
    cap.set(cv::CAP_PROP_EXPOSURE, 0);      
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

        displayImages(frame, thresholded);  // Displaying the original and thresholded frames

        // Check if 's' key is pressed to save the frame
        int key = cv::waitKey(100);
        if (key == 's' || key == 'S') {
            std::wstring savePath;
            if (saveFileDialog(savePath)) {
                std::cout << "Attempting to save image to: " << std::string(savePath.begin(), savePath.end()) << std::endl;
                if (!cv::imwrite(std::string(savePath.begin(), savePath.end()), frame)) {
                    std::cerr << "Error: Could not save the image to the specified path." << std::endl;
                    MessageBox(NULL, L"Error: Could not save the image to the specified path.", L"Error", MB_OK);
                }
                else {
                    std::cout << "Image saved successfully!" << std::endl;
                    MessageBox(NULL, L"Image saved successfully!", L"Success", MB_OK);
                }
            }
            else {
                std::cerr << "Error: Save dialog cancelled or encountered an error." << std::endl;
            }
        }
        // Check if display window is closed
        else if (cv::getWindowProperty("Original", cv::WND_PROP_VISIBLE) < 1) {
            int msgboxID = MessageBox(
                NULL,
                L"Do you want to save the frames?",
                L"Save Frames",
                MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2
            );

            switch (msgboxID) {
            case IDYES:
            {
                std::wstring savePathOriginal, savePathThresholded;
                if (saveFileDialog(savePathOriginal)) {
                    cv::imwrite(std::string(savePathOriginal.begin(), savePathOriginal.end()), frame);
                }
                if (saveFileDialog(savePathThresholded)) {
                    cv::imwrite(std::string(savePathThresholded.begin(), savePathThresholded.end()), thresholded);
                }
            }
            break;
            case IDNO:
                break;
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
