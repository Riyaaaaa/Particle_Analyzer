#include "mainwindow.h"
#include "imageview.h"
#include "ui_mainwindow.h"
#include<string>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ImageView* view = new ImageView;

    std::cout << "-------- read ----------" << std::endl;

    cv::waitKey(0);

    cv::Mat src = cv::imread("image.png");

    std::cout << "end" << std::endl;
    view->setImg(src);

    view->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
