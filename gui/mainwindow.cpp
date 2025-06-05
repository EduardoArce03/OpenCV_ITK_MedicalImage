#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include <opencv2/opencv.hpp>

#include "image_processing.h"
#include "brats_loader.h"
#include "utils.h"
#include "nii_processing.h"
#include <QTextStream>
#include <QDebug>

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->startSliceSpinBox->setMaximum(300);
    ui->endSliceSpinBox->setMaximum(300);
    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(ui->processButton, &QPushButton::clicked, this, &MainWindow::procesarSlice);
    connect(ui->batchButton, &QPushButton::clicked, this, &MainWindow::procesarLote);

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
            showResult(currentImage, ui->originalSliceLabel);
        } else {
            QMessageBox::warning(this, "Error", "No se pudo cargar el volumen FLAIR.");
        }
    }
}

void MainWindow::procesarSlice()
{
    if (!patient.modalities.count("flair")) return;

    int sliceIndex = ui->sliceSpinBox->value();
    auto flair = patient.modalities.at("flair");
    auto slice = ExtractSlice(flair, sliceIndex);
    currentImage = ITKToMat(slice);

    // 1. Convertir a 8 bits para visualizar
    cv::Mat originalU8;
    normalize(currentImage, originalU8, 0, 255, NORM_MINMAX);
    originalU8.convertTo(originalU8, CV_8UC1);

    // 2. Clonar para aplicar filtros
    cv::Mat processed = originalU8.clone();

    // 3. Obtener la máscara binaria de segmentación
    cv::Mat mask = extractMaskSlice(patient.segmentation, sliceIndex);
    cv::Mat maskBin;
    threshold(mask, maskBin, 0, 255, cv::THRESH_BINARY);

    cv::Mat valuesInTumor;
    originalU8.copyTo(valuesInTumor, maskBin);

    std::vector<uchar> tumorPixels;
    for (int y = 0; y < valuesInTumor.rows; ++y) {
        for (int x = 0; x < valuesInTumor.cols; ++x) {
            uchar val = valuesInTumor.at<uchar>(y, x);
            if (val > 0) tumorPixels.push_back(val);
        }
    }


    double sum = 0;
    uchar minVal = 255, maxVal = 0;
    for (uchar val : tumorPixels) {
        sum += val;
        if (val < minVal) minVal = val;
        if (val > maxVal) maxVal = val;
    }
    double mean = tumorPixels.empty() ? 0 : sum / tumorPixels.size();
    int area = tumorPixels.size();

    // Guardar en archivo CSV
    guardarEstadisticas("output_batch/estadisticas.csv", sliceIndex, mean, minVal, maxVal, area);


    // 4. PREPROCESAMIENTO (ejemplo con CLAHE solo en la zona del tumor)
    QString selected = ui->preprocessCombo->currentText();
    if (selected == "CLAHE") {
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(2.0);
        cv::Mat claheResult;
        clahe->apply(processed, claheResult);
        claheResult.copyTo(processed, maskBin);  // Aplicar solo en el tumor
    } else if (selected == "Hist. Equal.") {
        cv::Mat temp;
        cv::equalizeHist(processed, temp);
        temp.copyTo(processed, maskBin);  // También solo en el tumor
    } else if (selected == "Gamma Corr.") {
        cv::Mat temp;
        processed.convertTo(temp, CV_32F, 1.0 / 255.0);
        pow(temp, 1.0 / 1.5, temp);
        temp *= 255.0;
        temp.convertTo(temp, CV_8U);
        temp.copyTo(processed, maskBin);
    }

    // 5. CHECKBOX FILTERS (también podrías aplicarlos condicionalmente dentro del tumor)
    if (ui->checkThreshold->isChecked()) {
        cv::Mat temp;
        cv::threshold(processed, temp, 80, 255, cv::THRESH_BINARY);
        temp.copyTo(processed, maskBin);  // Aplicar solo dentro del tumor
    }

    if (ui->checkLogical->isChecked()) {
        cv::Mat temp;
        cv::bitwise_not(processed, temp);
        temp.copyTo(processed, maskBin);
    }

    if (ui->checkBlur->isChecked()) {
        cv::Mat temp;
        cv::GaussianBlur(processed, temp, cv::Size(5, 5), 1.0);
        temp.copyTo(processed, maskBin);
    }

    if (ui->checkMorph->isChecked()) {
        cv::Mat temp;
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::morphologyEx(processed, temp, cv::MORPH_CLOSE, element);
        temp.copyTo(processed, maskBin);
    }

    if (ui->checkEdges->isChecked()) {
        cv::Mat temp;
        cv::Canny(processed, temp, 100, 200);  // valores típicos, puedes ajustar
        temp.copyTo(processed, maskBin);
    }

    if (ui->checkContours->isChecked()) {
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(maskBin, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        cv::drawContours(processed, contours, -1, cv::Scalar(255), 1);  // Dibuja en blanco sobre el tumor
    }

    if (ui->checkAnd->isChecked()) {
        cv::Mat temp = processed.clone();
        cv::bitwise_and(processed, 128, temp);  // con una constante
        temp.copyTo(processed, maskBin);
    }

    if (ui->checkOr->isChecked()) {
        cv::Mat temp = processed.clone();
        cv::bitwise_or(processed, 128, temp);  // con una constante
        temp.copyTo(processed, maskBin);
    }


    // 6. Crear overlay
    cv::Mat overlay = overlayMaskOnBase(processed, maskBin, 0.4);

    // 7. Mostrar
    showResult(originalU8, ui->originalSliceLabel);
    showResult(processed, ui->processedSliceLabel);
    showResult(overlay, ui->overlaySliceLabel);
}



void MainWindow::procesarLote()
{
    if (!patient.modalities.count("flair")) return;

    int start = ui->startSliceSpinBox->value();
    int end = ui->endSliceSpinBox->value();
    if (start > end) std::swap(start, end);

    auto flair = patient.modalities.at("flair");
    auto seg = patient.segmentation;

    QDir().mkpath("output_batch");

    for (int i = start; i <= end; ++i) {
        auto slice = ExtractSlice(flair, i);
        cv::Mat img = ITKToMat(slice);

        cv::Mat imgU8;
        normalize(img, imgU8, 0, 255, NORM_MINMAX);
        imgU8.convertTo(imgU8, CV_8UC1);

        cv::Mat processed = imgU8.clone();

        cv::Mat mask = extractMaskSlice(seg, i);
        cv::Mat maskBin;
        threshold(mask, maskBin, 0, 255, cv::THRESH_BINARY);

        QString selected = ui->preprocessCombo->currentText();
        if (selected == "CLAHE") {
            cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
            clahe->setClipLimit(2.0);
            cv::Mat temp;
            clahe->apply(processed, temp);
            temp.copyTo(processed, maskBin);
        } else if (selected == "Hist. Equal.") {
            cv::Mat temp;
            cv::equalizeHist(processed, temp);
            temp.copyTo(processed, maskBin);
        } else if (selected == "Gamma Corr.") {
            cv::Mat temp;
            processed.convertTo(temp, CV_32F, 1.0 / 255.0);
            pow(temp, 1.0 / 1.5, temp);
            temp *= 255.0;
            temp.convertTo(temp, CV_8U);
            temp.copyTo(processed, maskBin);
        } else if (selected == "Contrast Stretching") {
            cv::Mat temp;
            double minVal, maxVal;
            cv::minMaxLoc(processed, &minVal, &maxVal);
            processed.convertTo(temp, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
            temp.copyTo(processed, maskBin);
        }

        if (ui->checkThreshold->isChecked()) {
            cv::Mat temp;
            threshold(processed, temp, 80, 255, THRESH_BINARY);
            temp.copyTo(processed, maskBin);
        }

        if (ui->checkLogical->isChecked()) {
            cv::Mat temp;
            bitwise_not(processed, temp);
            temp.copyTo(processed, maskBin);
        }

        if (ui->checkBlur->isChecked()) {
            cv::Mat temp;
            GaussianBlur(processed, temp, cv::Size(5, 5), 1.0);
            temp.copyTo(processed, maskBin);
        }

        if (ui->checkMorph->isChecked()) {
            cv::Mat temp;
            cv::Mat element = getStructuringElement(MORPH_RECT, cv::Size(3, 3));
            morphologyEx(processed, temp, MORPH_CLOSE, element);
            temp.copyTo(processed, maskBin);
        }

        if (ui->checkEdges->isChecked()) {
            cv::Mat temp;
            Canny(processed, temp, 100, 200);
            temp.copyTo(processed, maskBin);
        }

        if (ui->checkContours->isChecked()) {
            std::vector<std::vector<cv::Point>> contours;
            findContours(maskBin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            drawContours(processed, contours, -1, Scalar(255), 1);
        }

        if (ui->checkAnd->isChecked()) {
            cv::Mat temp = processed.clone();
            bitwise_and(processed, 128, temp);
            temp.copyTo(processed, maskBin);
        }

        if (ui->checkOr->isChecked()) {
            cv::Mat temp = processed.clone();
            bitwise_or(processed, 128, temp);
            temp.copyTo(processed, maskBin);
        }

        // === Overlay y Guardado ===
        cv::Mat overlay = overlayMaskOnBase(processed, maskBin, 0.4);

        std::string base = "output_batch/slice_" + std::to_string(i);
        imwrite(base + "_original.png", imgU8);
        imwrite(base + "_processed.png", processed);
        imwrite(base + "_overlay.png", overlay);

        // === Estadísticas ===
        cv::Mat valuesInTumor;
        imgU8.copyTo(valuesInTumor, maskBin);
        std::vector<uchar> tumorPixels;
        for (int y = 0; y < valuesInTumor.rows; ++y)
            for (int x = 0; x < valuesInTumor.cols; ++x)
                if (uchar val = valuesInTumor.at<uchar>(y, x); val > 0)
                    tumorPixels.push_back(val);

        QFile file("output_batch/intensidades_puro.csv");
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            for (uchar val : tumorPixels) out << QString::number(val) << "\n";
            file.close();
        }

        double sum = std::accumulate(tumorPixels.begin(), tumorPixels.end(), 0.0);
        uchar minVal = tumorPixels.empty() ? 0 : *std::min_element(tumorPixels.begin(), tumorPixels.end());
        uchar maxVal = tumorPixels.empty() ? 0 : *std::max_element(tumorPixels.begin(), tumorPixels.end());
        double mean = tumorPixels.empty() ? 0 : sum / tumorPixels.size();
        int area = tumorPixels.size();

        guardarEstadisticas("output_batch/estadisticasLote.csv", i, mean, minVal, maxVal, area);
    }

    QMessageBox::information(this, "Lote procesado", "✅ Imágenes procesadas y guardadas en 'output_batch/'");

    // === Crear Video ===
    cv::VideoWriter writer;
    int codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    double fps = 5.0;
    cv::Size frameSize;

    for (int i = start; i <= end; ++i) {
        std::string filename = "output_batch/slice_" + std::to_string(i) + "_overlay.png";
        cv::Mat frame = cv::imread(filename);
        if (frame.empty()) continue;

        if (!writer.isOpened()) {
            frameSize = frame.size();
            writer.open("output_batch/video_resultado.avi", codec, fps, frameSize, true);
            if (!writer.isOpened()) {
                QMessageBox::warning(this, "Error", "No se pudo crear el video.");
                return;
            }
        }
        writer.write(frame);
    }

    system("python3 generar_estadisticas.py");
}




void MainWindow::showResult(const cv::Mat& img, QLabel* label)
{
    if (img.empty() || !label) return;

    cv::Mat display;
    if (img.channels() == 1)
        cv::cvtColor(img, display, COLOR_GRAY2BGR);
    else
        display = img.clone();

    QImage qimg(display.data, display.cols, display.rows, display.step, QImage::Format_BGR888);
    label->setPixmap(QPixmap::fromImage(qimg).scaled(label->size(), Qt::KeepAspectRatio));
}

void MainWindow::applyFilters() {
    // Placeholder por si está conectado en el .ui
    QMessageBox::information(this, "Aplicar", "Este botón no tiene funcionalidad asignada.");
}

void MainWindow::guardarEstadisticas(const QString& filename, int sliceIndex, double mean, int minVal, int maxVal, int area) {
    QFile file(filename);
    bool existe = file.exists();
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        if (!existe) {
            out << "Slice,Media,Minimo,Maximo,Area\n";  // encabezado
        }
        out << sliceIndex << "," << mean << "," << minVal << "," << maxVal << "," << area << "\n";
        file.close();
    }
}


