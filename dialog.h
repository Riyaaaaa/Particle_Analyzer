#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <opencv2/opencv.hpp>

#define matrixSetter(_rows_,_cols_) void setElementM##_rows_##_cols_(QString str){ setElement< _rows_ -1, _cols_-1  >(str); }

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    enum FILTER_PRESET{
        SOBEL=0,
        LAPLACIAN,
        MEDIAN,
        BILATERAL,
        GAUSSIAN,
        SCHARR
    };

    bool isUsingPreset(){return _isUsingPreset;}
    cv::Mat getKernel(){ return _mat; }
    void setUsingPreset(bool isUsingPreset){ _isUsingPreset = isUsingPreset; }

    FILTER_PRESET getUsePreset(){ return _use_filter;}

    template<int ROWS,int COLS>
    void setElement(QString elem){
        checkValue(elem);
        _mat.at<float>(ROWS,COLS) = elem.toFloat();
    }
public slots:
    void stateChangedPreset(int);
    void stateChangedUser(int);
    void selectedPreset(bool);

    matrixSetter(1,1)
    matrixSetter(1,2)
    matrixSetter(1,3)
    matrixSetter(2,1)
    matrixSetter(2,2)
    matrixSetter(2,3)
    matrixSetter(3,1)
    matrixSetter(3,2)
    matrixSetter(3,3)

private:
    void checkValue(QString str);
    void setEnabledLayoutChilren(QLayout* layout,bool state);

    cv::Mat_<float> _mat;
    Ui::Dialog *ui;
    bool _isUsingPreset;

    FILTER_PRESET _use_filter;
};

#endif // DIALOG_H
