#include <opencv2/opencv.hpp>
#include "brats_loader.h"
#include "nii_processing.h"  // Aquí están processSlice, extractMaskSlice, overlayMask

using namespace cv;
using namespace std;

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
    } else {
        cout << "Imagen 3D con " << size[2] << " slices." << endl;
    }

    int sliceIndex = 123; // slice medio

    // Procesar slice para obtener imagen y máscara procesada
    Mat imgSlice = processSlice(flair, sliceIndex);          // Imagen procesada (gradiente + máscara)
    Mat maskSlice = extractMaskSlice(seg, sliceIndex);       // Máscara original (segmentación manual)
    threshold(maskSlice, maskSlice, 0, 255, THRESH_BINARY);

    // Superponer máscara procesada sobre imagen
    Mat overlay = overlayMask(imgSlice, maskSlice, 0.4);

    // Mostrar resultados
    imshow("Slice procesado (Z=" + to_string(sliceIndex) + ")", imgSlice);
    imshow("Máscara original (Z=" + to_string(sliceIndex) + ")", maskSlice);
    imshow("Superposición máscara", overlay);

    waitKey(0);

    return 0;
}
