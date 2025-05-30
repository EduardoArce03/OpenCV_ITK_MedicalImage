/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QHBoxLayout *buttonLayout;
    QPushButton *loadButton;
    QPushButton *applyButton;
    QGroupBox *noiseGroup;
    QVBoxLayout *noiseLayout;
    QHBoxLayout *meanLayout;
    QLabel *meanLabel;
    QSlider *meanSlider;
    QLabel *meanValueLabel;
    QHBoxLayout *stdLayout;
    QLabel *stdLabel;
    QSlider *stdSlider;
    QLabel *stdValueLabel;
    QHBoxLayout *varLayout;
    QLabel *varLabel;
    QSlider *varSlider;
    QLabel *varValueLabel;
    QLabel *originalLabel;
    QScrollArea *resultsScroll;
    QWidget *scrollContent;
    QVBoxLayout *resultsLayout;
    QHBoxLayout *lightingRow;
    QLabel *eqHistLabel;
    QLabel *claheLabel;
    QLabel *gammaLabel;
    QHBoxLayout *noiseRow;
    QLabel *noisyGaussLabel;
    QLabel *noisySpeckleLabel;
    QHBoxLayout *smoothingRow;
    QLabel *blurLabel;
    QLabel *gaussianLabel;
    QLabel *medianLabel;
    QHBoxLayout *edgesRow;
    QLabel *cannyLabel;
    QLabel *sobelLabel;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1200, 900);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        titleLabel = new QLabel(centralWidget);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        titleLabel->setAlignment(Qt::AlignCenter);
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        titleLabel->setFont(font);

        verticalLayout->addWidget(titleLabel);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName(QString::fromUtf8("buttonLayout"));
        loadButton = new QPushButton(centralWidget);
        loadButton->setObjectName(QString::fromUtf8("loadButton"));

        buttonLayout->addWidget(loadButton);

        applyButton = new QPushButton(centralWidget);
        applyButton->setObjectName(QString::fromUtf8("applyButton"));

        buttonLayout->addWidget(applyButton);


        verticalLayout->addLayout(buttonLayout);

        noiseGroup = new QGroupBox(centralWidget);
        noiseGroup->setObjectName(QString::fromUtf8("noiseGroup"));
        noiseLayout = new QVBoxLayout(noiseGroup);
        noiseLayout->setObjectName(QString::fromUtf8("noiseLayout"));
        meanLayout = new QHBoxLayout();
        meanLayout->setObjectName(QString::fromUtf8("meanLayout"));
        meanLabel = new QLabel(noiseGroup);
        meanLabel->setObjectName(QString::fromUtf8("meanLabel"));

        meanLayout->addWidget(meanLabel);

        meanSlider = new QSlider(noiseGroup);
        meanSlider->setObjectName(QString::fromUtf8("meanSlider"));
        meanSlider->setOrientation(Qt::Horizontal);
        meanSlider->setMinimum(0);
        meanSlider->setMaximum(100);
        meanSlider->setValue(0);

        meanLayout->addWidget(meanSlider);

        meanValueLabel = new QLabel(noiseGroup);
        meanValueLabel->setObjectName(QString::fromUtf8("meanValueLabel"));

        meanLayout->addWidget(meanValueLabel);


        noiseLayout->addLayout(meanLayout);

        stdLayout = new QHBoxLayout();
        stdLayout->setObjectName(QString::fromUtf8("stdLayout"));
        stdLabel = new QLabel(noiseGroup);
        stdLabel->setObjectName(QString::fromUtf8("stdLabel"));

        stdLayout->addWidget(stdLabel);

        stdSlider = new QSlider(noiseGroup);
        stdSlider->setObjectName(QString::fromUtf8("stdSlider"));
        stdSlider->setOrientation(Qt::Horizontal);
        stdSlider->setMinimum(0);
        stdSlider->setMaximum(100);
        stdSlider->setValue(25);

        stdLayout->addWidget(stdSlider);

        stdValueLabel = new QLabel(noiseGroup);
        stdValueLabel->setObjectName(QString::fromUtf8("stdValueLabel"));

        stdLayout->addWidget(stdValueLabel);


        noiseLayout->addLayout(stdLayout);

        varLayout = new QHBoxLayout();
        varLayout->setObjectName(QString::fromUtf8("varLayout"));
        varLabel = new QLabel(noiseGroup);
        varLabel->setObjectName(QString::fromUtf8("varLabel"));

        varLayout->addWidget(varLabel);

        varSlider = new QSlider(noiseGroup);
        varSlider->setObjectName(QString::fromUtf8("varSlider"));
        varSlider->setOrientation(Qt::Horizontal);
        varSlider->setMinimum(0);
        varSlider->setMaximum(100);
        varSlider->setValue(4);

        varLayout->addWidget(varSlider);

        varValueLabel = new QLabel(noiseGroup);
        varValueLabel->setObjectName(QString::fromUtf8("varValueLabel"));

        varLayout->addWidget(varValueLabel);


        noiseLayout->addLayout(varLayout);


        verticalLayout->addWidget(noiseGroup);

        originalLabel = new QLabel(centralWidget);
        originalLabel->setObjectName(QString::fromUtf8("originalLabel"));
        originalLabel->setAlignment(Qt::AlignCenter);
        originalLabel->setFrameShape(QFrame::Box);

        verticalLayout->addWidget(originalLabel);

        resultsScroll = new QScrollArea(centralWidget);
        resultsScroll->setObjectName(QString::fromUtf8("resultsScroll"));
        resultsScroll->setWidgetResizable(true);
        scrollContent = new QWidget();
        scrollContent->setObjectName(QString::fromUtf8("scrollContent"));
        resultsLayout = new QVBoxLayout(scrollContent);
        resultsLayout->setObjectName(QString::fromUtf8("resultsLayout"));
        lightingRow = new QHBoxLayout();
        lightingRow->setObjectName(QString::fromUtf8("lightingRow"));
        eqHistLabel = new QLabel(scrollContent);
        eqHistLabel->setObjectName(QString::fromUtf8("eqHistLabel"));
        eqHistLabel->setAlignment(Qt::AlignCenter);
        eqHistLabel->setFrameShape(QFrame::Box);

        lightingRow->addWidget(eqHistLabel);

        claheLabel = new QLabel(scrollContent);
        claheLabel->setObjectName(QString::fromUtf8("claheLabel"));
        claheLabel->setAlignment(Qt::AlignCenter);
        claheLabel->setFrameShape(QFrame::Box);

        lightingRow->addWidget(claheLabel);

        gammaLabel = new QLabel(scrollContent);
        gammaLabel->setObjectName(QString::fromUtf8("gammaLabel"));
        gammaLabel->setAlignment(Qt::AlignCenter);
        gammaLabel->setFrameShape(QFrame::Box);

        lightingRow->addWidget(gammaLabel);


        resultsLayout->addLayout(lightingRow);

        noiseRow = new QHBoxLayout();
        noiseRow->setObjectName(QString::fromUtf8("noiseRow"));
        noisyGaussLabel = new QLabel(scrollContent);
        noisyGaussLabel->setObjectName(QString::fromUtf8("noisyGaussLabel"));
        noisyGaussLabel->setAlignment(Qt::AlignCenter);
        noisyGaussLabel->setFrameShape(QFrame::Box);

        noiseRow->addWidget(noisyGaussLabel);

        noisySpeckleLabel = new QLabel(scrollContent);
        noisySpeckleLabel->setObjectName(QString::fromUtf8("noisySpeckleLabel"));
        noisySpeckleLabel->setAlignment(Qt::AlignCenter);
        noisySpeckleLabel->setFrameShape(QFrame::Box);

        noiseRow->addWidget(noisySpeckleLabel);


        resultsLayout->addLayout(noiseRow);

        smoothingRow = new QHBoxLayout();
        smoothingRow->setObjectName(QString::fromUtf8("smoothingRow"));
        blurLabel = new QLabel(scrollContent);
        blurLabel->setObjectName(QString::fromUtf8("blurLabel"));
        blurLabel->setAlignment(Qt::AlignCenter);
        blurLabel->setFrameShape(QFrame::Box);

        smoothingRow->addWidget(blurLabel);

        gaussianLabel = new QLabel(scrollContent);
        gaussianLabel->setObjectName(QString::fromUtf8("gaussianLabel"));
        gaussianLabel->setAlignment(Qt::AlignCenter);
        gaussianLabel->setFrameShape(QFrame::Box);

        smoothingRow->addWidget(gaussianLabel);

        medianLabel = new QLabel(scrollContent);
        medianLabel->setObjectName(QString::fromUtf8("medianLabel"));
        medianLabel->setAlignment(Qt::AlignCenter);
        medianLabel->setFrameShape(QFrame::Box);

        smoothingRow->addWidget(medianLabel);


        resultsLayout->addLayout(smoothingRow);

        edgesRow = new QHBoxLayout();
        edgesRow->setObjectName(QString::fromUtf8("edgesRow"));
        cannyLabel = new QLabel(scrollContent);
        cannyLabel->setObjectName(QString::fromUtf8("cannyLabel"));
        cannyLabel->setAlignment(Qt::AlignCenter);
        cannyLabel->setFrameShape(QFrame::Box);

        edgesRow->addWidget(cannyLabel);

        sobelLabel = new QLabel(scrollContent);
        sobelLabel->setObjectName(QString::fromUtf8("sobelLabel"));
        sobelLabel->setAlignment(Qt::AlignCenter);
        sobelLabel->setFrameShape(QFrame::Box);

        edgesRow->addWidget(sobelLabel);


        resultsLayout->addLayout(edgesRow);

        resultsScroll->setWidget(scrollContent);

        verticalLayout->addWidget(resultsScroll);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Vision por Computador - Im\303\241genes M\303\251dicas", nullptr));
        titleLabel->setText(QCoreApplication::translate("MainWindow", "Procesamiento de Im\303\241genes M\303\251dicas", nullptr));
        loadButton->setText(QCoreApplication::translate("MainWindow", "Cargar Imagen", nullptr));
        applyButton->setText(QCoreApplication::translate("MainWindow", "Aplicar Filtros", nullptr));
        noiseGroup->setTitle(QCoreApplication::translate("MainWindow", "Ruido", nullptr));
        meanLabel->setText(QCoreApplication::translate("MainWindow", "Media Gaussiana:", nullptr));
        meanValueLabel->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        stdLabel->setText(QCoreApplication::translate("MainWindow", "Desv. Est\303\241ndar:", nullptr));
        stdValueLabel->setText(QCoreApplication::translate("MainWindow", "25", nullptr));
        varLabel->setText(QCoreApplication::translate("MainWindow", "Varianza Speckle:", nullptr));
        varValueLabel->setText(QCoreApplication::translate("MainWindow", "4", nullptr));
        originalLabel->setText(QCoreApplication::translate("MainWindow", "No hay imagen cargada", nullptr));
        eqHistLabel->setText(QCoreApplication::translate("MainWindow", "Hist. Equal.", nullptr));
        claheLabel->setText(QCoreApplication::translate("MainWindow", "CLAHE", nullptr));
        gammaLabel->setText(QCoreApplication::translate("MainWindow", "Gamma Corr.", nullptr));
        noisyGaussLabel->setText(QCoreApplication::translate("MainWindow", "Ruido Gaussiano", nullptr));
        noisySpeckleLabel->setText(QCoreApplication::translate("MainWindow", "Ruido Speckle", nullptr));
        blurLabel->setText(QCoreApplication::translate("MainWindow", "Blur", nullptr));
        gaussianLabel->setText(QCoreApplication::translate("MainWindow", "Gaussian", nullptr));
        medianLabel->setText(QCoreApplication::translate("MainWindow", "Median", nullptr));
        cannyLabel->setText(QCoreApplication::translate("MainWindow", "Canny", nullptr));
        sobelLabel->setText(QCoreApplication::translate("MainWindow", "Sobel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
