#include <QApplication>
#include <QCommandLineParser>
#include <QSurfaceFormat>
#include <QString>
#include <iostream>
#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("Ray of Sunshine");
    QCoreApplication::setOrganizationName("CS 1230");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    // argument based parsing
    QCommandLineParser parser;
    parser.addPositionalArgument("input", "Input scene filename (e.g. cube)");
    parser.process(a);

    const QStringList args = parser.positionalArguments();
    if (args.size() >= 1) {
        settings.inputFile = args.at(0).toStdString();
    } else {
        std::cerr << "Usage: " << argv[0] << " <input file name>" << std::endl;
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

    int return_val = a.exec();
    w.finish();
    return return_val;
}
