#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// === FUNCIONES AUXILIARES ===

/**
 * @brief Asegura que una carpeta exista, si no, la crea.
 * @param path Ruta de la carpeta
 */
void ensureDirectoryExists(const std::string& path);

/**
 * @brief Guarda una imagen procesada en una carpeta específica.
 * @param img Imagen a guardar
 * @param folder Carpeta destino
 * @param filename Nombre del archivo
 */
void saveProcessed(cv::Mat img, const std::string& folder, const std::string& filename);

/**
 * @brief Redimensiona una imagen manteniendo proporciones.
 * @param src Imagen original
 * @param dst Imagen redimensionada
 * @param maxSize Tamaño máximo (ancho o alto)
 */
void resizeToFit(cv::Mat& src, cv::Mat& dst, int maxSize = 500);

/**
 * @brief Convierte una imagen OpenCV a color si es necesario.
 * @param src Imagen original
 * @param dst Imagen de salida
 */
void toColorIfNeeded(cv::Mat& src, cv::Mat& dst);

/**
 * @brief Concatena múltiples imágenes horizontalmente.
 * @param images Vector de imágenes
 * @return Imagen resultante
 */
cv::Mat hconcatImages(const std::vector<cv::Mat>& images);

/**
 * @brief Concatena múltiples imágenes verticalmente.
 * @param images Vector de imágenes
 * @return Imagen resultante
 */
cv::Mat vconcatImages(const std::vector<cv::Mat>& images);

#endif // UTILS_H