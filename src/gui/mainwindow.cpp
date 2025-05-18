#include "mainwindow.h"
#include "ui_mainwindow.h"

// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

// Funciones de procesamiento
#include "image_processing.h"

// ITK

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Conectar sliders y botones
    connect(ui->meanSlider, &QSlider::valueChanged, this, &MainWindow::updateNoiseParams);
    connect(ui->stdSlider, &QSlider::valueChanged, this, &MainWindow::updateNoiseParams);
    connect(ui->varSlider, &QSlider::valueChanged, this, &MainWindow::updateNoiseParams);
    connect(ui->applyButton, &QPushButton::clicked, this, &MainWindow::applyFilters);
    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadImage);

    // Valores iniciales
    ui->meanLabel->setText(QString::number(ui->meanSlider->value()));
    ui->stdLabel->setText(QString::number(ui->stdSlider->value()));
    ui->varLabel->setText(QString::number(ui->varSlider->value()));

    // Cargar imagen por defecto o dejar vacío
    currentImage = cv::Mat();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateNoiseParams()
{
    // Actualizar etiquetas
    ui->meanLabel->setText(QString::number(ui->meanSlider->value()));
    ui->stdLabel->setText(QString::number(ui->stdSlider->value()));
    ui->varLabel->setText(QString::number(ui->varSlider->value()));
}

void MainWindow::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp *.nii)"));
    if (!fileName.isEmpty()) {
        std::string path = fileName.toStdString();

        // Cargar imagen 2D (puede ser un slice de volumen .nii también)
        cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
        if (!img.empty()) {
            currentImage = img.clone();
            showOriginalImage();
        } else {
            QMessageBox::warning(this, "Error", "No se pudo cargar la imagen.");
        }
    }
}

void MainWindow::showOriginalImage()
{
    if (currentImage.empty()) return;

    // Mostrar en QLabel como QImage
    cv::Mat display;
    cv::cvtColor(currentImage, display, cv::COLOR_GRAY2BGR); // Convertir a color si es necesario
    QImage qimg(display.data, display.cols, display.rows, display.step, QImage::Format_BGR888);
    ui->originalLabel->setPixmap(QPixmap::fromImage(qimg).scaled(ui->originalLabel->size(), Qt::KeepAspectRatio));
}

void MainWindow::applyFilters()
{
    if (currentImage.empty()) return;

    int mean = ui->meanSlider->value();
    int std = ui->stdSlider->value();
    float var = static_cast<float>(ui->varSlider->value()) / 100.0f;

    // Aplicar ruido Gaussiano
    cv::Mat noisyGauss = addGaussianNoise(currentImage, mean, std);
    cv::Mat noisySpeckle = addSpeckleNoise(currentImage, var);

    // Aplicar filtros suavizados
    auto smoothed = applySmoothing(currentImage);
    cv::Mat blur = smoothed[0], gaussian = smoothed[1], median = smoothed[2];

    // Aplicar iluminación
    auto lighting = applyLightingFilters(currentImage);
    cv::Mat eqHist = lighting[0], clahe = lighting[1], gamma = lighting[2];

    // Aplicar bordes
    auto edges = applyEdges(currentImage);
    cv::Mat canny = edges[0], sobel = edges[1];

    // Mostrar resultados
    showResult(noisyGauss, ui->noisyGaussLabel);
    showResult(noisySpeckle, ui->noisySpeckleLabel);
    showResult(blur, ui->blurLabel);
    showResult(gaussian, ui->gaussianLabel);
    showResult(median, ui->medianLabel);
    showResult(eqHist, ui->eqHistLabel);
    showResult(clahe, ui->claheLabel);
    showResult(gamma, ui->gammaLabel);
    showResult(canny, ui->cannyLabel);
    showResult(sobel, ui->sobelLabel);
}

void MainWindow::showResult(const cv::Mat& img, QLabel* label)
{
    if (img.empty()) return;

    cv::Mat display;
    if (img.channels() == 1)
        cv::cvtColor(img, display, cv::COLOR_GRAY2BGR);
    else
        display = img.clone();

    QImage qimg(display.data, display.cols, display.rows, display.step, QImage::Format_BGR888);
    label->setPixmap(QPixmap::fromImage(qimg).scaled(label->size(), Qt::KeepAspectRatio));
}