#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
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
    void loadImage();         // Cargar imagen médica
    void applyFilters();     // Aplicar filtros seleccionados
    void updateNoiseParams(); // Actualizar valores mostrados

private:
    void showOriginal(const cv::Mat& img);
    void showResult(const cv::Mat& img, QLabel* label);

    Ui::MainWindow *ui;

    cv::Mat currentImage;
};

#endif // MAINWINDOW_H