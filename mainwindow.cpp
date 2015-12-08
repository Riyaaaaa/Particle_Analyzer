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
    connect(ui->ellipseID,SIGNAL(valueChanged(int)),ui->view,SLOT(emphasisEllipse(int)));
    connect(ui->ellipseID,SIGNAL(valueChanged(int)),this,SLOT(showDiscription(int)));
    connect(ui->view,SIGNAL(mousePressed(QPoint)),this,SLOT(mousePressed(QPoint));

    ui->thresholdBox->setMaximum(255);
    ui->thresholdBox->setMinimum(0);
    ui->maxBox->setMaximum(1000);
    ui->minBox->setMaximum(1000);

    ui->scale_slider->setValue(50);
    ui->thresholdBox->setValue(150);
    ui->minBox->setValue(10);
    ui->maxBox->setValue(500);
    ui->ellipseID->setMinimum(0);
    ui->ellipseID->setMaximum(0);

    ui->delete_button->setEnabled(false);
    ui->scaling->setEnabled(false);

    cv::Mat src = cv::imread("/Users/riya/Particle_Analysis/Resource/image.png", CV_LOAD_IMAGE_GRAYSCALE);
    if(src.empty())exit(-1);
    ui->view->setImg(src);
}

void MainWindow::applyAnalyze(){
    int threshold = ui->thresholdBox->value();
    double min = ui->minBox->value();
    double max = ui->maxBox->value();

    ui->view->startAnalysis(threshold,min,max);
    ui->ellipseID->setMaximum( ui->view->getEllipses().size()-1 );
}

void MainWindow::log(QString str){
    std::cout << str.toStdString() << std::endl;
    ui->log->append(str + "\n");
}

void MainWindow::scaleImage(int value){
    static int oldvalue=50;
    ui->view->scaleView(value-oldvalue);
    oldvalue=value;
}

void MainWindow::showDiscription(int id){
    const Ellipse& ellipse = ui->view->getEllipses()[id];

    ui->discription->clear();
    ui->discription->append("Area: " + QString::number(ellipse.area));
    ui->discription->append("location: (" + QString::number(ellipse.rect.center.x) + "," + QString::number(ellipse.rect.center.y) + ")");
}

void MainWindow::mousePressed(QPoint pos){
    if(isEnteringStandard){

    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
