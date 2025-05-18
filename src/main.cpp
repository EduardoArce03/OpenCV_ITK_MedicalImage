#include "image_processing.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "image_processing.h"
#include <itkImageFileReader.h>

// Definiciones de tipos ITK
using ImageType3D = itk::Image<float, 3>;
using ImageType2D = itk::Image<float, 2>;

int main() {
    std::cout << "[INFO] Iniciando aplicación..." << std::endl;

    // 🔹 Cargar volumen 3D desde archivo .nii
    auto reader = itk::ImageFileReader<ImageType3D>::New();
    reader->SetFileName("../data/images/BRATS_001.nii");
    try {
        reader->Update();
    } catch (const itk::ExceptionObject &ex) {
        std::cerr << "[ERROR] No se pudo cargar el volumen: " << ex.what() << std::endl;
        return -1;
    }

    // 🔹 Extraer un slice (ejemplo: número 50)
    auto slice = ExtractSlice(reader->GetOutput(), 50);

    // 🔹 Convertir a cv::Mat
    cv::Mat sliceMat = ITKToMat(slice);
    cv::Mat display;
    sliceMat.convertTo(display, CV_8U, 255.0); // Escalar a [0-255]

    // 🔹 Aplicar filtros de iluminación
    auto lightingResults = applyLightingFilters(display);

    // 🔹 Aplicar ruido
    cv::Mat noisyGauss = addGaussianNoise(display, 0, 25);
    cv::Mat noisySpeckle = addSpeckleNoise(display, 0.04f);

    // 🔹 Aplicar filtrado suavizado
    auto smoothed = applySmoothing(display);

    // 🔹 Detectar bordes
    auto edgeResults = applyEdges(display);

    // 🔹 Mostrar todos los resultados en una sola ventana
    std::vector<cv::Mat> row1 = {display, lightingResults[0], lightingResults[1], lightingResults[2]};
    std::vector<cv::Mat> row2 = {noisyGauss, noisySpeckle};
    std::vector<cv::Mat> row3 = {smoothed[0], smoothed[1], smoothed[2]};
    std::vector<cv::Mat> row4 = {edgeResults[0], edgeResults[1]};

    cv::Mat result1, result2, result3, result4;
    cv::hconcat(row1, result1);
    cv::hconcat(row2, result2);
    cv::hconcat(row3, result3);
    cv::hconcat(row4, result4);

    cv::Mat finalResult;
    cv::vconcat(std::vector<cv::Mat>{result1, result2, result3, result4}, finalResult);

    // 🔍 Redimensionar si es muy grande
    cv::Mat resizedFinal;
    cv::resize(finalResult, resizedFinal, cv::Size(), 0.7, 0.7); // 70% tamaño original

    // 🖼️ Mostrar resultado final
    cv::imshow("Visión por Computador - Resultados", resizedFinal);
    std::cout << "[INFO] Presione cualquier tecla para cerrar..." << std::endl;
    cv::waitKey(0);

    // 💾 Opcional: Guardar resultado
    cv::imwrite("../data/output/resultados_consolidados.jpg", finalResult);
    std::cout << "[INFO] Imágenes procesadas guardadas en ../data/output/" << std::endl;

    return 0;
}