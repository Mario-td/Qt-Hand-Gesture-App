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
    this->resize(1100, 700);

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
    mainLayout->addWidget(imageView, 1, 0, 8, 1);

    // setup area for the record button
    recordButton = new QPushButton(this);
    recordButton->setText("Record");
    mainLayout->addWidget(recordButton, 9, 0, Qt::AlignHCenter);

    // setup area for the gestures list
    gestureList = new QListView(this);
    gestureList->setViewMode(QListView::IconMode);
    gestureList->setResizeMode(QListView::Adjust);
    gestureList->setSpacing(7);
    gestureList->setWrapping(false);
    listModel = new QStandardItemModel(this);
    gestureList->setModel(listModel);
    mainLayout->addWidget(gestureList, 10, 0, 4, 1);

    QWidget *widget = new QWidget();
    widget->setLayout(mainLayout);
    setCentralWidget(widget);

    // setup status bar
    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);
    mainStatusBar->addPermanentWidget(mainStatusLabel);
    mainStatusLabel->setText("click \'Record\' or space bar and start performing one of the gestures listed above");

    populateGestureList();
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

void MainWindow::populateGestureList()
{
    QDir dir("./images");
    QStringList nameFilters;
    nameFilters << "*.gif";
    QFileInfoList files = dir.entryInfoList(
                              nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);

    foreach (QFileInfo cover, files) {
        QString name = cover.baseName();
        QStandardItem *item = new QStandardItem();
        listModel->appendRow(item);
        QModelIndex index = listModel->indexFromItem(item);
        listModel->setData(index, QPixmap(cover.absoluteFilePath()).scaledToHeight(150),
                           Qt::DecorationRole);
        listModel->setData(index, name, Qt::DisplayRole);
    }
}

void MainWindow::updateFrame(cv::Mat *mat)
{
    dataLock->lock();
    currentFrame = *mat;
    cv::resize(currentFrame, currentFrame, cv::Size(), 0.75, 0.75, cv::INTER_LINEAR);
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
