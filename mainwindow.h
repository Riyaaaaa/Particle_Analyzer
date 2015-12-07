#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <string>
#include "imageview.h"

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

private:
    Ui::MainWindow *ui;
    //cv::Mat input;
};

#endif // MAINWINDOW_H
