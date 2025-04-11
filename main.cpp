#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/cudaimgproc.hpp>

// ITK includes
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include <ITKBridgeOpenCV/itkOpenCVImageBridge.h>

int main() {
    std::string rutaImagen = "/home/eduardo/Documents/Vision/test/test.jpg";

    // 🔹 1. Cargar imagen con OpenCV
    cv::Mat cvImage = cv::imread(rutaImagen, cv::IMREAD_COLOR);
    if (cvImage.empty()) {
        cv::cuda::GpuMat gpuImage;
        std::cerr << "Error: no se pudo abrir la imagen con OpenCV." << std::endl;
        return -1;
    }
    cv::imshow("Original - OpenCV", cvImage);

    // 🔹 2. Leer la imagen con ITK (en escala de grises)
    using InputPixelType = unsigned char;
    constexpr unsigned int Dimension = 2;
    using InputImageType = itk::Image<InputPixelType, Dimension>;
    using ReaderType = itk::ImageFileReader<InputImageType>;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(rutaImagen);

    try {
        reader->Update();
    } catch (itk::ExceptionObject &error) {
        std::cerr << "Error al leer la imagen con ITK: " << error << std::endl;
        return -1;
    }

    // 🔹 3. Procesar con ITK - rescalar intensidad a 0-255
    using RescaleFilterType = itk::RescaleIntensityImageFilter<InputImageType, InputImageType>;
    RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
    rescaleFilter->SetInput(reader->GetOutput());
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);

    try {
        rescaleFilter->Update();
    } catch (itk::ExceptionObject &error) {
        std::cerr << "Error al aplicar filtro ITK: " << error << std::endl;
        return -1;
    }

    // 🔹 4. Convertir imagen ITK → OpenCV
    cv::Mat itkToCvImage = itk::OpenCVImageBridge::ITKImageToCVMat<InputImageType>(rescaleFilter->GetOutput());

    // 🔹 5. Mostrar resultado de ITK con OpenCV
    cv::imshow("Procesada con ITK - OpenCV", itkToCvImage);
    cv::waitKey(0);

    return 0;
}
