#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// Asegurar que una carpeta exista
void ensureDirectoryExists(const std::string& path);

// Guardar imagen procesada
void saveProcessed(cv::Mat img, const std::string& folder, const std::string& filename);

// Redimensionar y concatenar imágenes
cv::Mat hconcatImages(const std::vector<cv::Mat>& images);
cv::Mat vconcatImages(const std::vector<cv::Mat>& images);

#endif // UTILS_H