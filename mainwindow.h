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

    void setStandard(double standard){ _standard = standard; }
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void loadImage(QString);

public slots:
    bool init();
    void export_impl();
    void import();
    void export_ellipses(bool isClear);

private slots:
    void scaleImage(int);
    void applyAnalyze();
    void applyFilter();
    void log(QString);
    void showDiscription(int);
    void mousePressed(QPoint);
    void startSettingStandrd(){ isEnteringStandard=true; }
    void deleteEllipse();
    void destructResult();

private:
    Ui::MainWindow *ui;
    QString fileLocation;
    QString fileFullPath;
    bool isEnteringStandard=false;
    double _standard;

    enum STATE{
        NO_IMAGE=0,
        NO_ANALYZE,
        LOADED,
        ANALYZED
    };

    void setState(STATE);
    //cv::Mat input;
};

#endif // MAINWINDOW_H
