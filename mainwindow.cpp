#include "opencv2/videoio.hpp"

// probably won't be included here
#undef slots
#include "torch/script.h"
#define slots Q_SLOTS

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , capturer(nullptr)
{
    initUI();
    dataLock = new QMutex();
    displayCamera();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI()
{
    this->resize(800, 800);

    // setup a grid layout
    QGridLayout *mainLayout = new QGridLayout();

    // setup area for the text of the predicted gesture
    predictionText = new QLineEdit(this);
    predictionText->setReadOnly(true);
    predictionText->setText("Hi there!");
    predictionText->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(predictionText, 0, 0, Qt::AlignHCenter);

    // setup area for image display
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    mainLayout->addWidget(imageView, 1, 0, 12, 1);

    // setup area for the record button
    recordButton = new QPushButton(this);
    recordButton->setText("Record");
    mainLayout->addWidget(recordButton, 13, 0, Qt::AlignHCenter);

    // setup area for the gestures list
    gestureList = new QListView(this);
    mainLayout->addWidget(gestureList, 14, 0, 4, 1);

    QWidget *widget = new QWidget();
    widget->setLayout(mainLayout);
    setCentralWidget(widget);

    // setup status bar
    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);
    mainStatusBar->addPermanentWidget(mainStatusLabel);
    mainStatusLabel->setText("click \'Record\' or space bar and start performing one of the gestures listed above");
}

void MainWindow::createActions()
{

}

void MainWindow::displayCamera()
{

//    if (capturer != nullptr) {
//        capturer->setRunning(false);
//        disconnect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
//    }
    int camID = 0;
    capturer = new CaptureThread(camID, dataLock);
    connect(capturer, &CaptureThread::frameCaptured, this, &MainWindow::updateFrame);
    capturer->start();
}

void MainWindow::updateFrame(cv::Mat *mat)
{
    dataLock->lock();
    currentFrame = *mat;
    dataLock->unlock();

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
