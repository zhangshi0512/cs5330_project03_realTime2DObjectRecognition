#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include <opencv2/opencv.hpp>
#include "features.h"

void train(const Features& f, const std::string& label);
std::string classify(const Features& f);

#endif // CLASSIFICATION_H
