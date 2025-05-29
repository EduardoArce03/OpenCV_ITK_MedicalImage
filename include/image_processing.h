#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <string>

// Solo definimos tipos ITK si no se definen antes
// (Esto evita redefinición si ya se incluyó itkImage.h)

// Si 'itk' no está definido, incluimos el header necesario
#ifndef ITK_FORWARD_DECL
#include <itkImage.h>
#endif

// === TIPOS DE IMAGENES ITK ===
using ImageType3D = itk::Image<float, 3>;
using ImageType2D = itk::Image<float, 2>;

// === FUNCIONES ===
ImageType2D::Pointer ExtractSlice(ImageType3D::Pointer image, int sliceIndex);
cv::Mat ITKToMat(ImageType2D::Pointer image);

std::vector<cv::Mat> applyLightingFilters(const cv::Mat& gray);
cv::Mat addGaussianNoise(const cv::Mat& image, int mean, int std);
cv::Mat addSpeckleNoise(const cv::Mat& image, float var);
std::vector<cv::Mat> applySmoothing(const cv::Mat& gray, int k = 5);
std::vector<cv::Mat> applyEdges(const cv::Mat& image, bool smooth = false, int k = 5);
std::map<std::string, cv::Mat> applyMorphologicalOps(const cv::Mat& binary,
                                                    const std::vector<int>& kernelSizes);

#endif // IMAGE_PROCESSING_H