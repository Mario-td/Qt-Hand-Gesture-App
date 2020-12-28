#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QList>
#include <QMovie>
#include <QStatusBar>
#include <QLabel>
#include <QAction>
#include <QDir>

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
    void populateGestureVector();

private slots:
    void updateFrame(cv::Mat *);
    void recordingGesture();

private:
    QLineEdit *predictionText;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QPushButton *recordButton;

    QList<QMovie *> *gifMovieList;
    QList<QLabel *> *gifList;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

    cv::Mat currentFrame;

    // for capture thread
    QMutex *displayedDataLock;
    CaptureThread *capturer;
};
#endif // MAINWINDOW_H
