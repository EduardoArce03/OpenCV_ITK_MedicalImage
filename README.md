
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
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr/local \
  -DWITH_CUDA=ON \
  -DWITH_CUDNN=OFF \
  -DOPENCV_DNN_CUDA=ON \
  -DCUDA_ARCH_BIN=86 \
  -DCUDA_ARCH_PTX= \
  -DWITH_TBB=ON \
  -DWITH_OPENGL=ON \
  -DBUILD_EXAMPLES=OFF \
  -DWITH_GTK=ON \
  -DOPENCV_ENABLE_NONFREE=ON \
  -DOPENCV_EXTRA_MODULES_PATH=~/opencv_cuda/opencv_contrib/modules
```

 Ajusta `CUDA_ARCH_BIN=86` según tu GPU (por ejemplo: 75 = GTX 1660, 89 = RTX 40xx). eso se ve en
 la pagina de nvidia, IMPORTANTISIMO


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

add_executable(MiApp gui/main.cpp)

target_include_directories(MiApp PRIVATE
        ${OpenCV_INCLUDE_DIRS}
        /usr/local/include/ITKBridgeOpenCV
)

target_link_libraries(MiApp
        ${ITK_LIBRARIES}
        ${OpenCV_LIBS}
)
```

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
Eduardo Arce - UPS 2025
CompuInside
Esta guía fue armada con sudor, código y siuuu 🐐
