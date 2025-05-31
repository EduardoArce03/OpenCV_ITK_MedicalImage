#include "nii_processing.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkOpenCVImageBridge.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkBinaryDilateImageFilter.h"

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
    threshFilter->SetLowerThreshold(20);
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

cv::Mat extractMaskSlice(itk::Image<unsigned char, 3>::Pointer maskVolume, unsigned int sliceIndex) {
    using MaskSliceType = itk::Image<unsigned char, 2>;

    auto region = maskVolume->GetLargestPossibleRegion();

    MaskSliceType::Pointer slice = MaskSliceType::New();

    MaskSliceType::SizeType size = {region.GetSize()[0], region.GetSize()[1]};
    MaskSliceType::IndexType start = {0, 0};
    MaskSliceType::RegionType sliceRegion;
    sliceRegion.SetIndex(start);
    sliceRegion.SetSize(size);
    slice->SetRegions(sliceRegion);
    slice->Allocate();

    for (int y = 0; y < static_cast<int>(size[1]); ++y) {
        for (int x = 0; x < static_cast<int>(size[0]); ++x) {
            itk::Index<3> idx3D = {x, y, sliceIndex};
            itk::Index<2> idx2D = {x, y};
            slice->SetPixel(idx2D, maskVolume->GetPixel(idx3D));
        }
    }

    // Ya es uchar, listo para convertir
    cv::Mat cvMask = itk::OpenCVImageBridge::ITKImageToCVMat(slice.GetPointer());


    return cvMask;
}

cv::Mat overlayMask(const cv::Mat& image, const cv::Mat& mask, double alpha) {
    cv::Mat colorImage;
    if (image.channels() == 1)
        cv::cvtColor(image, colorImage, cv::COLOR_GRAY2BGR);
    else
        colorImage = image.clone();

    cv::Mat colorMask;
    if (mask.channels() == 1)
        cv::cvtColor(mask, colorMask, cv::COLOR_GRAY2BGR);
    else
        colorMask = mask.clone();

    // Ahora el tamaño debe ser igual, si no, redimensionar:
    if (colorMask.size() != colorImage.size())
        cv::resize(colorMask, colorMask, colorImage.size());

    // Poner rojo en máscara (donde máscara > 0)
    for (int y = 0; y < colorMask.rows; ++y) {
        for (int x = 0; x < colorMask.cols; ++x) {
            if (colorMask.at<cv::Vec3b>(y, x)[0] > 0) { // pixel zona
                colorMask.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
            }
        }
    }

    cv::Mat result;
    cv::addWeighted(colorMask, alpha, colorImage, 1 - alpha, 0, result);
    return result;
}

Mat processSliceAndGetMask(VolumeType::Pointer volume, unsigned int sliceIndex) {
    // Extraer slice 2D axial
    auto region = volume->GetLargestPossibleRegion();
    SliceType::Pointer slice = SliceType::New();

    SliceType::SizeType size = {region.GetSize()[0], region.GetSize()[1]};
    SliceType::IndexType start = {0, 0};
    SliceType::RegionType sliceRegion;
    sliceRegion.SetSize(size);
    sliceRegion.SetIndex(start);
    slice->SetRegions(sliceRegion);
    slice->Allocate();

    for (int y = 0; y < static_cast<int>(size[1]); ++y)
        for (int x = 0; x < static_cast<int>(size[0]); ++x) {
            itk::Index<3> idx3D = {x, y, sliceIndex};
            itk::Index<2> idx2D = {x, y};
            slice->SetPixel(idx2D, volume->GetPixel(idx3D));
        }

    // Suavizado para reducir ruido
    using SmoothingFilterType = itk::DiscreteGaussianImageFilter<SliceType, SliceType>;
    auto smoothing = SmoothingFilterType::New();
    smoothing->SetInput(slice);
    smoothing->SetVariance(1.0);
    smoothing->Update();

    // Gradiente para bordes
    using GradientFilterType = GradientMagnitudeImageFilter<SliceType, SliceType>;
    auto gradientFilter = GradientFilterType::New();
    gradientFilter->SetInput(smoothing->GetOutput());
    gradientFilter->Update();

    // Umbralización binaria (ajusta según convenga)
    using BinaryThresholdFilterType = BinaryThresholdImageFilter<SliceType, MaskType>;
    auto threshFilter = BinaryThresholdFilterType::New();
    threshFilter->SetInput(gradientFilter->GetOutput());
    threshFilter->SetLowerThreshold(20);
    threshFilter->SetUpperThreshold(255);
    threshFilter->SetInsideValue(255);
    threshFilter->SetOutsideValue(0);
    threshFilter->Update();

    // Morfología: primero erosión, luego dilatación (apertura)
    using StructuringElementType = BinaryBallStructuringElement<unsigned char, 2>;
    StructuringElementType structElem;
    structElem.SetRadius(1);
    structElem.CreateStructuringElement();

    using ErodeFilterType = BinaryErodeImageFilter<MaskType, MaskType, StructuringElementType>;
    auto erodeFilter = ErodeFilterType::New();
    erodeFilter->SetInput(threshFilter->GetOutput());
    erodeFilter->SetKernel(structElem);
    erodeFilter->SetForegroundValue(255);
    erodeFilter->Update();

    using DilateFilterType = itk::BinaryDilateImageFilter<MaskType, MaskType, StructuringElementType>;
    auto dilateFilter = DilateFilterType::New();
    dilateFilter->SetInput(erodeFilter->GetOutput());
    dilateFilter->SetKernel(structElem);
    dilateFilter->SetForegroundValue(255);
    dilateFilter->Update();

    MaskType::Pointer cleanedMask = dilateFilter->GetOutput();

    // Convertir máscara a cv::Mat
    Mat cvMask = itk::OpenCVImageBridge::ITKImageToCVMat(cleanedMask.GetPointer());

    return cvMask;
}


