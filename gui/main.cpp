#include <opencv2/opencv.hpp>
#include "brats_loader.h"
#include "nii_processing.h"  // processSlice, extractMaskSlice, overlayMask

// ITK includes para slice original
#include "itkImage.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkOpenCVImageBridge.h"

using namespace cv;
using namespace std;
using namespace itk;

int main(int argc, char *argv[]) {
    std::string path = "../dataset/HGG/BraTS19_2013_2_1";

    BratsData patient = loadBratsCase(path);
    auto flair = patient.modalities["flair"];
    auto seg = patient.segmentation;

    auto region = flair->GetLargestPossibleRegion();
    auto size = region.GetSize();

    cout << "Dimensiones del volumen: "
         << size[0] << " x " << size[1] << " x " << size[2] << endl;

    if (size[2] == 1) {
        cout << "Imagen 2D con un solo slice." << endl;
        return 0;
    } else {
        cout << "Imagen 3D con " << size[2] << " slices." << endl;
    }

    int sliceIndex = 123; // Slice que se va a visualizar

    // === Slice original (sin procesar) ===
    using SliceType = itk::Image<float, 2>;
    SliceType::Pointer slice = SliceType::New();

    SliceType::SizeType size2D = {size[0], size[1]};
    SliceType::IndexType start = {0, 0};
    SliceType::RegionType sliceRegion;
    sliceRegion.SetSize(size2D);
    sliceRegion.SetIndex(start);
    slice->SetRegions(sliceRegion);
    slice->Allocate();

    for (int y = 0; y < static_cast<int>(size2D[1]); ++y) {
        for (int x = 0; x < static_cast<int>(size2D[0]); ++x) {
            Index<3> idx3D = {x, y, sliceIndex};
            Index<2> idx2D = {x, y};
            slice->SetPixel(idx2D, flair->GetPixel(idx3D));
        }
    }

    // Rescalar intensidades [0, 255]
    using UCharSliceType = itk::Image<unsigned char, 2>;
    auto rescaler = RescaleIntensityImageFilter<SliceType, UCharSliceType>::New();
    rescaler->SetInput(slice);
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);
    rescaler->Update();

    Mat originalImg = itk::OpenCVImageBridge::ITKImageToCVMat(rescaler->GetOutput());

    // Procesar slice para obtener imagen y máscara procesada
    Mat imgSlice = processSlice(flair, sliceIndex);          // Imagen procesada (gradiente + morfología)
    Mat maskSlice = extractMaskSlice(seg, sliceIndex);       // Máscara original (segmentación manual)
    threshold(maskSlice, maskSlice, 0, 255, THRESH_BINARY);  // Binarizar por si es multiclase

    // Superponer máscara procesada sobre imagen
    Mat overlay = overlayMask(imgSlice, maskSlice, 0.4);

    // Mostrar resultados
    imshow("Slice original (Z=" + to_string(sliceIndex) + ")", originalImg);
    imshow("Slice procesado (Z=" + to_string(sliceIndex) + ")", imgSlice);
    imshow("Máscara original (Z=" + to_string(sliceIndex) + ")", maskSlice);
    imshow("Superposición máscara", overlay);

    cout << "Mostrando visualización de slice Z=" << sliceIndex << ". Presiona cualquier tecla para cerrar." << endl;
    waitKey(0);

    return 0;
}
