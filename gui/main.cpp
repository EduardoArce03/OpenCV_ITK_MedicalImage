#include <opencv2/opencv.hpp>
#include "brats_loader.h"
#include "nii_processing.h"  // processSlice, extractMaskSlice, overlayMask

using namespace cv;
using namespace std;

// Procesamiento individual (1 slice)
void procesarIndividual(const BratsData& patient, int sliceIndex) {
    Mat imgSlice = processSlice(patient.modalities.at("flair"), sliceIndex);
    Mat maskSlice = extractMaskSlice(patient.segmentation, sliceIndex);
    threshold(maskSlice, maskSlice, 0, 255, THRESH_BINARY);
    Mat overlay = overlayMask(imgSlice, maskSlice, 0.4);

    imshow("Slice procesado", imgSlice);
    imshow("Máscara original", maskSlice);
    imshow("Superposición máscara", overlay);
    waitKey(0);
}

// Procesamiento por lotes
void procesarPorLotes(const BratsData& patient, int startSlice, int endSlice) {
    system("mkdir -p output");

    for (int sliceIndex = startSlice; sliceIndex <= endSlice; ++sliceIndex) {
        cout << "Procesando slice " << sliceIndex << "..." << endl;

        Mat imgSlice = processSlice(patient.modalities.at("flair"), sliceIndex);
        Mat maskSlice = extractMaskSlice(patient.segmentation, sliceIndex);
        threshold(maskSlice, maskSlice, 0, 255, THRESH_BINARY);
        Mat overlay = overlayMask(imgSlice, maskSlice, 0.4);

        imshow("Slice", imgSlice);
        imshow("Máscara", maskSlice);
        imshow("Superposición", overlay);
        waitKey(30);

        string base = "output/slice_" + to_string(sliceIndex);
        imwrite(base + "_image.png", imgSlice);
        imwrite(base + "_mask.png", maskSlice);
        imwrite(base + "_overlay.png", overlay);
    }

    cout << "✅ Procesamiento por lotes finalizado." << endl;
    waitKey(0);
}

int main(int argc, char *argv[]) {
    std::string path = "../dataset/HGG/BraTS19_2013_3_1";
    BratsData patient = loadBratsCase(path);

    auto size = patient.modalities["flair"]->GetLargestPossibleRegion().GetSize();
    cout << "Volumen: " << size[0] << " x " << size[1] << " x " << size[2] << endl;

    // 🔽 Elige uno descomentando:

    // --- Individual ---
    //procesarIndividual(patient, 100);

    // --- Por lotes ---
    procesarPorLotes(patient, 70, 130);

    return 0;
}
