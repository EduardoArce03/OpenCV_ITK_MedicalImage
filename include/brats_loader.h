//
// Created by eduardo on 28/05/25.
//

#ifndef NII_LOADER_H
#define NII_LOADER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <itkImage.h>

using namespace cv;
using namespace std;
using namespace itk;
using VolumeImageType = Image<float, 3>;
using MaskImageType = Image<unsigned char, 3>;
struct BratsData {
    map<string, VolumeImageType::Pointer> modalities;
    MaskImageType::Pointer segmentation;
};

BratsData loadBratsCase(const string& patientFolder);

#endif //NII_LOADER_H
