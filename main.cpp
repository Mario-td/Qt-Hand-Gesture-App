#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile file("stylesheet.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);

    MainWindow window;
    window.setWindowTitle("Hand Gesture Classification");
    window.setWindowState(Qt::WindowMaximized);
    window.show();
    return app.exec();
}
