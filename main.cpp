#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  MainWindow window;
  window.setWindowTitle("Hand Gesture Classification");
  window.show();
  return app.exec();
}
