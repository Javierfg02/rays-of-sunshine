#pragma once

#include "realtime.h"
#include "./utils/aspectratiowidget.hpp"
#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    Realtime *realtime;
    AspectRatioWidget *aspectRatioWidget;

};
