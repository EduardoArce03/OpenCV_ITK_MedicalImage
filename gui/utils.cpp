#include "utils.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

// Asegura que la carpeta de salida exista
void ensureDirectoryExists(const std::string& path) {
    if (!fs::exists(path)) {
        fs::create_directories(path);
    }
}

// Guarda la imagen procesada en la carpeta especificada
void saveProcessed(cv::Mat img, const std::string& folder, const std::string& filename) {
    ensureDirectoryExists(folder);
    std::string fullPath = folder + "/" + filename;
    cv::imwrite(fullPath, img);
}

// Concatena imágenes horizontalmente
cv::Mat hconcatImages(const std::vector<cv::Mat>& images) {
    if (images.empty()) return {};
    cv::Mat result;
    cv::hconcat(images, result);
    return result;
}

// Concatena imágenes verticalmente
cv::Mat vconcatImages(const std::vector<cv::Mat>& images) {
    if (images.empty()) return {};
    cv::Mat result;
    cv::vconcat(images, result);
    return result;
}

// Aplica una máscara sobre una imagen base
cv::Mat overlayMaskOnBase(const cv::Mat& base, const cv::Mat& mask, double alpha) {
    CV_Assert(base.size() == mask.size());
    CV_Assert(base.type() == CV_8UC1 || base.type() == CV_8UC3);
    CV_Assert(mask.type() == CV_8UC1);

    // Convertir imagen base a color si está en escala de grises
    cv::Mat baseColor;
    if (base.channels() == 1)
        cv::cvtColor(base, baseColor, cv::COLOR_GRAY2BGR);
    else
        baseColor = base.clone();

    // Crear imagen de la máscara en color (rojo)
    cv::Mat maskColor(base.size(), CV_8UC3, cv::Scalar(0, 0, 255));
    cv::Mat maskBinary;
    cv::threshold(mask, maskBinary, 1, 255, cv::THRESH_BINARY);

    // Combinación
    cv::Mat overlay = baseColor.clone();
    maskColor.copyTo(overlay, maskBinary);

    // Mezclar con la imagen base original
    cv::Mat blended;
    cv::addWeighted(overlay, alpha, baseColor, 1.0 - alpha, 0.0, blended);

    return blended;
}
