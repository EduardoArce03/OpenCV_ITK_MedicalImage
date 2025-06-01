#include <opencv2/opencv.hpp>
#include "brats_loader.h"
#include "itkOpenCVImageBridge.h"
#include "itkRescaleIntensityImageFilter.h"
#include "image_processing.h"
#include "nii_processing.h"

using namespace cv;
using namespace std;

// Superponer máscara sobre imagen base con color rojo
Mat overlayMaskOnBase(const Mat& base, const Mat& mask, float alpha = 0.4) {
    Mat colorBase;
    cvtColor(base, colorBase, COLOR_GRAY2BGR);
    Mat colorMask;
    cvtColor(mask, colorMask, COLOR_GRAY2BGR);
    colorMask.setTo(Scalar(0, 255, 0), mask);  // rojo puro
    Mat blended;
    addWeighted(colorBase, 1.0, colorMask, alpha, 0.0, blended);
    return blended;
}

// Procesamiento individual (1 slice)
void procesarIndividual(const BratsData& patient, int sliceIndex) {
    // Crear carpeta de salida
    system("mkdir -p output");

    // Obtener volumen FLAIR
    auto flair = patient.modalities.at("flair");
    auto region = flair->GetLargestPossibleRegion();
    auto size = region.GetSize();

    // Extraer FLAIR slice (original sin procesar)
    Mat imgOriginal(size[1], size[0], CV_32FC1);
    for (int y = 0; y < size[1]; ++y)
        for (int x = 0; x < size[0]; ++x)
            imgOriginal.at<float>(y, x) = flair->GetPixel({x, y, sliceIndex});

    Mat imgOriginalU8;
    normalize(imgOriginal, imgOriginalU8, 0, 255, NORM_MINMAX);
    imgOriginalU8.convertTo(imgOriginalU8, CV_8UC1);

    // Imagen procesada
    Mat imgSlice = processSlice(flair, sliceIndex);

    // Extraer máscara de la segmentación
    Mat maskSlice = extractMaskSlice(patient.segmentation, sliceIndex);
    threshold(maskSlice, maskSlice, 0, 255, THRESH_BINARY);

    // Superposición de la máscara sobre procesado y sobre original
    Mat overlayProcessed = overlayMask(imgSlice, maskSlice, 0.4);
    Mat overlayOriginal = overlayMaskOnBase(imgOriginalU8, maskSlice, 0.4);

    // Redimensionar imágenes para asegurar mismo tamaño si es necesario
    resize(imgSlice, imgSlice, imgOriginalU8.size());
    resize(maskSlice, maskSlice, imgOriginalU8.size());
    resize(overlayProcessed, overlayProcessed, imgOriginalU8.size());
    resize(overlayOriginal, overlayOriginal, imgOriginalU8.size());

    // Convertir imágenes en color para concatenarlas horizontalmente
    Mat c1, c2, c3, c4;
    cvtColor(imgOriginalU8, c1, COLOR_GRAY2BGR);
    cvtColor(imgSlice, c2, COLOR_GRAY2BGR);
    cvtColor(maskSlice, c3, COLOR_GRAY2BGR); // blanco y negro a color
    c4 = overlayOriginal;

    // Concatenar en una sola imagen horizontal
    Mat display;
    hconcat(vector<Mat>{c1, c2, c3, c4}, display);

    // Mostrar en una sola ventana
    imshow("Vista Compuesta [FLAIR | Procesado | Máscara | Superposición]", display);
    waitKey(0);

    // Guardar imágenes
    string base = "output/slice_" + to_string(sliceIndex);
    imwrite(base + "_flair.png", imgOriginalU8);
    imwrite(base + "_processed.png", imgSlice);
    imwrite(base + "_mask.png", maskSlice);
    imwrite(base + "_overlay_original.png", overlayOriginal);
    imwrite(base + "_overlay_processed.png", overlayProcessed);
    imwrite(base + "_composite.png", display);
}

// Main principal
int main(int argc, char *argv[]) {
    string path = "../dataset/HGG/BraTS19_2013_2_1";
    BratsData patient = loadBratsCase(path);

    auto size = patient.modalities["flair"]->GetLargestPossibleRegion().GetSize();
    cout << "Volumen cargado: " << size[0] << " x " << size[1] << " x " << size[2] << endl;

    // Procesar una slice específica
    procesarIndividual(patient, 100);  // puedes cambiar el índice si quieres

    return 0;
}
