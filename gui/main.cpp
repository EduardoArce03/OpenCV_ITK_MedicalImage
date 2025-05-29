#include <QApplication>
#include "mainwindow.h"
#include <opencv2/opencv.hpp>
#include "brats_loader.h"
#include "itkOpenCVImageBridge.h"
#include "itkRescaleIntensityImageFilter.h"

using namespace cv;
using namespace std;
int main(int argc, char *argv[]) {
    // Ruta a una carpeta de paciente (puede ser LGG o HGG)
    std::string path = "../dataset/HGG/BraTS19_2013_2_1";

    // Cargar imagen (solo flair)
    BratsData patient = loadBratsCase(path);
    auto flair = patient.modalities["flair"];

    // Extraer slice medio
    using SliceType = Image<float, 2>;
    SliceType::Pointer slice = SliceType::New();

    auto region = flair->GetLargestPossibleRegion();
    int z = region.GetSize()[2] / 2;
    using FloatImage2D = Image<float, 2>;
    using UCharImage2D = Image<unsigned char, 2>;
    SliceType::SizeType size = {region.GetSize()[0], region.GetSize()[1]};
    SliceType::IndexType start = {0, 0};

    SliceType::RegionType sliceRegion;
    sliceRegion.SetIndex(start);
    sliceRegion.SetSize(size);
    slice->SetRegions(sliceRegion);
    slice->Allocate();

    for (int y = 0; y < size[1]; ++y) {
        for (int x = 0; x < size[0]; ++x) {
            Index<3> idx3D = {x, y, z};
            Index<2> idx2D = {x, y};
            slice->SetPixel(idx2D, flair->GetPixel(idx3D));
        }
    }

    using UCharImage2D = Image<unsigned char, 2>;
    auto rescaler = RescaleIntensityImageFilter<FloatImage2D, UCharImage2D>::New();
    rescaler->SetInput(slice);
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);
    rescaler->Update();

    Mat img = itk::OpenCVImageBridge::ITKImageToCVMat(rescaler->GetOutput());
    imshow("Flair Slice Z=" + std::to_string(z), img);
    waitKey(0);

    return 0;
}
