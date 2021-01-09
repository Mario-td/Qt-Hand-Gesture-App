#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qApp->setStyleSheet("MainWindow {"
                        "background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:0, y2:0,"
                        "stop:0 #9b63f8, stop:1 #809ffc)"
                        "}"
                        "QGraphicsView { "
                        "background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:0, y2:0,"
                        "stop:0 #989CE2, stop:1 #c1c3f3);"
                        "border: 0px;"
                        "}"
                        "QPushButton {"
                        "border-style: solid;"
                        "border-width: 2px;"
                        "border-radius: 10px;"
                        "border-color: rgba(0, 0, 0, 0);"
                        "background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:0,"
                        "stop:0 #F4578E, stop:1 #EC3676);"
                        "color: white;"
                        "min-width: 6em;"
                        "}"
                        "QLabel {"
                        "color: white"
                        "}");
    MainWindow window;
    window.setWindowTitle("Hand Gesture Classification");
    window.show();
    return app.exec();
}
