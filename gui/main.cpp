#include <opencv2/opencv.hpp>

#include "brats_loader.h"
#include "itkOpenCVImageBridge.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImage.h"
#include "itkIndex.h"

using namespace cv;
using namespace std;
using namespace itk;

int main(int argc, char *argv[]) {
    std::string path = "../dataset/HGG/BraTS19_2013_2_1";

    BratsData patient = loadBratsCase(path);
    auto flair = patient.modalities["flair"];

    // Imprimir dimensiones del volumen
    auto region = flair->GetLargestPossibleRegion();
    auto size = region.GetSize();

    cout << "Dimensiones del volumen:" << endl;
    cout << "  X: " << size[0] << endl;
    cout << "  Y: " << size[1] << endl;
    cout << "  Z: " << size[2] << endl;

    if (size[2] == 1) {
        cout << "La imagen es 2D, solo tiene un slice en Z." << endl;
    } else {
        cout << "La imagen es 3D, con " << size[2] << " slices en Z." << endl;
    }

    // Ahora puedes continuar con el resto del código para extraer el slice, etc.
    int z = size[2] / 2;

    using PixelType = float;
    constexpr unsigned int Dimension2D = 2;
    using SliceType = Image<PixelType, Dimension2D>;

    SliceType::Pointer slice = SliceType::New();

    SliceType::SizeType size2D = {size[0], size[1]};
    SliceType::IndexType start = {0, 0};

    SliceType::RegionType sliceRegion;
    sliceRegion.SetIndex(start);
    sliceRegion.SetSize(size2D);
    slice->SetRegions(sliceRegion);
    slice->Allocate();

    for (int y = 0; y < static_cast<int>(size2D[1]); ++y) {
        for (int x = 0; x < static_cast<int>(size2D[0]); ++x) {
            Index<3> idx3D = {x, y, z};
            Index<2> idx2D = {x, y};
            slice->SetPixel(idx2D, flair->GetPixel(idx3D));
        }
    }

    using UCharImage2D = Image<unsigned char, Dimension2D>;
    auto rescaler = RescaleIntensityImageFilter<SliceType, UCharImage2D>::New();
    rescaler->SetInput(slice);
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);
    rescaler->Update();

    Mat img = itk::OpenCVImageBridge::ITKImageToCVMat(rescaler->GetOutput());
    imshow("Slice Z=" + std::to_string(z), img);
    waitKey(0);
    auto maxZ = size[2];
    for (unsigned int sliceIdx = 0; sliceIdx < maxZ; sliceIdx += 20) {
        // Extraer slice actual
        SliceType::Pointer slice = SliceType::New();
        slice->SetRegions(sliceRegion);
        slice->Allocate();

        for (int y = 0; y < static_cast<int>(size2D[1]); ++y) {
            for (int x = 0; x < static_cast<int>(size2D[0]); ++x) {
                Index<3> idx3D = {x, y, sliceIdx};
                Index<2> idx2D = {x, y};
                slice->SetPixel(idx2D, flair->GetPixel(idx3D));
            }
        }

        auto rescaler = RescaleIntensityImageFilter<SliceType, UCharImage2D>::New();
        rescaler->SetInput(slice);
        rescaler->SetOutputMinimum(0);
        rescaler->SetOutputMaximum(255);
        rescaler->Update();

        Mat img = itk::OpenCVImageBridge::ITKImageToCVMat(rescaler->GetOutput());
        imshow("Slice Z=" + std::to_string(sliceIdx), img);
        cout << "Mostrando slice Z=" << sliceIdx << endl;
        waitKey(0);
    }

    return 0;
}
