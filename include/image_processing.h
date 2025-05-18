#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <string>
#include <itkImage.h>

// === FUNCIONES DE FILTROS DE ILUMINACIÓN ===

/**
 * @brief Aplica filtros de iluminación: ecualización, CLAHE y corrección Gamma.
 * @param gray Imagen en escala de grises
 * @return Vector con 3 imágenes procesadas: [Ecualizada, CLAHE, Gamma]
 */
std::vector<cv::Mat> applyLightingFilters(const cv::Mat& gray);

// === FUNCIONES DE RUIDO ===

/**
 * @brief Añade ruido Gaussiano a una imagen.
 * @param image Imagen original
 * @param mean Media del ruido
 * @param std Desviación estándar
 * @return Imagen con ruido Gaussiano
 */
cv::Mat addGaussianNoise(const cv::Mat& image, int mean, int std);

/**
 * @brief Añade ruido Speckle a una imagen.
 * @param image Imagen original
 * @param var Varianza del ruido
 * @return Imagen con ruido Speckle
 */
cv::Mat addSpeckleNoise(const cv::Mat& image, float var);


// === TIPOS DE IMAGENES ITK ===
using ImageType3D = itk::Image<float, 3>;
using ImageType2D = itk::Image<float, 2>;

// === FUNCIONES ===
ImageType2D::Pointer ExtractSlice(ImageType3D::Pointer image, int sliceIndex);
cv::Mat ITKToMat(ImageType2D::Pointer image);


// === FILTROS DE SUAVIZADO ===

/**
 * @brief Aplica filtros de suavizado: Blur, GaussianBlur, MedianBlur.
 * @param gray Imagen en escala de grises
 * @param k Tamaño del kernel (impar)
 * @return Vector con 3 imágenes filtradas: [Blur, Gaussian, Median]
 */
std::vector<cv::Mat> applySmoothing(const cv::Mat& gray, int k = 5);

// === DETECCIÓN DE BORDES ===

/**
 * @brief Detecta bordes usando Canny y Sobel.
 * @param image Imagen original (BGR o GRAY)
 * @param smooth Si true, aplica GaussianBlur antes
 * @param k Tamaño del kernel para suavizado
 * @return Vector con 2 imágenes: [Canny, Sobel]
 */
std::vector<cv::Mat> applyEdges(const cv::Mat& image, bool smooth = false, int k = 5);

// === OPERACIONES MORFOLÓGICAS ===

/**
 * @brief Aplica operaciones morfológicas: erosión, dilatación, top-hat, black-hat.
 * @param binary Imagen binaria/gris
 * @param kernelSizes Vectores de tamaños de kernel
 * @return Mapa con resultados: {"erosion_15", "dilation_15", ...}
 */
std::map<std::string, cv::Mat> applyMorphologicalOps(const cv::Mat& binary,
                                                    const std::vector<int>& kernelSizes);

#endif // IMAGE_PROCESSING_H