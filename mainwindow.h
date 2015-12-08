#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <string>
#include "imageview.h"
#include "ellipse.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void scaleImage(int);
    void applyAnalyze();
    void log(QString);
    void showDiscription(int);
    void mousePressed(QPoint);

private:
    Ui::MainWindow *ui;
    bool isEnteringStandard=false;
    //cv::Mat input;
};

#endif // MAINWINDOW_H
