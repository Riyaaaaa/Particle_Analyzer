#include "imageview.h"
#include<string>

ImageView::ImageView()
{

}

void ImageView::paintEvent( QPaintEvent *event ){
    QPainter widgetpainter( viewport() );
    widgetpainter.setWorldTransform( m_matrix );
    QImage qimg = m_img.scaled(
        viewport()->width(),
    	viewport()->height(),
     	Qt::KeepAspectRatio,Qt::FastTransformation);
    widgetpainter.drawImage( 0, 0, qimg );
}

void ImageView::setImg( QImage &img ){
    m_img = QImage( img );
    viewport()->update();
}

void ImageView::setImg( cv::Mat& img ){
    cv::Mat dst;
    cv::cvtColor(img, dst, CV_RGB2BGR);
    m_img = QImage(dst.data, dst.cols, dst.rows, QImage::Format_RGB888);
}

void ImageView::scaleView( qreal factor, QPointF center )
{
	factor/=5;//-0.1 <-> 0.1
	factor+=1;//0.9 <-> 1.1

	//limit zoom out ---
	if (m_matrix.m11()==1 && factor < 1)
		return;

	if (m_matrix.m11()*factor<1)
		factor = 1/m_matrix.m11();


	//limit zoom in ---
	if (m_matrix.m11()>100 && factor > 1)
		return;

	//inverse the transform
	int a, b;
	m_matrix_inv.map(center.x(),center.y(),&a,&b);

	m_matrix.translate(a-factor*a,b-factor*b);
	m_matrix.scale(factor,factor);

	controlImagePosition();
}

void ImageView::controlImagePosition()
{
	qreal left, top, right, bottom;

	//after check top-left, bottom right corner to avoid getting "out" during zoom/panning
	m_matrix.map(0,0,&left,&top);

	if (left > 0)
	{
		m_matrix.translate(-left,0);
		left = 0;
	}
	if (top > 0)
	{
		m_matrix.translate(0,-top);
		top = 0;
	}

	QSize sizeImage = size();
	m_matrix.map(sizeImage.width(),sizeImage.height(),&right,&bottom);
	if (right < sizeImage.width())
	{
		m_matrix.translate(sizeImage.width()-right,0);
		right = sizeImage.width();
	}
	if (bottom < sizeImage.height())
	{
		m_matrix.translate(0,sizeImage.height()-bottom);
		bottom = sizeImage.height();
	}

	m_matrix_inv = m_matrix.inverted();

	viewport()->update();
}

void ImageView::mouseMoveEvent( QMouseEvent *event )
{
	QPoint pnt = event->pos();

	if ( m_matrix.m11() > 1 && event->buttons() == Qt::LeftButton )
	{
		QPointF pntf = ( pnt - m_pntDownPos ) / m_matrix.m11();
		m_pntDownPos = event->pos();
		m_matrix.translate( pntf.x(), pntf.y() );
		controlImagePosition();
		viewport()->update();
	}

	viewport()->update();

	QWidget::mouseMoveEvent( event );
}


void ImageView::mousePressEvent( QMouseEvent *event )
{
	m_pntDownPos = event->pos();

	QWidget::mousePressEvent( event );
}

