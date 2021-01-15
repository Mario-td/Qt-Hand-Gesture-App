#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "capture_thread.h"

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QGraphicsRectItem>
#include <QGraphicsProxyWidget>
#include <QKeyEvent>
#include <QList>
#include <QGraphicsOpacityEffect>
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

private:
    void initUI();
    void keepButtonSpace();
    void displayCamera();
    void setupGif(QLabel *gif, QMovie *movieGif, QGraphicsProxyWidget *graphicsProxyGif,
                  const QString &path, const int &PosX, const int &PosY);

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void updateFrame(cv::Mat *);
    void updateWindowWhileRecording();
    void updateWindowAfterRecording();
    void updateWindowAfterPredicting(const char *gestureName);
    void resetUI();
    void askForUserCommands();
    void giveUserInstructions();

private:
    // UI elements
    QLabel *predictionText;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QDir dirImages;

    struct SceneGif {
        QLabel *label;
        QMovie *movie;
        QGraphicsItem *graphics;
        QGraphicsProxyWidget *graphicsProxy;
        SceneGif()
        {
            label = new QLabel();
            movie = new QMovie(label);
            graphics = new QGraphicsRectItem();
            graphicsProxy = new QGraphicsProxyWidget(graphics);
        }
    };

    SceneGif *robotGif;
    SceneGif *actionGif;

    QPushButton *recordButton;

    QList<QMovie *> *gestureGifMovieList;
    QList<QLabel *> *gestureGifList;
    QList<QLabel *> *gestureNameList;
    QList<QGraphicsOpacityEffect *> *opacityEffect;

    cv::Mat currentFrame;

    // for capture thread
    QMutex *displayedDataLock;
    CaptureThread *capturer;

    // for predicting thread
    PredictGestureThread *classifier;
};
#endif // MAINWINDOW_H
