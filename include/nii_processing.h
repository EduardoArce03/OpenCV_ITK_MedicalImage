//
// Created by eduardo on 30/05/25.
//

#ifndef NII_PROCESSING_H
#define NII_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <itkImage.h>
using namespace itk;
using namespace cv;

using SliceType = Image<float,2>;
Mat processSlice(Image<float, 3>::Pointer volume, unsigned int sliceIndex);
Mat extractMaskSlice(itk::Image<unsigned char, 3>::Pointer maskVolume, unsigned int sliceIndex);
Mat overlayMask(const Mat& image, const Mat& mask, double alpha=0.4);
#endif //NII_PROCESSING_H
