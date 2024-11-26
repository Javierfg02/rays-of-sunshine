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

    // Create main vertical layout
    QVBoxLayout *mainLayout = new QVBoxLayout();

    // Add aspect ratio widget to fill most of the space
    mainLayout->addWidget(aspectRatioWidget, 1);

    // Create container widget for the button
    QWidget *buttonContainer = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);

    // Create save button
    QPushButton *saveButton = new QPushButton("Save Image");
    saveButton->setFixedWidth(100);

    // Center the button in the container
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addStretch();

    // Add button container to main layout
    mainLayout->addWidget(buttonContainer);

    // Add button layout to main layout
    mainLayout->addLayout(buttonLayout);
    mainLayout->setAlignment(buttonLayout, Qt::AlignBottom);

    // Set widget properties
    aspectRatioWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    realtime->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setMinimumSize(640, 480);

    // Set central widget
    QWidget *widget = new QWidget();
    widget->setLayout(mainLayout);
    this->setCentralWidget(widget);

    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveImage);
}

void MainWindow::onSaveImage() {
    std::string outputPath = std::string(PROJECT_PATH) + "/scenefiles/outputs/" + settings.outputFile + ".png";
    realtime->saveViewportImage(outputPath);
    std::cout << "Image saved to: " << outputPath << std::endl;
}

void MainWindow::finish() {
    return;
}
