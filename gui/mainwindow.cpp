#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include <opencv2/opencv.hpp>

#include "image_processing.h"
#include "brats_loader.h"
#include "utils.h"
#include "nii_processing.h"

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(ui->applyButton, &QPushButton::clicked, this, &MainWindow::applyFilters);
    connect(ui->processButton, &QPushButton::clicked, this, &MainWindow::procesarSlice);

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

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadImage()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Seleccionar carpeta BraTS");
    if (!folder.isEmpty()) {
        std::string path = folder.toStdString();
        patient = loadBratsCase(path);

        auto volume = patient.modalities["flair"];
        if (volume) {
            auto slice = ExtractSlice(volume, 50);
            currentImage = ITKToMat(slice);
            currentImage.convertTo(currentImage, CV_8U, 255.0);
            showOriginal(currentImage);
        } else {
            QMessageBox::warning(this, "Error", "No se pudo cargar el volumen FLAIR.");
        }
    }
}

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

    auto lightingResults = applyLightingFilters(gray);
    showResult(lightingResults[0], ui->eqHistLabel);
    showResult(lightingResults[1], ui->claheLabel);
    showResult(lightingResults[2], ui->gammaLabel);

    cv::Mat noisyGauss = addGaussianNoise(gray, mean, std);
    cv::Mat noisySpeckle = addSpeckleNoise(gray, var);
    showResult(noisyGauss, ui->noisyGaussLabel);
    showResult(noisySpeckle, ui->noisySpeckleLabel);

    auto smoothed = applySmoothing(gray);
    showResult(smoothed[0], ui->blurLabel);
    showResult(smoothed[1], ui->gaussianLabel);
    showResult(smoothed[2], ui->medianLabel);

    auto edgeResults = applyEdges(gray, true);
    showResult(edgeResults[0], ui->cannyLabel);
}

void MainWindow::procesarSlice()
{
    if (!patient.modalities.count("flair")) return;

    int sliceIndex = ui->sliceSpinBox->value();

    auto flair = patient.modalities.at("flair");
    auto region = flair->GetLargestPossibleRegion();
    auto size = region.GetSize();

    cv::Mat original(size[1], size[0], CV_32FC1);
    for (int y = 0; y < size[1]; ++y)
        for (int x = 0; x < size[0]; ++x)
            original.at<float>(y, x) = flair->GetPixel({x, y, sliceIndex});

    cv::Mat originalU8;
    normalize(original, originalU8, 0, 255, NORM_MINMAX);
    originalU8.convertTo(originalU8, CV_8UC1);

    cv::Mat imgSlice = processSlice(flair, sliceIndex);
    cv::Mat maskSlice = extractMaskSlice(patient.segmentation, sliceIndex);
    threshold(maskSlice, maskSlice, 0, 255, THRESH_BINARY);

    cv::Mat overlayOriginal = overlayMaskOnBase(originalU8, maskSlice, 0.4);

    ui->originalSliceLabel->setPixmap(QPixmap::fromImage(QImage(originalU8.data, originalU8.cols, originalU8.rows, originalU8.step, QImage::Format_Grayscale8)).scaled(ui->originalSliceLabel->size(), Qt::KeepAspectRatio));
    ui->processedSliceLabel->setPixmap(QPixmap::fromImage(QImage(imgSlice.data, imgSlice.cols, imgSlice.rows, imgSlice.step, QImage::Format_Grayscale8)).scaled(ui->processedSliceLabel->size(), Qt::KeepAspectRatio));
    ui->overlaySliceLabel->setPixmap(QPixmap::fromImage(QImage(overlayOriginal.data, overlayOriginal.cols, overlayOriginal.rows, overlayOriginal.step, QImage::Format_BGR888)).scaled(ui->overlaySliceLabel->size(), Qt::KeepAspectRatio));
}

void MainWindow::showOriginal(const cv::Mat& img)
{
    if (img.empty()) return;
    QImage qimg(img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8);
    ui->originalLabel->setPixmap(QPixmap::fromImage(qimg).scaled(ui->originalLabel->size(), Qt::KeepAspectRatio));
}

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
