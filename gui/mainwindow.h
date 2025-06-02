#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QSpinBox>
#include <opencv2/opencv.hpp>
#include "brats_loader.h"

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadImage();
    void applyFilters();
    void procesarSlice();  // Nueva función para procesar slice individual

private:
    void showOriginal(const cv::Mat& img);
    void showResult(const cv::Mat& img, QLabel* label);

    Ui::MainWindow *ui;

    BratsData patient;
    std::string currentModality = "flair";
    cv::Mat currentImage;
};

#endif // MAINWINDOW_H
