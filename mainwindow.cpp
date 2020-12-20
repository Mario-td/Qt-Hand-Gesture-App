#include "opencv2/opencv.hpp"

#undef slots
#include "torch/script.h"
#define slots Q_SLOTS

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  initUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI()
{
  this->resize(800, 600);

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
