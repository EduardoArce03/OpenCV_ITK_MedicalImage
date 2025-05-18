#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QFileDialog>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateNoiseParams();  // Actualiza etiquetas de sliders
    void loadImage();          // Cargar imagen médica desde disco
    void applyFilters();       // Aplicar técnicas de procesamiento
    void saveResults();        // Guardar imágenes procesadas

private:
    Ui::MainWindow *ui;

    cv::Mat currentImage;      // Imagen cargada actualmente
    std::string outputFolder;  // Carpeta donde se guardan resultados

    // Métodos privados auxiliares
    void showOriginalImage();                  // Mostrar imagen original
    void showResult(const cv::Mat& img, QLabel* label); // Mostrar imagen procesada en QLabel
};

#endif // MAINWINDOW_H