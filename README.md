
# 🧬 ITK + OpenCV + CUDA + BridgeOpenCV en Ubuntu 22.04
Cómo compilar y usar **ITK** junto a **OpenCV con soporte CUDA**
---

## ✅ Requisitos

```bash
sudo apt update
sudo apt install -y git cmake g++ libgtk2.0-dev pkg-config \
                    libavcodec-dev libavformat-dev libswscale-dev \
                    libjpeg-dev libpng-dev libtiff-dev libdc1394-22-dev \
                    libopenexr-dev libeigen3-dev libtbb-dev \
                    libcuda1-525 nvidia-cuda-toolkit
```

---

## ⚙️ Paso 1: Compilar OpenCV con soporte CUDA

```bash
git clone https://github.com/opencv/opencv.git ~/opencv_cuda/opencv
cd ~/opencv_cuda/opencv
mkdir build && cd build

cmake .. \
  -DWITH_CUDA=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr/local

make -j$(nproc)
sudo make install
```

---

## 🧬 Paso 2: Compilar e instalar ITK

```bash
git clone https://github.com/InsightSoftwareConsortium/ITK.git ~/ITK
cd ~/ITK
mkdir build && cd build

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_TESTING=OFF

make -j$(nproc)
sudo make install
```

---

## 🔌 Paso 3: Instalar el módulo `ITKBridgeOpenCV`

```bash
mkdir -p ~/ITKBridgeOpenCVExternal
cd ~/ITKBridgeOpenCVExternal
cp -r ~/ITK/Modules/Video/BridgeOpenCV .
```

### 📄 `CMakeLists.txt` principal:

```cmake
cmake_minimum_required(VERSION 3.16)
project(ITKBridgeOpenCVExternal)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_subdirectory(BridgeOpenCV)
```

### 📄 `BridgeOpenCV/CMakeLists.txt` → al final:

```cmake
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include/ITKBridgeOpenCV
        FILES_MATCHING PATTERN "*.h*")
```

### 🛠️ Compilar e instalar el módulo:

```bash
mkdir build && cd build
cmake .. \
  -DITK_DIR=/usr/local/lib/cmake/ITK-6.0 \
  -DOpenCV_DIR=/usr/local/lib/cmake/opencv4

make -j$(nproc)
sudo make install
```

---

## 💻 Paso 4: Crear proyecto C++ con ITK + OpenCV + CUDA

### 📁 Estructura:

```
Vision/test/
├── build/
├── main.cpp
└── CMakeLists.txt
```

### 📄 `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(MiProyectoITKOpenCV)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(OpenCV REQUIRED)

add_executable(MiApp main.cpp)

target_include_directories(MiApp PRIVATE
  ${OpenCV_INCLUDE_DIRS}
  /usr/local/include/ITKBridgeOpenCV
)

target_link_libraries(MiApp
  ${ITK_LIBRARIES}
  ${OpenCV_LIBS}
)
```

### 📄 `main.cpp` (resumen):

```cpp
#include <opencv2/opencv.hpp>
#include <ITKBridgeOpenCV/itkOpenCVImageBridge.h>
#include <itkImage.h>
#include <iostream>

int main() {
    cv::Mat cvImage = cv::imread("test.jpg", cv::IMREAD_GRAYSCALE);
    if (cvImage.empty()) return 1;

    using ImageType = itk::Image<unsigned char, 2>;
    auto itkImage = itk::OpenCVImageBridge::CVMatToITKImage<ImageType>(cvImage);
    auto backToCV = itk::OpenCVImageBridge::ITKImageToCVMat<ImageType>(itkImage);

    // CUDA: aplicar filtro Gaussiano
    cv::cuda::GpuMat gpuImage;
    gpuImage.upload(backToCV);
    cv::cuda::GpuMat gpuBlurred;
    cv::cuda::GaussianBlur(gpuImage, gpuBlurred, cv::Size(15,15), 0);

    cv::Mat result;
    gpuBlurred.download(result);
    cv::imshow("CUDA Blur", result);
    cv::waitKey(0);
    return 0;
}
```

---

## ✅ Paso 5: Compilar el proyecto

```bash
cd Vision/test
mkdir -p build && cd build
cmake ..
make -j$(nproc)
./MiApp
```

---

## 🧪 Verificación final

- ✔️ OpenCV carga la imagen
- ✔️ ITK procesa y transforma
- ✔️ CUDA acelera el post-procesado
- ✔️ ¡Y todo funciona junto!

---

## ✨ Créditos

Esta guía fue armada con sudor, código y siuuu 🐐
