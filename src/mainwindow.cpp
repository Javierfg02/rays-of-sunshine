#include "mainwindow.h"
#include "settings.h"
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
    // Create main window and layout
    realtime = new Realtime;
    aspectRatioWidget = new AspectRatioWidget(this);
    aspectRatioWidget->setAspectWidget(realtime, 3.f/4.f);

    // Create main layout
    QHBoxLayout *hLayout = new QHBoxLayout(); // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);

    // Add widgets to layout
    hLayout->addWidget(aspectRatioWidget); // Changed from vLayout to hLayout

    // Set the layout stretch factor
    hLayout->setStretchFactor(aspectRatioWidget, 1);

    // Set size policy for widgets
    aspectRatioWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    realtime->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Set minimum size
    this->setMinimumSize(640, 480);

    // Set the layout for the main window
    QWidget *widget = new QWidget();
    widget->setLayout(hLayout);
    this->setCentralWidget(widget);
}

void MainWindow::finish() {
    return;
}
