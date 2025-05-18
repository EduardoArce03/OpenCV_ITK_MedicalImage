#include "utils.h"

void ensureDirectoryExists(const std::string& path) {
    if (!fs::exists(path)) {
        fs::create_directories(path);
    }
}

void saveProcessed(cv::Mat img, const std::string& folder, const std::string& filename) {
    ensureDirectoryExists(folder);
    std::string fullpath = folder + "/" + filename;
    cv::imwrite(fullpath, img);
}

void resizeToFit(cv::Mat& src, cv::Mat& dst, int maxSize) {
    double scale = std::min(1.0, static_cast<double>(maxSize) / std::max(src.cols, src.rows));
    if (scale < 1.0)
        cv::resize(src, dst, cv::Size(), scale, scale);
    else
        dst = src.clone();
}

void toColorIfNeeded(cv::Mat& src, cv::Mat& dst) {
    if (src.channels() == 1)
        cv::cvtColor(src, dst, cv::COLOR_GRAY2BGR);
    else
        dst = src.clone();
}

cv::Mat hconcatImages(const std::vector<cv::Mat>& images) {
    if (images.empty()) return cv::Mat();
    std::vector<cv::Mat> resized;
    for (const auto& img : images) {
        cv::Mat temp;
        resizeToFit(img, temp, 800); // Ajustar al ancho máximo
        resized.push_back(temp);
    }
    cv::Mat result;
    cv::hconcat(resized.data(), resized.size(), result);
    return result;
}

cv::Mat vconcatImages(const std::vector<cv::Mat>& images) {
    if (images.empty()) return cv::Mat();
    std::vector<cv::Mat> resized;
    int targetWidth = 0;
    for (const auto& img : images) {
        cv::Mat temp;
        resizeToFit(img, temp, 800);
        targetWidth = std::max(targetWidth, temp.cols);
        resized.push_back(temp);
    }

    cv::Mat result;
    for (auto& img : resized) {
        cv::Mat temp;
        if (img.cols != targetWidth) {
            cv::resize(img, temp, cv::Size(targetWidth, img.rows));
        } else {
            temp = img;
        }
        if (result.empty())
            temp.copyTo(result);
        else
            cv::vconcat(result, temp, result);
    }
    return result;
}