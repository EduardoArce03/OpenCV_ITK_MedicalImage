#include <itkImage.h>
#include <itkExtractImageFilter.h>
#include <opencv2/opencv.hpp>
#include "image_processing.h"
#include <iostream>

ImageType2D::Pointer ExtractSlice(ImageType3D::Pointer image, int sliceIndex) {
    itk::ImageRegion<3> region = image->GetLargestPossibleRegion();
    itk::Size<3> size = region.GetSize();
    itk::Index<3> start = region.GetIndex();

    size[2] = 0; // Solo un slice
    start[2] = sliceIndex;

    itk::ImageRegion<3> desiredRegion;
    desiredRegion.SetSize(size);
    desiredRegion.SetIndex(start);

    using ExtractorType = itk::ExtractImageFilter<ImageType3D, ImageType2D>;
    auto extractor = ExtractorType::New();
    extractor->SetExtractionRegion(desiredRegion);
    extractor->SetInput(image);
    extractor->SetDirectionCollapseToSubmatrix();
    extractor->Update();

    return extractor->GetOutput();
}

cv::Mat ITKToMat(ImageType2D::Pointer image) {
    itk::ImageRegion<2> region = image->GetLargestPossibleRegion();
    itk::Size<2> size = region.GetSize();

    cv::Mat mat(size[1], size[0], CV_32FC1);

    itk::ImageRegionConstIterator<ImageType2D> it(image, region);
    for (int y = 0; y < size[1]; ++y)
        for (int x = 0; x < size[0]; ++x, ++it)
            mat.at<float>(y, x) = it.Get();

    return mat;
}