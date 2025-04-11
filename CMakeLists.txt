cmake_minimum_required(VERSION 3.16)
project(MiProyectoITKOpenCV)

# Requiere C++17 o superior
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar ITK (sin componentes específicos)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# Buscar OpenCV
find_package(OpenCV REQUIRED)

# Crear ejecutable
add_executable(MiApp main.cpp)

# Incluir headers del módulo ITKBridgeOpenCV manualmente
target_include_directories(MiApp PRIVATE
  ${OpenCV_INCLUDE_DIRS}
  /usr/local/include/ITKBridgeOpenCV
)

# Enlazar librerías de ITK y OpenCV
target_link_libraries(MiApp
  ${ITK_LIBRARIES}
  ${OpenCV_LIBS}
)
