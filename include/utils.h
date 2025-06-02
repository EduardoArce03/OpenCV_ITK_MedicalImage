#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

void ensureDirectoryExists(const std::string& path);
void saveProcessed(cv::Mat img, const std::string& folder, const std::string& filename);
cv::Mat hconcatImages(const std::vector<cv::Mat>& images);
cv::Mat vconcatImages(const std::vector<cv::Mat>& images);

// Superponer una máscara sobre una imagen base con transparencia
cv::Mat overlayMaskOnBase(const cv::Mat& base, const cv::Mat& mask, double alpha = 0.5);

#endif // UTILS_H
