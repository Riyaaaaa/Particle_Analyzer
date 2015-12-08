#-------------------------------------------------
#
# Project created by QtCreator 2015-12-04T11:56:38
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9

CONFIG += c++11
#CONFIG += x_86

TARGET = ParticleAnalysis
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    imageview.cpp \
    ellipse.cpp \
    modal_dialog.cpp

HEADERS  += mainwindow.h \
    imageview.h \
    ellipse.h \
    modal_dialog.h

DEPENDPATH += /usr/local/include
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/Cellar/opencv/2.4.11_1/lib \
-lopencv_calib3d \
-lopencv_contrib \
-lopencv_core \
-lopencv_features2d \
-lopencv_flann \
-lopencv_gpu \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_legacy \
-lopencv_ml \
-lopencv_objdetect \
-lopencv_video

DISTFILES += \
    Resource/image.png

FORMS += \
    Resource/mainwindow.ui
