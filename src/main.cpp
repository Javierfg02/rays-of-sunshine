#include <QApplication>
#include <QCommandLineParser>
#include <QSurfaceFormat>
#include <QString>
#include <iostream>
#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addPositionalArgument("input", "Input scene filename (e.g. ambient.json)");
    parser.addPositionalArgument("output", "Output image filename (e.g. ambient.png)");
    parser.process(a);

    const QStringList args = parser.positionalArguments();
    if (args.size() >= 2) {
        settings.inputFile = args.at(0).toStdString();
        settings.outputFile = args.at(1).toStdString();
    } else {
        std::cerr << "Usage: " << argv[0] << " <input.json> <output.png>" << std::endl;
        return 1;
    }

    QSurfaceFormat fmt;
    fmt.setVersion(4, 1);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow w;
    w.initialize();
    w.resize(800, 600);
    w.show();

    return a.exec();
}
