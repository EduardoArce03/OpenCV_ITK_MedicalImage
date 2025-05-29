#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
#include "image_processing.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(ui->applyButton, &QPushButton::clicked, this, &MainWindow::applyFilters);
    connect(ui->meanSlider, &QSlider::valueChanged, this, &MainWindow::updateNoiseParams);
    connect(ui->stdSlider, &QSlider::valueChanged, this, &MainWindow::updateNoiseParams);
    connect(ui->varSlider, &QSlider::valueChanged, this, &MainWindow::updateNoiseParams);

    // Valores iniciales
    ui->meanLabel->setText(QString::number(ui->meanSlider->value()));
    ui->stdLabel->setText(QString::number(ui->stdSlider->value()));
    ui->varLabel->setText(QString::number(ui->varSlider->value()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateNoiseParams()
{
    ui->meanLabel->setText(QString::number(ui->meanSlider->value()));
    ui->stdLabel->setText(QString::number(ui->stdSlider->value()));
    ui->varLabel->setText(QString::number(ui->varSlider->value()));
}

void MainWindow::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir Imagen Médica"), "", tr("Imágenes (*.png *.jpg *.bmp *.nii)"));
    if (!fileName.isEmpty()) {
        std::string path = fileName.toStdString();
        currentImage = cv::imread(path, cv::IMREAD_GRAYSCALE);
        if (!currentImage.empty()) {
            showOriginal(currentImage);
        } else {
            QMessageBox::warning(this, "Error", "No se pudo cargar la imagen.");
        }
    }
}

void MainWindow::applyFilters()
{
    if (currentImage.empty()) return;

    int mean = ui->meanSlider->value();
    int std = ui->stdSlider->value();
    float var = static_cast<float>(ui->varSlider->value()) / 100.0f;

    // Aplicar ruido
    cv::Mat noisyGauss = addGaussianNoise(currentImage, mean, std);
    cv::Mat noisySpeckle = addSpeckleNoise(currentImage, var);

    // Aplicar suavizado
    auto smoothed = applySmoothing(currentImage);
    cv::Mat blur = smoothed[0], gaussian = smoothed[1], median = smoothed[2];

    // Aplicar iluminación
    auto lighting = applyLightingFilters(currentImage);
    cv::Mat eqHist = lighting[0], clahe = lighting[1], gamma = lighting[2];

    // Mostrar resultados
    showResult(eqHist, ui->eqHistLabel);
    showResult(clahe, ui->claheLabel);
    showResult(gamma, ui->gammaLabel);
    showResult(noisyGauss, ui->noisyGaussLabel);
    showResult(noisySpeckle, ui->noisySpeckleLabel);
    showResult(blur, ui->blurLabel);
    showResult(gaussian, ui->gaussianLabel);
    showResult(median, ui->medianLabel);
}

void MainWindow::showOriginal(const cv::Mat& img)
{
    QImage qimg(img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8);
    ui->originalLabel->setPixmap(QPixmap::fromImage(qimg).scaled(ui->originalLabel->size(), Qt::KeepAspectRatio));
}

void MainWindow::showResult(const cv::Mat& img, QLabel* label)
{
    cv::Mat display;
    if (img.channels() == 1)
        cv::cvtColor(img, display, cv::COLOR_GRAY2BGR);
    else
        display = img.clone();

    QImage qimg(display.data, display.cols, display.rows, display.step, QImage::Format_BGR888);
    label->setPixmap(QPixmap::fromImage(qimg).scaled(label->size(), Qt::KeepAspectRatio));
}