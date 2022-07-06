#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "capture_thread.h"
#include "resources.h"

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QKeyEvent>
#include <QList>
#include <QMovie>
#include <QStatusBar>
#include <QLabel>
#include <QAction>
#include <QDir>
#include <QTimer>

constexpr int camID = 0;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void initializeUIComponents();
    void initializeCamera();
    void setupGif(QLabel *gif, QMovie *movieGif, QGraphicsProxyWidget *graphicsProxyGif,
                  const QString &path, const int &PosX, const int &PosY);

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void updateFrame(cv::Mat *);
    void startRecording();
    void updateWindowAfterRecording();
    void updateWindowAfterPredicting(int ii);
    void resetUI();
    void askToPressButton();
    void giveUserInstructions();

private:
    // UI elements
    QLabel *topText;
    QGraphicsDropShadowEffect *topTextShadow;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

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
        ~SceneGif()
        {
            delete label;
            delete graphics;
        }
    };

    SceneGif *robotGif;
    SceneGif *actionGif;

    QPushButton *recordButton;
    QGraphicsDropShadowEffect *recordButtonShadow;

    QList<QMovie *> *gestureGifMovieList;
    QList<QLabel *> *gestureGifList;
    QList<QGraphicsOpacityEffect *> *gifOpacityEffect;
    QList<QLabel *> *gestureNameList;
    QList<QGraphicsDropShadowEffect *> *gestureNameTextShadow;

    cv::Mat currentFrame;

    // for capture thread
    QMutex *displayFrameLock = new QMutex();
    CaptureThread *capturer;

    // for predicting thread
    PredictGestureThread *classifier;
    bool displaying = false;
};
#endif // MAINWINDOW_H
