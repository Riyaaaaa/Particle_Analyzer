#include "dialog.h"
#include "ui_dialog.h"
#include <QPushButton>

#define matrixConnector(_rows_,_cols_) connect(ui->m##_rows_##_cols_,SIGNAL(textChanged(QString)),this,SLOT(setElementM##_rows_##_cols_(QString)))
#define matrixInitialize(_rows_,_cols_) ui->m##_rows_##_cols_->setText("1");// _mat.at<float>(_rows_-1,_cols_-1)=1.0f;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    _mat = cv::Mat_<float>(3,3);

    connect(ui->checkPreset,SIGNAL(stateChanged(int)),this,SLOT(stateChangedPreset(int)));
    connect(ui->checkFilter,SIGNAL(stateChanged(int)),this,SLOT(stateChangedUser(int)));

    connect(ui->sobel,SIGNAL(clicked(bool)),this,SLOT(selectedPreset(bool)));
    connect(ui->bilateral,SIGNAL(clicked(bool)),this,SLOT(selectedPreset(bool)));
    connect(ui->gaussian,SIGNAL(clicked(bool)),this,SLOT(selectedPreset(bool)));
    connect(ui->scharr,SIGNAL(clicked(bool)),this,SLOT(selectedPreset(bool)));
    connect(ui->laplacian,SIGNAL(clicked(bool)),this,SLOT(selectedPreset(bool)));
    connect(ui->median,SIGNAL(clicked(bool)),this,SLOT(selectedPreset(bool)));

    matrixConnector(1,1);
    matrixConnector(1,2);
    matrixConnector(1,3);
    matrixConnector(2,1);
    matrixConnector(2,2);
    matrixConnector(2,3);
    matrixConnector(3,1);
    matrixConnector(3,2);
    matrixConnector(3,3);

    matrixInitialize(1,1);
    matrixInitialize(1,2);
    matrixInitialize(1,3);
    matrixInitialize(2,1);
    matrixInitialize(2,2);
    matrixInitialize(2,3);
    matrixInitialize(3,1);
    matrixInitialize(3,2);
    matrixInitialize(3,3);

    //connect();

    ui->checkPreset->setCheckState(Qt::CheckState::Checked);
    ui->sobel->setChecked(true);
    _use_filter = SOBEL;

    setEnabledLayoutChilren(ui->userfilter,false);

}

void Dialog::stateChangedPreset(int state){
    switch (state) {
    case Qt::Unchecked:
        setUsingPreset(false);
        ui->checkFilter->setChecked(true);
        break;
    case Qt::PartiallyChecked:
        break;
    case Qt::Checked:
        setUsingPreset(true);
        ui->checkFilter->setChecked(false);
        break;
    default:
        break;
    }
}

void Dialog::stateChangedUser(int state){
    switch (state) {
    case Qt::Unchecked:
        setUsingPreset(true);
        ui->checkPreset->setCheckState(Qt::CheckState::Checked);
        setEnabledLayoutChilren(ui->userfilter,false);
        break;
    case Qt::PartiallyChecked:
        break;
    case Qt::Checked:
        setUsingPreset(false);
        ui->checkPreset->setCheckState(Qt::CheckState::Unchecked);
        setEnabledLayoutChilren(ui->userfilter,true);
        break;
    default:
        break;
    }
}

void Dialog::checkValue(QString str){
    QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    int unti=0;
    QValidator::State state = doubleValidator->validate(str,unti);

    if(state == QValidator::State::Acceptable){
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        this->setStyleSheet("background: white");
    }
    else{
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        this->setStyleSheet("background: red");
    }
}

void Dialog::setEnabledLayoutChilren(QLayout* layout,bool state){
    for (int i = 0; i < layout->count(); ++i) {
        QWidget *w = layout->itemAt(i)->widget();
        if(w != NULL)
            w->setEnabled(state);
    }
}

void Dialog::selectedPreset(bool){
    if(ui->sobel->isChecked()){
        _use_filter = SOBEL;
    }
    else if(ui->gaussian->isChecked()){
        _use_filter = GAUSSIAN;
    }
    else if(ui->median->isChecked()){
        _use_filter = MEDIAN;
    }
    else if(ui->laplacian->isChecked()){
        _use_filter = LAPLACIAN;
    }
    else if(ui->scharr->isChecked()){
        _use_filter = SCHARR;
    }
    else if(ui->bilateral->isChecked()){
        _use_filter = BILATERAL;
    }
}

Dialog::~Dialog()
{
    delete ui;
}
