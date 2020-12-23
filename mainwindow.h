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
#include <QAction>

#include "opencv2/opencv.hpp"

#include "capture_thread.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initUI();
    void createActions();
    void displayCamera();

private slots:
    void updateFrame(cv::Mat *);

private:
    QAction *recordAction;

    QLineEdit *predictionText;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QPushButton *recordButton;

    QListView *gestureList;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

    cv::Mat currentFrame;

    // for capture thread
    QMutex *dataLock;
    CaptureThread *capturer;
};
#endif // MAINWINDOW_H
