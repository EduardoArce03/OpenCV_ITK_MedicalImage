//
// Created by eduardo on 22/04/25.
//

#include "image_processing.h"
#include <itkExtractImageFilter.h>
#include <itkImageRegionConstIterator.h>
#include <vector>
#include <opencv4/opencv2/opencv.hpp>

ImageType2D::Pointer ExtractSlice(ImageType3D::Pointer image, int sliceIndex) {
    ImageType3D::RegionType region = image->GetLargestPossibleRegion();
    ImageType3D::SizeType size = region.GetSize();
    size[2] = 0; // Solo 1 slice

    ImageType3D::IndexType start = region.GetIndex();
    start[2] = sliceIndex;

    ImageType3D::RegionType desiredRegion;
    desiredRegion.SetSize(size);
    desiredRegion.SetIndex(start);

    auto extractor = itk::ExtractImageFilter<ImageType3D, ImageType2D>::New();
    extractor->SetExtractionRegion(desiredRegion);
    extractor->SetInput(image);
    extractor->SetDirectionCollapseToSubmatrix();
    extractor->Update();

    return extractor->GetOutput();
}

cv::Mat ITKToMat(ImageType2D::Pointer image) {
    auto region = image->GetLargestPossibleRegion();
    auto size = region.GetSize();
    cv::Mat mat(size[1], size[0], CV_32FC1);

    itk::ImageRegionConstIterator<ImageType2D> it(image, region);
    for (int y = 0; y < size[1]; ++y)
        for (int x = 0; x < size[0]; ++x, ++it)
            mat.at<float>(y, x) = it.Get();

    return mat;
}


// APLICAR FILTROS DE ILUMINACION
std::vector<cv::Mat> applyLightingFilters(const cv::Mat& gray) {
    std::vector<cv::Mat> results;

    // Histogram Equalization
    cv::Mat eq_hist;
    cv::equalizeHist(gray, eq_hist);
    cv::cvtColor(eq_hist, eq_hist, cv::COLOR_GRAY2BGR);
    cv::putText(eq_hist, "Hist. Equal.", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
    results.push_back(eq_hist);

    // CLAHE
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(2.0);
    clahe->setTilesGridSize(cv::Size(8, 8));
    cv::Mat clahed;
    clahe->apply(gray, clahed);
    cv::cvtColor(clahed, clahed, cv::COLOR_GRAY2BGR);
    cv::putText(clahed, "CLAHE", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
    results.push_back(clahed);

    // Gamma Correction
    cv::Mat gamma_corrected;
    gray.convertTo(gamma_corrected, CV_32F, 1.0 / 255.0);
    cv::pow(gamma_corrected, 1.0 / 1.5, gamma_corrected);
    gamma_corrected *= 255.0;
    gamma_corrected.convertTo(gamma_corrected, CV_8U);
    cv::cvtColor(gamma_corrected, gamma_corrected, cv::COLOR_GRAY2BGR);
    cv::putText(gamma_corrected, "Gamma Corr.", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
    results.push_back(gamma_corrected);

    return results;
}


// AÑADIR RUIDO GAUSSIANO
cv::Mat addGaussianNoise(const cv::Mat& image, int mean, int std) {
    cv::Mat noise = cv::Mat::zeros(image.size(), CV_16S);
    cv::randn(noise, mean, std);
    cv::Mat noisy = image + noise;
    cv::normalize(noisy, noisy, 0, 255, cv::NORM_MINMAX, CV_8U);

    cv::putText(noisy, "Gauss Noise µ=" + std::to_string(mean) + " σ=" + std::to_string(std),
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
    return noisy;
}

//AÑADIR RUIDO SPECKLE

cv::Mat addSpeckleNoise(const cv::Mat& image, float var) {
    cv::Mat noise = cv::Mat::zeros(image.size(), CV_32F);
    cv::randu(noise, 0.0, 1.0);
    cv::Mat noisy = image.clone();
    for (int y = 0; y < image.rows; ++y)
        for (int x = 0; x < image.cols; ++x)
            noisy.at<uchar>(y, x) = cv::saturate_cast<uchar>(
                image.at<uchar>(y, x) * (1.0 + var * noise.at<float>(y, x))
            );

    cv::putText(noisy, "Speckle Noise var=" + std::to_string(var).substr(0, 4),
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
    return noisy;
}

// APLICAR FILTROS Y SUAVIZADOS

std::vector<cv::Mat> applySmoothing(const cv::Mat& gray, int k) {
    if (k % 2 == 0) k += 1;
    k = std::max(k, 3);

    cv::Mat blur, gaussian, median;
    cv::blur(gray, blur, cv::Size(k, k));
    cv::GaussianBlur(gray, gaussian, cv::Size(k, k), 0);
    cv::medianBlur(gray, median, k);

    // Convertir a color y etiquetar
    cv::cvtColor(blur, blur, cv::COLOR_GRAY2BGR);
    cv::cvtColor(gaussian, gaussian, cv::COLOR_GRAY2BGR);
    cv::cvtColor(median, median, cv::COLOR_GRAY2BGR);

    cv::putText(blur, "Blur " + std::to_string(k) + "x" + std::to_string(k),
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
    cv::putText(gaussian, "Gaussian " + std::to_string(k) + "x" + std::to_string(k),
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
    cv::putText(median, "Median " + std::to_string(k) + "x" + std::to_string(k),
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);

    return {blur, gaussian, median};
}

// DETECCION DE BORDES (CANNY Y SOBEL)
std::vector<cv::Mat> applyEdges(const cv::Mat& image, bool smooth, int k) {
    cv::Mat gray;
    if (image.channels() > 1)
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else
        gray = image.clone();

    if (smooth)
        cv::GaussianBlur(gray, gray, cv::Size(k, k), 0);

    cv::Mat sobelX, sobelY, mag;

    cv::Sobel(gray, sobelX, CV_64F, 1, 0, 3);
    cv::Sobel(gray, sobelY, CV_64F, 0, 1, 3);
    cv::magnitude(sobelX, sobelY, mag);

    // ✅ CORRECCIÓN: Convierte a tipo mostrable
    cv::Mat mag8u;
    mag.convertTo(mag8u, CV_8U);

    // Opcional: Agregar etiqueta
    cv::putText(mag8u, "Sobel", cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);

    return {mag8u};
}

// APLICACIONES MORFOLOGICAS

std::map<std::string, cv::Mat> applyMorphologicalOps(const cv::Mat& binary, const std::vector<int>& kernelSizes) {
    std::map<std::string, cv::Mat> results;

    for (int size : kernelSizes) {
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(size, size));

        cv::Mat erosion, dilation, tophat, blackhat, combined;
        cv::erode(binary, erosion, kernel);
        cv::dilate(binary, dilation, kernel);
        cv::morphologyEx(binary, tophat, cv::MORPH_TOPHAT, kernel);
        cv::morphologyEx(binary, blackhat, cv::MORPH_BLACKHAT, kernel);
        combined = binary + (tophat - blackhat);

        results["erosion_" + std::to_string(size)] = erosion;
        results["dilation_" + std::to_string(size)] = dilation;
        results["tophat_" + std::to_string(size)] = tophat;
        results["blackhat_" + std::to_string(size)] = blackhat;
        results["combined_" + std::to_string(size)] = combined;
    }

    return results;
}