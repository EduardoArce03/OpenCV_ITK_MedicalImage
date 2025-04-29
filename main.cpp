#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/cudaimgproc.hpp>

// ITK includes
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include <itkOpenCVImageBridge.h>
#include "image_utils.h"

int main() {

    using ImageType3D = itk::Image<float, 3>;
    auto reader = itk::ImageFileReader<ImageType3D>::New();
    reader->SetFileName("../data/images/BRATS_001.nii");
    reader->Update();
    auto volume = reader->GetOutput();

    // 👉 Extraer slice 50
    auto slice = ExtractSlice(volume, 50);

    // 👉 Convertir a cv::Mat
    cv::Mat sliceMat = ITKToMat(slice);

    // 👉 Mostrar la imagen
    cv::Mat display;
    sliceMat.convertTo(display, CV_8U, 255.0); // Escalado de [0,1] a [0,255]

    cv::imshow("Slice Escalado", display);

    // Normalización para visualizar original
    cv::Mat normalizedOriginal;
    cv::normalize(sliceMat, normalizedOriginal, 0, 1, cv::NORM_MINMAX);
    cv::imshow("Slice Original Normalizado", normalizedOriginal);

    cv::waitKey(0);

    /* PRUEBA DE PRIMERAS TECNICAS */
    // Ecualización global
    cv::Mat equalized;
    cv::equalizeHist(display, equalized);
    cv::imshow("Equalized Slice", equalized);
    cv::waitKey(0);

    // Ecualización adaptativa (CLAHE)
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(2.0);
    cv::Mat claheEqualized;
    clahe->apply(display, claheEqualized);

    cv::imshow("CLAHE Slice", claheEqualized);
    cv::waitKey(0);

    return 0;
}
