#include "nii_processing.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkOpenCVImageBridge.h"

using namespace itk;
using namespace cv;

using VolumeType = Image<float, 3>;
using SliceType = Image<float, 2>;
using MaskType = Image<unsigned char, 2>;

Mat processSlice(VolumeType::Pointer volume, unsigned int sliceIndex) {
    // Extraer slice axial
    auto region = volume->GetLargestPossibleRegion();
    SliceType::Pointer slice = SliceType::New();

    SliceType::SizeType size = { region.GetSize()[0], region.GetSize()[1] };
    SliceType::IndexType start = { 0, 0 };
    SliceType::RegionType sliceRegion;
    sliceRegion.SetSize(size);
    sliceRegion.SetIndex(start);
    slice->SetRegions(sliceRegion);
    slice->Allocate();



    for (int y = 0; y < size[1]; ++y) {
        for (int x = 0; x < size[0]; ++x) {
            Index<3> idx3D = { x, y, sliceIndex };
            Index<2> idx2D = { x, y };
            slice->SetPixel(idx2D, volume->GetPixel(idx3D));
        }
    }

    // Resaltar bordes con gradiente
    using GradientFilterType = GradientMagnitudeImageFilter<SliceType, SliceType>;
    auto gradientFilter = GradientFilterType::New();
    gradientFilter->SetInput(slice);
    gradientFilter->Update();
    SliceType::Pointer gradientImage = gradientFilter->GetOutput();

    // Umbralización binaria
    using BinaryThresholdFilterType = BinaryThresholdImageFilter<SliceType, MaskType>;
    auto threshFilter = BinaryThresholdFilterType::New();
    threshFilter->SetInput(gradientImage);
    threshFilter->SetLowerThreshold(10);
    threshFilter->SetUpperThreshold(255);
    threshFilter->SetInsideValue(255);
    threshFilter->SetOutsideValue(0);
    threshFilter->Update();
    MaskType::Pointer binaryMask = threshFilter->GetOutput();

    // Operación morfológica (erosión)
    using StructuringElementType = BinaryBallStructuringElement<unsigned char, 2>;
    using ErodeFilterType = BinaryErodeImageFilter<MaskType, MaskType, StructuringElementType>;
    StructuringElementType structElem;
    structElem.SetRadius(1);
    structElem.CreateStructuringElement();

    auto erodeFilter = ErodeFilterType::New();
    erodeFilter->SetInput(binaryMask);
    erodeFilter->SetKernel(structElem);
    erodeFilter->SetForegroundValue(255);
    erodeFilter->Update();

    MaskType::Pointer cleanedMask = erodeFilter->GetOutput();

    // Convertir máscara a cv::Mat
    cv::Mat cvMask = itk::OpenCVImageBridge::ITKImageToCVMat<MaskType>(cleanedMask.GetPointer());

    return cvMask;
}
