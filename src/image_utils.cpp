//
// Created by eduardo on 22/04/25.
//

#include "image_utils.h"
#include <itkExtractImageFilter.h>
#include <itkImageRegionConstIterator.h>

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
