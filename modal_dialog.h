#ifndef MODALDIALOG_H
#define MODALDIALOG_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDoubleValidator>

class ModalDialog : public QDialog
{
    Q_OBJECT
    public:
        explicit ModalDialog(QWidget* parent = 0);
        ~ModalDialog();
        double getValue();

public slots:
        void sendValue();
        void checkValue(QString);

    private:
        QLabel* label;
        QPushButton* button;
        QLineEdit* lineEdit;
};

#endif // MODALDIALOG_H
