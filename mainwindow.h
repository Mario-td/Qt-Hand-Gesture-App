#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "capture_thread.h"
#include "resources.h"

#include <QMainWindow>
#include <QGridLayout>
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
#include <QLabel>
#include <QDir>
#include <QTimer>

#define CAM_ID 0
#define INTRO_MSG "Hi! Give me commands with your hand"
#define INSTRUCTIONS_MSG "make some of the gestures listed below"
#define ASK_MSG "click \"Record\" or press spacebar and start"
#define THINK_MSG "Let me think..."
#define GUESSED_MSG "You made "
#define TRY_AGAIN_MSG "Try again"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    // GUI elements
    QLabel *topText = new QLabel(this);;
    QGraphicsDropShadowEffect *topTextShadow = new QGraphicsDropShadowEffect();;

    QGraphicsScene *imageScene = new QGraphicsScene(this);
    QGraphicsView *imageView = new QGraphicsView(imageScene);

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

    SceneGif *robotGif = new SceneGif();
    SceneGif *actionGif = new SceneGif();

    QPushButton *recordButton = new QPushButton(this);
    QGraphicsDropShadowEffect *recordButtonShadow = new QGraphicsDropShadowEffect();

    QList<QLabel *> *gestureGifList = new QList<QLabel *>;
    QList<QMovie *> *gestureGifMovieList = new QList<QMovie *>;
    QList<QLabel *> *gestureNameList = new QList<QLabel *>;
    QList<QGraphicsOpacityEffect *> *gifOpacityEffect = new QList<QGraphicsOpacityEffect *>;
    QList<QGraphicsDropShadowEffect *> *gestureNameTextShadow = new QList<QGraphicsDropShadowEffect *>;

    QMutex *displayFrameLock = new QMutex();
    CaptureThread *capturer = new CaptureThread(CAM_ID, displayFrameLock);
    cv::Mat currentFrame;

    void initializeUIComponents();
    void startCapturerThread();
    void setupGif(QLabel *gif, QMovie *movieGif, QGraphicsProxyWidget *graphicsProxyGif,
                  const QString &path, const int &PosX, const int &PosY);

private slots:
    void updateFrame(cv::Mat *);
    void startRecording();
    void updateWindowAfterRecording();
    void updateWindowAfterPredicting(int ii);
    void resetUI();
    void askToPressButton();
    void giveUserInstructions();
};
#endif // MAINWINDOW_H
