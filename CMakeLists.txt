cmake_minimum_required(VERSION 3.16)
project(MiProyectoITKOpenCV)

# Estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Paquetes necesarios
find_package(OpenEXR REQUIRED)
find_package(Iconv REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# Soporte para Qt5
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Rutas personalizadas
include_directories(/home/andres/ITKBridgeOpenCVExternal/BridgeOpenCV/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Fuentes del proyecto
add_executable(MiApp
        gui/main.cpp
        gui/mainwindow.cpp
        src/image_utils.cpp
        src/image_processing.cpp
)

# Enlazar bibliotecas
target_link_libraries(MiApp
        PRIVATE
        ${OpenCV_LIBS}
        ${ITK_LIBRARIES}
        Qt5::Widgets
)