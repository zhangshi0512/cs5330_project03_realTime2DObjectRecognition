// main.cpp

#include <iostream>
#include <fstream>
#include <string>
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

// function to compute features
// Structure to hold region features
struct RegionFeatures {
    cv::RotatedRect orientedBoundingBox;
    cv::Vec2f axisOfLeastMoment;
    float percentFilled;
    float bboxAspectRatio;
};

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

    }
    catch (const cv::Exception& e) {
        std::cerr << "Error computing features for region " << regionID << ": " << e.what() << std::endl;
    }

    return features;
}

// Function to hold training database
// Data structure to hold feature vectors and labels
struct ObjectData {
    std::string label;
    RegionFeatures features;
};

std::vector<ObjectData> objectDB;

void saveObjectDB(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::out);
    if (!outFile) {
        std::cerr << "Error: Couldn't open file for writing: " << filename << std::endl;
        return;
    }

    // Write the number of objects
    outFile << objectDB.size() << std::endl;

    for (const ObjectData& data : objectDB) {
        outFile << data.label << " ";

        // Serialize the features:
        outFile << data.features.orientedBoundingBox.center.x << " "
            << data.features.orientedBoundingBox.center.y << " "
            << data.features.orientedBoundingBox.size.width << " "
            << data.features.orientedBoundingBox.size.height << " "
            << data.features.orientedBoundingBox.angle << " "
            << data.features.axisOfLeastMoment[0] << " "
            << data.features.axisOfLeastMoment[1] << " "
            << data.features.percentFilled << " "
            << data.features.bboxAspectRatio << std::endl;
    }

    outFile.close();
    std::cout << "Saved object data to: " << filename << std::endl;
}

// helper function for user to attach a label
/**
std::string getLabelFromUser() {
    char input[100];
    std::cout << "Enter the label for the object: ";
    std::cin.getline(input, sizeof(input));
    return std::string(input);
}
**/

// function to load object database from a csv file
bool loadObjectDBFromCSV(const std::string& filename, std::vector<ObjectData>& objectDB) {
    std::ifstream inFile(filename, std::ios::in);
    if (!inFile) {
        std::cerr << "Error: Couldn't open file for reading: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        ObjectData data;

        std::getline(ss, data.label, ',');  // Get the label

        // Deserialize the features:
        ss >> data.features.orientedBoundingBox.center.x >> data.features.orientedBoundingBox.center.y
            >> data.features.orientedBoundingBox.size.width >> data.features.orientedBoundingBox.size.height
            >> data.features.orientedBoundingBox.angle >> data.features.axisOfLeastMoment[0]
            >> data.features.axisOfLeastMoment[1] >> data.features.percentFilled
            >> data.features.bboxAspectRatio;

        objectDB.push_back(data);
    }

    inFile.close();
    std::cout << "Loaded object data from: " << filename << std::endl;
    return true;
}

// Function to define the scaled Euclidean distance metric
float scaledEuclideanDistance(const RegionFeatures& f1, const RegionFeatures& f2) {
    float stdev_orientedBoundingBox_center_x = 1.0;
    float stdev_orientedBoundingBox_center_y = 1.0;
    float stdev_orientedBoundingBox_size_width = 1.0;
    float stdev_orientedBoundingBox_size_height = 1.0;
    float stdev_orientedBoundingBox_angle = 1.0;
    float stdev_axisOfLeastMoment_0 = 1.0;
    float stdev_axisOfLeastMoment_1 = 1.0;
    float stdev_percentFilled = 1.0;
    float stdev_bboxAspectRatio = 1.0;

    float distance = 0.0;

    float distance_center_x = std::pow((f1.orientedBoundingBox.center.x - f2.orientedBoundingBox.center.x) / stdev_orientedBoundingBox_center_x, 2);
    float distance_center_y = std::pow((f1.orientedBoundingBox.center.y - f2.orientedBoundingBox.center.y) / stdev_orientedBoundingBox_center_y, 2);
    float distance_width = std::pow((f1.orientedBoundingBox.size.width - f2.orientedBoundingBox.size.width) / stdev_orientedBoundingBox_size_width, 2);
    float distance_height = std::pow((f1.orientedBoundingBox.size.height - f2.orientedBoundingBox.size.height) / stdev_orientedBoundingBox_size_height, 2);
    float distance_angle = std::pow((f1.orientedBoundingBox.angle - f2.orientedBoundingBox.angle) / stdev_orientedBoundingBox_angle, 2);
    float distance_axis_0 = std::pow((f1.axisOfLeastMoment[0] - f2.axisOfLeastMoment[0]) / stdev_axisOfLeastMoment_0, 2);
    float distance_axis_1 = std::pow((f1.axisOfLeastMoment[1] - f2.axisOfLeastMoment[1]) / stdev_axisOfLeastMoment_1, 2);
    float distance_filled = std::pow((f1.percentFilled - f2.percentFilled) / stdev_percentFilled, 2);
    float distance_aspect = std::pow((f1.bboxAspectRatio - f2.bboxAspectRatio) / stdev_bboxAspectRatio, 2);

    distance = distance_center_x + distance_center_y + distance_width + distance_height +
        distance_angle + distance_axis_0 + distance_axis_1 + distance_filled + distance_aspect;

    // Debug prints
    std::cout << "center_x distance: " << distance_center_x << std::endl;
    std::cout << "center_y distance: " << distance_center_y << std::endl;
    std::cout << "width distance: " << distance_width << std::endl;
    std::cout << "height distance: " << distance_height << std::endl;
    std::cout << "angle distance: " << distance_angle << std::endl;
    std::cout << "axis_0 distance: " << distance_axis_0 << std::endl;
    std::cout << "axis_1 distance: " << distance_axis_1 << std::endl;
    std::cout << "percent filled distance: " << distance_filled << std::endl;
    std::cout << "aspect ratio distance: " << distance_aspect << std::endl;

    return std::sqrt(distance);
}


// Function to compare new object's feature vector to existing database using the distance metric
const float RECOGNITION_THRESHOLD = 10.0; // This is a placeholder value. You might need to adjust it based on your data.

std::string classifyObject(const RegionFeatures& features) {
    std::cout << "Classifying object..." << std::endl;
    float minDistance = std::numeric_limits<float>::max();
    std::string bestMatchLabel = "Unknown";

    for (const ObjectData& data : objectDB) {
        float distance = scaledEuclideanDistance(features, data.features);
        std::cout << "Distance to " << data.label << ": " << distance << std::endl;
        if (distance < minDistance) {
            minDistance = distance;
            bestMatchLabel = data.label;
        }
    }

    return bestMatchLabel;
}


int main() {
    // initialize the objectDB vector
    std::vector<ObjectData> objectDB;

    // load the existing database from a local csv:
    if (loadObjectDBFromCSV("C:/Users/Shi Zhang/My Drive/CS/NEU Align/Courses/2023 Fall/5330/Project03/img_database.csv", objectDB)) {
        std::cout << "Number of objects in database: " << objectDB.size() << std::endl;
    }
    else {
        std::cerr << "Failed to load object database." << std::endl;
        return -1; // Exit if we can't load the database
    }

    // Open the default camera
    cv::VideoCapture cap(0);
    cv::waitKey(2000);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    // Adjust the exposure and brightness of the external camera
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

        cv::Mat cleaned;
        applyMorphologicalOperations(thresholded, cleaned);
        cv::threshold(cleaned, cleaned, 127, 255, cv::THRESH_BINARY);

        cv::Mat stats;
        cv::Mat labels = labelConnectedComponents(cleaned, 500, stats);
        cv::Mat coloredLabels = colorConnectedComponents(labels);

        cv::Mat visualization = coloredLabels.clone();
        for (int i = 1; i < stats.rows; i++) {  // Starting from 1 to skip the background
            std::cout << "Processing region: " << i << std::endl;
            RegionFeatures features = computeRegionFeatures(labels, i);

            // add label on top of the image
            int x = static_cast<int>(features.orientedBoundingBox.center.x);
            int y = static_cast<int>(features.orientedBoundingBox.center.y);

            int textOffsetY = 10;
            int textOffsetX = 40; // Increase this if necessary, depending on the average width of the text
            if (y - textOffsetY < 0) {
                y = textOffsetY; // Reset y to the offset if it's too close to the top
            }
            if (x - textOffsetX < 0) {
                x = textOffsetX; // Reset x to the offset if it's too close to the left edge
            }

            std::string label = classifyObject(features);
            cv::putText(visualization, label, cv::Point(x - textOffsetX, y - textOffsetY), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

            // Draw oriented bounding box
            cv::Point2f vertices[4];
            features.orientedBoundingBox.points(vertices);
            for (int j = 0; j < 4; j++) {
                cv::line(visualization, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 0), 2);
            }

            // Draw axis of least moment (as a line centered on the bounding box's center)
            cv::Point2f center = features.orientedBoundingBox.center;
            // Draw in both directions from the center
            cv::Point2f endpoint = center + 100 * cv::Point2f(features.axisOfLeastMoment[0], features.axisOfLeastMoment[1]); 
            cv::Point2f startpoint = center - 100 * cv::Point2f(features.axisOfLeastMoment[0], features.axisOfLeastMoment[1]); 
            cv::line(visualization, startpoint, endpoint, cv::Scalar(0, 0, 255), 2);
        }

        // Displaying the images
        displayImages(frame, thresholded, cleaned, visualization);

        // Inform the user about possible actions
        std::cout << "Press 'n' to label the object, 's' to save the images, 'w' to save the database, or 'q' to quit." << std::endl;

        // Wait for a user keypress
        int key = cv::waitKey(0);  // 0 means wait indefinitely until a key is pressed

        // Handle the keypress
        if (key == 'n' || key == 'N') {
            std::string label;
            std::cout << "Enter the label for the current object: ";
            std::cin >> label;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear the newline from the buffer

            // storing features from the lasrgest region
            int largestArea = 0;
            int largestLabel = 0;
            for (int i = 1; i < stats.rows; i++) {
                int area = stats.at<int>(i, cv::CC_STAT_AREA);
                if (area > largestArea) {
                    largestArea = area;
                    largestLabel = i;
                }
            }

            RegionFeatures features = computeRegionFeatures(labels, largestLabel);
            objectDB.push_back({ label, features });
            std::cout << "Stored features for object labeled: " << label << std::endl;
        }
        else if (key == 's' || key == 'S') {
            displayAndOptionallySave(frame, thresholded, cleaned, visualization);
        }
        else if (key == 'w' || key == 'W') {
            saveObjectDB("C:/Users/Shi Zhang/My Drive/CS/NEU Align/Courses/2023 Fall/5330/Project03/img_database_single.txt");
        }
        else if (key == 'q' || key == 'Q' || key == 27) {  // 27 is the ESC key
            break;  // Exit the loop
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
