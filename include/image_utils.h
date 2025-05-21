//
// Created by eduardo on 22/04/25.
//

#ifndef IMAGE_UTILSS_H
#define IMAGE_UTILSS_H



#include <opencv2/opencv.hpp>
#include <itkImage.h>

// Tipos
using ImageType3D = itk::Image<float, 3>;
using ImageType2D = itk::Image<float, 2>;

// Declaraciones
ImageType2D::Pointer ExtractSlice(ImageType3D::Pointer image, int sliceIndex);
cv::Mat ITKToMat(ImageType2D::Pointer image);

#endif