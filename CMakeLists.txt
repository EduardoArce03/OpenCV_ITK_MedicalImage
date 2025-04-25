cmake_minimum_required(VERSION 3.16)
project(MiProyectoITKOpenCV)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenEXR REQUIRED)
find_package(Iconv REQUIRED)

# Eigen
find_package(Eigen3 REQUIRED)

# OpenCV
find_package(OpenCV REQUIRED)

# ITK
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# Incluir ruta personalizada del bridge OpenCV-ITK
include_directories(/home/andres/ITKBridgeOpenCVExternal/BridgeOpenCV/include)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Crear el ejecutable
add_executable(MiApp
        main.cpp
        src/image_utils.cpp)

# Vincular bibliotecas
target_link_libraries(MiApp ${OpenCV_LIBS} ${ITK_LIBRARIES})
