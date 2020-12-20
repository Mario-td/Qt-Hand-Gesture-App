#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QListView>
#include <QStatusBar>
#include <QLabel>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  void initUI();

private:
  QLineEdit *predictionText;

  QGraphicsScene *imageScene;
  QGraphicsView *imageView;

  QPushButton *recordButton;

  QListView *gestureList;

  QStatusBar *mainStatusBar;
  QLabel *mainStatusLabel;
};
#endif // MAINWINDOW_H
