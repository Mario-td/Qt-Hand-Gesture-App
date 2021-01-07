#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qApp->setStyleSheet("MainWindow { background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:0, y2:0, "
                        "stop:0 #9b63f8, stop:1 #809ffc) }"
                        "QGraphicsView { background-color: #c1c3f3 }"
                        "QPushButton {"
                        "border-style: solid;"
                        "border-width: 2px;"
                        "border-radius: 10px;"
                        "border-color: #f97da8;"
                        "background-color: #f97da8;"
                        "color: white;"
                        "min-width: 6em;"
                        "box-shadow: 10px 10px 8px 10px black;"
                        "}"
                        "QLabel {"
                        "color: white"
                        "}");
    MainWindow window;
    window.setWindowTitle("Hand Gesture Classification");
    window.show();
    return app.exec();
}
