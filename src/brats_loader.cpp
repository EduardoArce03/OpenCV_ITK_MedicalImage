#include "brats_loader.h"
#include "itkImageFileReader.h"
#include <filesystem>

namespace fs = std::filesystem;
using namespace itk;

template<typename TImage>
typename TImage::Pointer readNii(const string& path) {
    using ReaderType = ImageFileReader<TImage>;
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(path);
    reader->Update();
    return reader->GetOutput();
}

BratsData loadBratsCase(const std::string& patientFolder) {
    BratsData data;
    string prefix = fs::path(patientFolder).filename().string(); // "BraTS19_2013_2_1"

    vector<std::string> modalities = {"flair", "t1", "t1ce", "t2"};

    for (const auto& modality : modalities) {
        string filePath = patientFolder + "/" + prefix + "_" + modality + ".nii";
        data.modalities[modality] = readNii<VolumeImageType>(filePath);
    }

    string maskPath = patientFolder + "/" + prefix + "_seg.nii";
    data.segmentation = readNii<MaskImageType>(maskPath);

    return data;
}
