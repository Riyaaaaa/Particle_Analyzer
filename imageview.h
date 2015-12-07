#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QObject>
#include <QWidget>
#include <QtGui>
#include <QGraphicsView>
#include <Qimage>
#include <opencv2/opencv.hpp>

class ImageView : public QGraphicsView
{
public:
    ImageView();
    //ImageView(QWidget *pWnd);
    
    void setImg( QImage &img );
    void setImg( cv::Mat &img );
    
protected:
    void controlImagePosition();
    void scaleView( qreal factor, QPointF center );
    void mouseMoveEvent( QMouseEvent *event );
    void mousePressEvent( QMouseEvent *event );
    
private:
  	void paintEvent( QPaintEvent *event );
    QImage m_img;
    QPoint m_pntDownPos;
    QTransform m_matrix;
    QTransform m_matrix_inv;
};

#endif // IMAGEVIEW_H
