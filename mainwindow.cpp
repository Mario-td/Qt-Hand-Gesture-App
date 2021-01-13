#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), dirImages("./images"), capturer(nullptr), classifier(nullptr)
{
    initUI();
    displayedDataLock = new QMutex();
    appStartup();
}

MainWindow::~MainWindow()
{
    delete predictionText;

    delete imageScene;
    delete imageView;

    delete robotGif;
    delete actionGif;

    delete recordButton;

    for (auto &gestureGifMovie : *gestureGifMovieList) {
        delete gestureGifMovie;
    }
    delete gestureGifMovieList;

    for (auto &gestureGif : *gestureGifList) {
        delete gestureGif;
    }
    delete gestureGifList;
}

void MainWindow::initUI()
{
    // setup a grid layout
    QGridLayout *mainLayout = new QGridLayout();

    // setup area for the text of the predicted gesture
    predictionText = new QLabel(this);
    QFont predictionTextFont("Sans serif", 40, QFont::Bold);
    predictionText->setText("Hi! Give me commands with your hand");
    predictionText->setFont(predictionTextFont);
    predictionText->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(predictionText, 0, 0, 1, 5);

    // setup area for image display
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    mainLayout->addWidget(imageView, 1, 0, 9, 5);

    // setup the scene gifs
    actionGif = new SceneGif();
    robotGif = new SceneGif();
    setupGif(robotGif->label, robotGif->movie, robotGif->graphicsProxy,
             dirImages.path() + "/robot.gif", 0, 20);
    imageScene->addItem(robotGif->graphicsProxy);

    // setup area for the record button
    recordButton = new QPushButton(this);
    QFont recordButtonFont("Serif", 15, QFont::Bold);
    recordButton->setText("Record");
    recordButton->setFont(recordButtonFont);
    mainLayout->addWidget(recordButton, 10, 2, Qt::AlignHCenter);
    connect(recordButton, SIGNAL(clicked(bool)), this, SLOT(updateWindowWhileRecording()));

    // setup area for the gesture gif list
    gestureGifList = new QList<QLabel *>;
    gestureGifMovieList = new QList<QMovie *>;
    QStringList nameFilters;
    nameFilters << "*.gif";
    QFileInfoList files = QDir(dirImages.path() + "/gestures/").entryInfoList(
                              nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
    QFont gestureNameFont("Sans Serif", 15, QFont::Bold);

    // populates the gesture gif list
    for (int i = 0, n = files.size(); i < n; ++i) {
        gestureGifList->append(new QLabel());
        gestureGifMovieList->append(new QMovie(gestureGifList->back()));
        gestureGifMovieList->back()->setFileName(files[i].filePath());
        gestureGifList->back()->setMovie(gestureGifMovieList->back());

        // gets the size of the gif to resize it
        gestureGifMovieList->back()->jumpToFrame(0);
        QSize gifSize = gestureGifMovieList->back()->currentImage().size();
        gestureGifMovieList->back()->setScaledSize(gifSize * 0.65);
        gestureGifMovieList->back()->start();
        mainLayout->addWidget(gestureGifList->back(), 11, i, Qt::AlignHCenter);

        // setup the name of the gestures
        QString name = files[i].baseName();
        QLabel *gestureName = new QLabel(this);
        gestureName->setText(name);
        gestureName->setFont(gestureNameFont);
        gestureName->setAlignment(Qt::AlignHCenter);
        mainLayout->addWidget(gestureName, 12, i, Qt::AlignHCenter);
    }

    QWidget *widget = new QWidget(this);
    widget->setLayout(mainLayout);
    setCentralWidget(widget);
}

void MainWindow::appStartup()
{
    // keeps the widget space after making it invisible
    QSizePolicy retainButtonSpace = recordButton->sizePolicy();
    retainButtonSpace.setRetainSizeWhenHidden(true);
    recordButton->setSizePolicy(retainButtonSpace);
    recordButton->setVisible(false);

    displayCamera();
}

void MainWindow::displayCamera()
{
    // set the used camera ID
    int camID = 0;
    capturer = new CaptureThread(camID, displayedDataLock);

    // creates a capture thread object and connects the signals to the mainwindow slots
    connect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
    connect(capturer, &CaptureThread::finishedRecording, this, &MainWindow::updateWindowAfterRecording);
    connect(capturer, &CaptureThread::cameraReady, this, &MainWindow::askForUserCommands);
    connect(capturer, &CaptureThread::howToUseInfo, this, &MainWindow::giveUserInstructions);

    // creates a predict gesture thread object and connects the signals to the mainwindow slots
    classifier = new PredictGestureThread(capturer->getRunning(), capturer->getPredictingFrames(),
                                          capturer->getPredictingDataLock());
    connect(classifier, &PredictGestureThread::finishedPrediction, this,
            &MainWindow::updateWindowAfterPredicting);
    connect(classifier, &PredictGestureThread::resetPrediction, this,
            &MainWindow::setRecordButtonVisible);

    // connects capturer and classifier for restarting a prediction
    connect(classifier, &PredictGestureThread::resetPrediction, capturer,
            &CaptureThread::setDisplaying);

    // runs both threads
    capturer->start();
    classifier->start();
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

void MainWindow::askForUserCommands()
{
    predictionText->setText("make some of the gestures listed below");
}

void MainWindow::giveUserInstructions()
{
    predictionText->setText("click \"Record\" or press spacebar and start");
    recordButton->setVisible(true);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && recordButton->isVisible())
        updateWindowWhileRecording();
    else if (event->key() == Qt::Key_Escape)
        this->close();
}

void MainWindow::updateFrame(cv::Mat *mat)
{
    displayedDataLock->lock();
    currentFrame = *mat;
    displayedDataLock->unlock();

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

void MainWindow::updateWindowWhileRecording()
{
    capturer->startIntervalTimer();
    capturer->setRecording(true);
    predictionText->setText("");

    // keeps the widget space after making it invisible
    QSizePolicy retainButtonSpace = recordButton->sizePolicy();
    retainButtonSpace.setRetainSizeWhenHidden(true);
    recordButton->setSizePolicy(retainButtonSpace);
    recordButton->setVisible(false);
}

void MainWindow::updateWindowAfterRecording()
{
    imageScene->clear();
    delete robotGif;
    delete actionGif;
    robotGif = new SceneGif();
    actionGif = new SceneGif();
    setupGif(robotGif->label, robotGif->movie, robotGif->graphicsProxy,
             dirImages.path() + "/robot.gif", 0, 20);
    setupGif(actionGif->label, actionGif->movie, actionGif->graphicsProxy,
             dirImages.path() + "/waiting.gif", 210, 0);
    qDebug() << imageScene->items();
    imageScene->addItem(robotGif->graphicsProxy);
    imageScene->addItem(actionGif->graphicsProxy);
    imageView->setSceneRect(robotGif->label->rect());
    imageScene->update();

    predictionText->setText(QString("Let me think..."));
}

void MainWindow::updateWindowAfterPredicting(const char *gestureName)
{
    imageScene->removeItem(actionGif->graphicsProxy);
    delete actionGif;
    actionGif = new SceneGif();
    setupGif(actionGif->label, actionGif->movie, actionGif->graphicsProxy,
             dirImages.path() + "/bulb.gif", 210, -10);
    qDebug() << imageScene->items();
    imageScene->addItem(actionGif->graphicsProxy);
    imageScene->update();

    predictionText->setText(QString("You made ")
                            + QString(gestureName) + QString("!"));
}

void MainWindow::setRecordButtonVisible()
{
    recordButton->setVisible(true);
}
