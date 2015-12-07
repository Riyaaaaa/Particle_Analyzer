#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QObject>
#include <QWidget>
#include <QtGui>
#include <QGraphicsView>
#include <Qimage>
#include<QLabel>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include "ellipse.h"

class ImageView : public QGraphicsView
{
    Q_OBJECT

public:
    ImageView();
    ImageView(QWidget *pWnd);
    
    void setImg( QImage &img );
    void setImg( cv::Mat &img );
    
    void controlImagePosition();
    void scaleView( qreal factor );
    void mouseMoveEvent( QMouseEvent *event );
    void mousePressEvent( QMouseEvent *event );

    void startAnalysis(int threshold,double ellipse_min,double ellipse_max);
    void particleAnalysis(int threshold,double ellipse_min,double ellipse_max);
    void renderAnalyzedImage();

    void emphasisEllipse(std::size_t id);

signals:
    void log(QString);//{return str;}
    
private:
  	void paintEvent( QPaintEvent *event );
    cv::Mat input;
    cv::Mat analyzed_image;
    QImage m_img;
    QPoint m_pntDownPos;
    QTransform m_matrix;
    QTransform m_matrix_inv;
    QLabel myLabel;
    std::vector< Ellipse > ellipses;
};

#endif // IMAGEVIEW_H
