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
    
    void setImg( const QImage &img );
    void setImg( const cv::Mat &img );
    const cv::Mat& getImg(){ return input; }

    void setResource(const cv::Mat& img){ input = img.clone(); }

    bool init();
    
    void controlImagePosition();
    void scaleView( qreal factor );
    void mouseMoveEvent( QMouseEvent *event );
    void mousePressEvent( QMouseEvent *event );

    void startAnalysis(int threshold,double ellipse_min,double ellipse_max);
    void particleAnalysis(int threshold,double ellipse_min,double ellipse_max);
    void particleAnalysis(int threshold,double ellipse_min,double ellipse_max,cv::Mat filter);
    void renderAnalyzedImage(const std::vector<Ellipse>& ellipses);
    void deleteEllipse( int id );
    void setStandard( QPoint,double);
    double getScale() const{ return meter_per_pixel; };

    const std::vector< Ellipse >& getEllipses() const{ return _ellipses; }
    void setEllipses(const std::vector< Ellipse >& ellipses){ _ellipses = ellipses;}

public slots:
    void emphasisEllipse(int id);
    void switchImage();
    
signals:
    void log(QString);
    void mousePressed( QPoint );

private:
  	void paintEvent( QPaintEvent *event );

    cv::Mat input;
    cv::Mat analyzed_image;
    cv::Mat emphasised;

    double meter_per_pixel=1;

    QImage m_img;
    QPoint m_pntDownPos;
    QTransform m_matrix;
    QTransform m_matrix_inv;
    QLabel myLabel;
    std::vector< Ellipse > _ellipses;
};

#endif // IMAGEVIEW_H
