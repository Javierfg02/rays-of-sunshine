#include "mainwindow.h"
#include "settings.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("Rays of Sunshine");
}

void MainWindow::initialize() {
    realtime = new Realtime;
    aspectRatioWidget = new AspectRatioWidget(this);
    aspectRatioWidget->setAspectWidget(realtime, 3.f/4.f);

    // Create main layout
    QHBoxLayout *hLayout = new QHBoxLayout();

    // Create save button
    QPushButton *saveButton = new QPushButton("Save Image");
    saveButton->setFixedWidth(100);  // Control button width

    // Add to layout
    hLayout->addWidget(aspectRatioWidget, 1);
    hLayout->addWidget(saveButton, 0, Qt::AlignBottom);  // Align bottom right

    // Set widget properties
    aspectRatioWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    realtime->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setMinimumSize(640, 480);

    // Set central widget
    QWidget *widget = new QWidget();
    widget->setLayout(hLayout);
    this->setCentralWidget(widget);

    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveImage);
}

void MainWindow::onSaveImage() {
    std::string outputPath = std::string(PROJECT_PATH) + "/scenefiles/outputs/" + settings.inputFile + ".png";
    realtime->saveViewportImage(outputPath);
    std::cout << "Image saved to: " << outputPath << std::endl;
}

void MainWindow::finish() {
    return;
}
