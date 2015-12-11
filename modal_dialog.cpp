#include "modal_dialog.h"

ModalDialog::ModalDialog(QWidget* parent)
    : QDialog(parent)
{
    label = new QLabel("[μm]");
    button = new QPushButton("set");
    lineEdit = new QLineEdit;

    connect(button,SIGNAL( clicked() ),this,SLOT( sendValue() ) );
    connect(lineEdit, SIGNAL( textChanged(QString) ), this, SLOT(checkValue(QString)));

    button->setEnabled(false);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(lineEdit);
    layout->addWidget(label);
    layout->addWidget(button);
    setLayout(layout);
}

void ModalDialog::sendValue(){
    //_owner->setStandard(lineEdit->Value());
    this->accept();
}

double ModalDialog::getValue(){
    return lineEdit->text().toDouble();
}

void ModalDialog::checkValue(QString str){
    QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    int unti=0;
    QValidator::State state = doubleValidator->validate(str,unti);

    if(state == QValidator::State::Acceptable){
        button->setEnabled(true);
        this->setStyleSheet("background: white");
    }
    else{
        button->setEnabled(false);
        this->setStyleSheet("background: red");
    }
}

ModalDialog::~ModalDialog()
{
    //delete ui;
}

