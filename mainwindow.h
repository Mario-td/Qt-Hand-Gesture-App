#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "capture_thread.h"

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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initUI();
    void displayCamera();
    void populateGestureVector();

private slots:
    void updateFrame(cv::Mat *);
    void recordingGesture();
    void updateWindowAfterRecording(int *elapsedTime);
    void updateWindowAfterPredicting(const char *gestureName);

private:
    // UI elements
    QLabel *predictionText;

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

    // for predicting thread
    PredictGestureThread *classifier;
};
#endif // MAINWINDOW_H
