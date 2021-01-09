﻿#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , capturer(nullptr)
{
    initUI();
    displayedDataLock = new QMutex();
    displayCamera();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI()
{
    this->resize(1000, 700);

    // setup a grid layout
    QGridLayout *mainLayout = new QGridLayout();

    // setup area for the text of the predicted gesture
    predictionText = new QLabel(this);
    predictionText->setText("Hi there!");
    predictionText->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(predictionText, 0, 2, Qt::AlignHCenter);

    // setup area for image display
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    mainLayout->addWidget(imageView, 1, 0, 9, 5);

    // setup area for the record button
    recordButton = new QPushButton(this);
    recordButton->setText("Record");
    mainLayout->addWidget(recordButton, 10, 2, Qt::AlignHCenter);
    connect(recordButton, SIGNAL(clicked(bool)), this, SLOT(updateWindowWhileRecording()));

    // setup area for the gesture gif list
    gifList = new QList<QLabel *>;
    gifMovieList = new QList<QMovie *>;
    QDir dir("./images");
    QStringList nameFilters;
    nameFilters << "*.gif";
    QFileInfoList files = dir.entryInfoList(
                              nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
    // populates the gif list
    for (int i = 0, n = files.size(); i < n; ++i) {
        gifList->append(new QLabel());
        gifMovieList->append(new QMovie(gifList->back()));
        gifMovieList->back()->setFileName(files[i].filePath());
        gifList->back()->setMovie(gifMovieList->back());

        // gets the size of the gif to resize it
        gifMovieList->back()->jumpToFrame(0);
        QSize gifSize = gifMovieList->back()->currentImage().size();
        gifMovieList->back()->setScaledSize(gifSize * 0.65);
        gifMovieList->back()->start();
        mainLayout->addWidget(gifList->back(), 11, i, Qt::AlignHCenter);

        // setup the name of the gestures
        QString name = files[i].baseName();
        QLabel *gestureName = new QLabel(this);
        gestureName->setText(name);
        gestureName->setAlignment(Qt::AlignHCenter);
        mainLayout->addWidget(gestureName, 12, i, Qt::AlignHCenter);
    }

    QWidget *widget = new QWidget(this);
    widget->setLayout(mainLayout);
    setCentralWidget(widget);
}

void MainWindow::displayCamera()
{
    // set the used camera ID
    int camID = 0;
    capturer = new CaptureThread(camID, displayedDataLock);

    // creates a capture thread object and connects the signals to the mainwindow slots
    connect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
    connect(capturer, &CaptureThread::finishedRecording, this, &MainWindow::updateWindowAfterRecording);
    capturer->start();

    // creates a predict gesture thread object and connects the signals to the mainwindow slots
    classifier = new PredictGestureThread(capturer->getRunning(), capturer->getPredictingFrames(),
                                          capturer->getPredictingDataLock());
    connect(classifier, &PredictGestureThread::finishedPrediction, this,
            &MainWindow::updateWindowAfterPredicting);
    classifier->start();
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
    predictionText->setText(QString("You can stop"));
}

void MainWindow::updateWindowAfterPredicting(const char *gestureName)
{
    predictionText->setText(QString("You performed ")
                            + QString(gestureName));
    recordButton->setVisible(true);
}
