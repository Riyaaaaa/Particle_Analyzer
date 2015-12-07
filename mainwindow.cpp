#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<string>
#include <iostream>
#include <QObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->scale_slider,SIGNAL(sliderMoved(int)),this,SLOT(scaleImage(int)));
    connect(ui->s_analysis,SIGNAL(clicked()),this,SLOT(applyAnalyze()));
    connect(ui->view,SIGNAL(log(QString)),this,SLOT(log(QString)));

    ui->thresholdBox->setMaximum(255);
    ui->thresholdBox->setMinimum(0);
    ui->maxBox->setMaximum(1000);
    ui->minBox->setMaximum(1000);

    ui->scale_slider->setValue(50);
    ui->thresholdBox->setValue(150);
    ui->minBox->setValue(10);
    ui->maxBox->setValue(500);



    cv::Mat src = cv::imread("/Users/riya/Particle_Analysis/Resource/image.png", CV_LOAD_IMAGE_GRAYSCALE);
    if(src.empty())exit(-1);
    ui->view->setImg(src);
}

void MainWindow::applyAnalyze(){
    int threshold = ui->thresholdBox->value();
    double min = ui->minBox->value();
    double max = ui->maxBox->value();

    ui->view->startAnalysis(threshold,min,max);
}

void MainWindow::log(QString str){
    std::cout << str.toStdString() << std::endl;
    ui->log->append(str + "\n");
}

void MainWindow::scaleImage(int value){
    static int oldvalue=50;
    ui->view->scaleView(oldvalue-value);
    oldvalue=value;
}

MainWindow::~MainWindow()
{
    delete ui;
}
