#include "imageview.h"
#include<string>


ImageView::ImageView()
{
    QGraphicsView::QGraphicsView();
}

ImageView::ImageView(QWidget *pWnd):QGraphicsView(pWnd){
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
    input = img.clone();
    m_img = QImage(input.data, input.cols, input.rows, QImage::Format_Indexed8);
    m_img = m_img.convertToFormat(QImage::Format_RGB32);
}

void ImageView::scaleView( qreal factor )
{
    factor /= 10.;//-0.1 <-> 0.1
    factor += 1;

    std::cout << factor << std::endl;

	//limit zoom out ---
	if (m_matrix.m11()==1 && factor < 1)
		return;

	if (m_matrix.m11()*factor<1)
		factor = 1/m_matrix.m11();


	//limit zoom in ---
	if (m_matrix.m11()>100 && factor > 1)
		return;
/*
	//inverse the transform
	int a, b;
	m_matrix_inv.map(center.x(),center.y(),&a,&b);
*/

    //m_matrix.translate(a-factor*a,b-factor*b);
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

void ImageView::particleAnalysis(int threshold,double ellipse_min,double ellipse_max){
    cv::Mat result(input.rows, input.cols, input.type());
        cv::threshold(input, result, threshold, 255, CV_THRESH_BINARY);

        typedef std::vector<cv::Point> outline;
        std::vector<outline> contours;
        std::vector< std::pair< double , outline > > ellipse_vectors;

        emit log("start analysis");

        cv::findContours(result, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        ellipses.clear();

        for(auto& contour: contours){
            auto area = cv::contourArea(contour);
            ellipse_vectors.push_back( std::make_pair(area, std::move(contour)) );
        }
        ellipse_vectors.erase(std::remove_if(ellipse_vectors.begin(),ellipse_vectors.end(),[=](std::pair<double,outline>& ellipse){
                                             return ellipse.second.size() < 5 || ellipse.first < ellipse_min || ellipse.first > ellipse_max;
                                           }
                                        ),
                ellipse_vectors.end());
        //std::sort(ellipses.begin(),ellipses.end());
        for(auto& vector: ellipse_vectors){
            cv::Mat ellipse(input.rows,input.cols,input.type());
            auto rect = [vector](){
                cv::Mat pointf;
                cv::Mat(vector.second).convertTo(pointf, CV_32F);
                return cv::fitEllipse(pointf);
            }();
            cv::ellipse(ellipse, rect, cv::Scalar(255, 0, 0), -1);
            ellipses.push_back(Ellipse(rect,vector.first,ellipse));
        }
        emit log("finished analysis");
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

void ImageView::startAnalysis(int threshold,double ellipse_min,double ellipse_max){
    ellipses.clear();

    particleAnalysis(threshold,ellipse_min,ellipse_max);
    renderAnalyzedImage();
}

 void ImageView::renderAnalyzedImage(){
     emit log("rendering ellipses...");

     std::vector<cv::Mat> mv;
     cv::Mat merged(input.rows,input.cols,input.type());
     cv::cvtColor(input,merged,CV_GRAY2BGR);

     cv::split(merged, mv);
     mv.push_back(cv::Mat::zeros(input.rows,input.cols,CV_8UC1));

     for(auto& ellipse: ellipses){
         /*
         for(int i=0; i<mv[2].size(); i++){
             mv[2].data[i] = mv[i] + ellipse.data[i] < 255 ? mv[i] + ellipse.data[i] : 255;
         }
         */
         mv[3] += ellipse.image;
     }

     cv::merge(mv,merged);
     cv::imwrite("result.png",merged);

     cv::imshow("source image",input);
     cv::waitKey(-1);
     cv::destroyWindow("source image");

     QImage img = QImage(mv[3].data, mv[3].cols, mv[3].rows, QImage::Format_Indexed8);
     img = img.convertToFormat(QImage::Format_RGB32);

     m_img = img;
     analyzed_image = mv[3].clone();
     viewport()->update();

     emit log("finished redering");
 }

 void ImageView::emphasisEllipse(std::size_t id){
     cv::Mat emphasised;
     cvtColor(analyzed_image.clone(),emphasised,CV_8UC3);

     cv::ellipse(emphasised,ellipses[id].rect,cv::Scalar(255,0,0),-1);

     m_img = QImage(emphasised.data, emphasised.cols, emphasised.rows, QImage::Format_RGB888);
 }
