#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "capture_thread.h"

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
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
    void appIntro();
    void displayCamera();
    void populateGestureVector();
    void giveUserInstructions();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private slots:
    void updateFrame(cv::Mat *);
    void updateWindowWhileRecording();
    void updateWindowAfterRecording();
    void updateWindowAfterPredicting(const char *gestureName);

private:
    // UI elements
    QLabel *predictionText;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QPixmap *robotImage;
    QPixmap *bulbImage;

    QPushButton *recordButton;

    QList<QMovie *> *gifMovieList;
    QList<QLabel *> *gifList;

    cv::Mat currentFrame;

    // for capture thread
    QMutex *displayedDataLock;
    CaptureThread *capturer;

    // for predicting thread
    PredictGestureThread *classifier;
};
#endif // MAINWINDOW_H
