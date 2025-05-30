#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include <opencv2/opencv.hpp>

#include "image_processing.h"
#include "brats_loader.h"
#include "utils.h"

using namespace cv;
using namespace std;

// Constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Conectar botones y sliders
    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(ui->applyButton, &QPushButton::clicked, this, &MainWindow::applyFilters);

    // Sliders - Actualizar etiquetas
    connect(ui->meanSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->meanValueLabel->setText(QString::number(value));
    });

    connect(ui->stdSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->stdValueLabel->setText(QString::number(value));
    });

    connect(ui->varSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->varValueLabel->setText(QString::number(value));
    });
}

// Destructor
MainWindow::~MainWindow()
{
    delete ui;
}

// === Cargar imagen médica desde carpeta BraTS ===
void MainWindow::loadImage()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Seleccionar carpeta BraTS");
    if (!folder.isEmpty()) {
        std::string path = folder.toStdString();
        patient = loadBratsCase(path);  // Carga FLAIR, T1, T1ce, T2 y máscara

        auto volume = patient.modalities["flair"];
        if (volume) {
            auto slice = ExtractSlice(volume, 50); // Extraer slice medio
            currentImage = ITKToMat(slice);
            currentImage.convertTo(currentImage, CV_8U, 255.0); // Escalar a [0-255]
            showOriginal(currentImage);
        } else {
            QMessageBox::warning(this, "Error", "No se pudo cargar el volumen FLAIR.");
        }
    }
}

// === Aplicar filtros desde GUI ===
void MainWindow::applyFilters()
{
    if (currentImage.empty()) return;

    int mean = ui->meanSlider->value();
    int std = ui->stdSlider->value();
    float var = static_cast<float>(ui->varSlider->value()) / 100.0f;

    cv::Mat gray;
    if (currentImage.channels() == 3)
        cv::cvtColor(currentImage, gray, cv::COLOR_BGR2GRAY);
    else
        gray = currentImage.clone();

    // Aplicar filtros de iluminación
    auto lightingResults = applyLightingFilters(gray);
    showResult(lightingResults[0], ui->eqHistLabel);
    showResult(lightingResults[1], ui->claheLabel);
    showResult(lightingResults[2], ui->gammaLabel);

    // Ruido Gaussiano y Speckle
    cv::Mat noisyGauss = addGaussianNoise(gray, mean, std);
    cv::Mat noisySpeckle = addSpeckleNoise(gray, var);
    showResult(noisyGauss, ui->noisyGaussLabel);
    showResult(noisySpeckle, ui->noisySpeckleLabel);

    // Filtros suavizados
    auto smoothed = applySmoothing(gray);
    showResult(smoothed[0], ui->blurLabel);
    showResult(smoothed[1], ui->gaussianLabel);
    showResult(smoothed[2], ui->medianLabel);

    // Detección de bordes
    auto edgeResults = applyEdges(gray, true);
    showResult(edgeResults[0], ui->cannyLabel);
}

// === Mostrar imagen original en QLabel ===
void MainWindow::showOriginal(const cv::Mat& img)
{
    if (img.empty()) return;

    QImage qimg(img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8);
    ui->originalLabel->setPixmap(QPixmap::fromImage(qimg).scaled(ui->originalLabel->size(), Qt::KeepAspectRatio));
}

// === Mostrar cualquier resultado en QLabel ===
void MainWindow::showResult(const cv::Mat& img, QLabel* label)
{
    if (img.empty() || !label) return;

    cv::Mat display;
    if (img.channels() == 1)
        cv::cvtColor(img, display, cv::COLOR_GRAY2BGR);
    else
        display = img.clone();

    QImage qimg(display.data, display.cols, display.rows, display.step, QImage::Format_BGR888);
    label->setPixmap(QPixmap::fromImage(qimg).scaled(label->size(), Qt::KeepAspectRatio));
}