#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    initializeUIComponents();
    startCapturerThread();

    QTimer::singleShot(5000, this, &MainWindow::giveUserInstructions);
    QTimer::singleShot(10000, this, &MainWindow::askToPressButton);
}

void MainWindow::initializeUIComponents()
{
    QGridLayout *mainLayout = new QGridLayout();

    // Main text
    topTextShadow->setXOffset(2);
    topTextShadow->setYOffset(2);
    topTextShadow->setBlurRadius(8);
    topTextShadow->setColor(QColor(54, 100, 239, 200));
    QFont topTextFont("Sans serif", 40, QFont::Bold);
    topText->setText(INTRO_MSG);
    topText->setFont(topTextFont);
    topText->setAlignment(Qt::AlignCenter);
    topText->setGraphicsEffect(topTextShadow);
    mainLayout->addWidget(topText, 0, 0, 1, 5);

    // Image display
    mainLayout->addWidget(imageView, 1, 0, 9, 5);

    // Scene gifs
    setupGif(robotGif->label, robotGif->movie, robotGif->graphicsProxy,
             ROBOT_GIF_PATH, 0, 20);
    imageScene->addItem(robotGif->graphicsProxy);

    // Record button
    recordButtonShadow->setXOffset(2);
    recordButtonShadow->setYOffset(2);
    recordButtonShadow->setColor(QColor(200, 0, 71, 200));
    QFont recordButtonFont("Serif", 13, QFont::Bold);
    recordButton->setText("Record");
    recordButton->setFont(recordButtonFont);
    recordButton->setGraphicsEffect(recordButtonShadow);
    QSizePolicy retainButtonSpace = recordButton->sizePolicy();
    retainButtonSpace.setRetainSizeWhenHidden(true);
    recordButton->setSizePolicy(retainButtonSpace);
    recordButton->setVisible(false);
    mainLayout->addWidget(recordButton, 10, 2, Qt::AlignHCenter);
    connect(recordButton, SIGNAL(clicked(bool)), this, SLOT(startRecording()));

    // Gestures gifs
    QStringList nameFilters;
    nameFilters << "*.gif";
    QFileInfoList files = QDir(GESTURES_GIFS_PATH).entryInfoList(
                              nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
    QFont gestureNameFont("Sans Serif", 15, QFont::Bold);

    // Populates the gesture gif list
    for (int i = 0, n = files.size(); i < n; ++i) {
        gestureGifList->append(new QLabel());
        gestureGifMovieList->append(new QMovie(gestureGifList->back()));
        gestureGifMovieList->back()->setFileName(files[i].filePath());
        gestureGifList->back()->setMovie(gestureGifMovieList->back());

        // Resize gestures gifs
        gestureGifMovieList->back()->jumpToFrame(0);
        QSize gifSize = gestureGifMovieList->back()->currentImage().size();
        gestureGifMovieList->back()->setScaledSize(gifSize * 0.65);
        gestureGifMovieList->back()->start();
        mainLayout->addWidget(gestureGifList->back(), 11, i, Qt::AlignHCenter);

        // Gestures names
        QString name = files[i].baseName();
        gestureNameList->append(new QLabel(this));
        gestureNameList->back()->setText(name);
        gestureNameList->back()->setFont(gestureNameFont);
        gestureNameList->back()->setAlignment(Qt::AlignHCenter);
        mainLayout->addWidget(gestureNameList->back(), 12, i, Qt::AlignHCenter);

        // Graphics effects of the gestures images
        gifOpacityEffect->append(new QGraphicsOpacityEffect());
        gifOpacityEffect->back()->setOpacity(0.4);
        gifOpacityEffect->back()->setEnabled(false);
        gestureGifList->back()->setGraphicsEffect(gifOpacityEffect->back());

        // Graphics effects of the gesture names
        gestureNameTextShadow->append(new QGraphicsDropShadowEffect());
        gestureNameTextShadow->back()->setXOffset(2);
        gestureNameTextShadow->back()->setYOffset(2);
        gestureNameTextShadow->back()->setBlurRadius(8);
        gestureNameTextShadow->back()->setColor(QColor(102, 23, 231, 200));
        gestureNameList->back()->setGraphicsEffect(gestureNameTextShadow->at(i));
    }

    QWidget *widget = new QWidget(this);
    widget->setLayout(mainLayout);
    setCentralWidget(widget);
}

void MainWindow::startCapturerThread()
{
    // Signals and slots connections
    connect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
    connect(capturer, &CaptureThread::finishedRecording, this, &MainWindow::updateWindowAfterRecording);
    connect(&(capturer->worker), &WorkerThread::resultReady, this,
            &MainWindow::updateWindowAfterPredicting);

    capturer->start();
}

void MainWindow::setupGif(QLabel *gif, QMovie *movieGif, QGraphicsProxyWidget *graphicsProxyGif,
                          const QString &path, const int &PosX, const int &PosY)
{
    movieGif->setFileName(path);
    movieGif->start();
    gif->setAttribute( Qt::WA_NoSystemBackground );
    gif->setMovie(movieGif);
    graphicsProxyGif->setWidget(gif);
    graphicsProxyGif->setPos(PosX, PosY);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && recordButton->isVisible())
        startRecording();
    else if (event->key() == Qt::Key_Escape)
        this->close();
}

void MainWindow::giveUserInstructions()
{
    topText->setText(INSTRUCTIONS_MSG);
}

void MainWindow::askToPressButton()
{
    capturer->setDisplaying(true);
    delete robotGif;
    delete actionGif;
    topText->setText(ASK_MSG);
    recordButton->setVisible(true);
}

void MainWindow::updateFrame(cv::Mat *mat)
{
    displayFrameLock->lock();
    currentFrame = *mat;
    displayFrameLock->unlock();

    cv::resize(currentFrame, currentFrame, cv::Size(), 0.75, 0.75, cv::INTER_LINEAR);

    QImage frame(
        currentFrame.data,
        currentFrame.cols,
        currentFrame.rows,
        currentFrame.step,
        QImage::Format_RGB888);
    QPixmap image = QPixmap::fromImage(frame);

    imageScene->clear();
    imageView->resetMatrix();
    imageScene->addPixmap(image);
    imageScene->update();
    imageView->setSceneRect(image.rect());
}

void MainWindow::startRecording()
{
    capturer->setRecording(true);
    topText->setText("");
    recordButton->setVisible(false);
}

void MainWindow::updateWindowAfterRecording()
{
    // Clears the scene and adds the gif items
    imageScene->clear();
    robotGif = new SceneGif();
    robotGif->graphics->show();
    actionGif = new SceneGif();
    setupGif(robotGif->label, robotGif->movie, robotGif->graphicsProxy,
             ROBOT_GIF_PATH, 0, 20);
    setupGif(actionGif->label, actionGif->movie, actionGif->graphicsProxy,
             WAITING_GIF_PATH, 210, 0);
    imageScene->addItem(robotGif->graphicsProxy);
    imageScene->addItem(actionGif->graphicsProxy);
    imageView->setSceneRect(robotGif->label->rect());
    imageScene->update();

    topText->setText(THINK_MSG);
}

void MainWindow::updateWindowAfterPredicting(int ii)//
{
    // Change the waiting gif for the bulb
    imageScene->removeItem(actionGif->graphicsProxy);
    delete actionGif;
    actionGif = new SceneGif();
    setupGif(actionGif->label, actionGif->movie, actionGif->graphicsProxy,
             BULB_GIF_PATH, 210, -10);
    imageScene->addItem(actionGif->graphicsProxy);
    imageScene->update();

    topText->setText(QString(GUESSED_MSG)
                     + QString(gestureNameList->at(ii)->text()));
    for (int i = 0, n = gestureNameList->size(); i < n; i++) {
        if (i != ii) {
            gifOpacityEffect->at(i)->setEnabled(true);
            gestureNameList->at(i)->setHidden(true);
        }
    }

    QTimer::singleShot(3000, this, &MainWindow::resetUI);
}

void MainWindow::resetUI()
{
    capturer->setDisplaying(true);
    for (int i = 0, n = gestureNameList->size(); i < n; i++) {
        gifOpacityEffect->at(i)->setEnabled(false);
        gestureNameList->at(i)->setHidden(false);
    }
    delete actionGif;
    delete robotGif;
    topText->setText(TRY_AGAIN_MSG);
    recordButton->setVisible(true);
}
