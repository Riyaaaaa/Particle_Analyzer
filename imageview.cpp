#include "imageview.h"
#include<string>


ImageView::ImageView()
{
    QGraphicsView::QGraphicsView();
}

ImageView::ImageView(QWidget *pWnd):QGraphicsView(pWnd){
}

void ImageView::paintEvent( QPaintEvent *event ){
    if(m_img.isNull())return;
    QPainter widgetpainter( viewport() );
    widgetpainter.setWorldTransform( m_matrix );
    QImage qimg = m_img.scaled(
        viewport()->width(),
        viewport()->height(),
        Qt::KeepAspectRatio,Qt::FastTransformation);
    widgetpainter.drawImage( 0, 0, qimg );
}

void ImageView::setImg( const QImage &img ){
    m_img = QImage( img );
    viewport()->update();
}

void ImageView::setImg(const cv::Mat& img ){
    setResource(img);
    emphasised = img.clone();
    m_img = QImage(input.data, input.cols, input.rows, QImage::Format_Indexed8);
    m_img = m_img.convertToFormat(QImage::Format_RGB32);
}

bool ImageView::init(){
    m_img.fill(QColor(255,255,255));
    _ellipses.clear();
    analyzed_image = cv::Mat::zeros(analyzed_image.rows,analyzed_image.cols,analyzed_image.type());
    viewport()->update();
    return true;
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
/*
    m_qimg = QImage( m_img.ptr(), m_img.cols, m_img.rows, m_img.step, QImage::Format_RGB888 );
    if ( viewport()->width() < m_img.width() )
        m_img = m_img.scaled(
            viewport()->width()*m_matrix.m11(),
            viewport()->height()*m_matrix.m11(),
            Qt::KeepAspectRatio,Qt::FastTransformation);
*/
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
    float k=2.0f;
    /*
    cv::Mat KernelData = (cv::Mat_<float>(3,3) <<
            -k/9.0f, -k/9.0f,           -k/9.0f,
            -k/9.0f, 1 + (8 * k)/9.0f,  -k/9.0f,
            -k/9.0f, -k/9.0f,           -k/9.0f
        );
        */

        cv::threshold(input, result, threshold, 255, CV_THRESH_BINARY);

        typedef std::vector<cv::Point> outline;
        std::vector<outline> contours;
        std::vector< std::pair< double , outline > > ellipse_vectors;


        emit log("start analysis");

        cv::findContours(result, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        _ellipses.clear();

        for(auto& contour: contours){
            auto area = cv::contourArea(contour) * meter_per_pixel;
            ellipse_vectors.push_back( std::make_pair(area, std::move(contour)) );
        }
        ellipse_vectors.erase(std::remove_if(ellipse_vectors.begin(),ellipse_vectors.end(),[=](std::pair<double,outline>& ellipse){
                                             return ellipse.second.size() < 5 || ellipse.first < ellipse_min || ellipse.first > ellipse_max;
                                           }
                                        ),
                ellipse_vectors.end());
        //std::sort(ellipses.begin(),ellipses.end());
        for(auto& vector: ellipse_vectors){
            cv::Mat ellipse = cv::Mat::zeros(input.rows,input.cols,input.type());
            auto rect = [vector](){
                cv::Mat pointf;
                cv::Mat(vector.second).convertTo(pointf, CV_32F);
                return cv::fitEllipse(pointf);
            }();
            cv::ellipse(ellipse, rect, cv::Scalar(255, 0, 0), -1);
            _ellipses.push_back(Ellipse(rect,vector.first,ellipse));
        }
        emit log("finished analysis");
}

void ImageView::mouseMoveEvent( QMouseEvent *event )
{
    if(m_img.isNull())return;
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
    if(m_img.isNull())return;
    m_pntDownPos = event->pos();

   double dScale = (double)viewport()->width() / (double)m_img.width();
   if ( dScale > ( (double)viewport()->height() / (double)m_img.height() ) )
    dScale = (double)viewport()->height() / (double)m_img.height();

   QPointF p = m_matrix_inv.map( event->pos() );
   emit mousePressed( QPoint( p.x() / dScale, p.y() / dScale ) );

    QWidget::mousePressEvent( event );
}

void ImageView::startAnalysis(int threshold,double ellipse_min,double ellipse_max){
    _ellipses.clear();
    analyzed_image = cv::Mat::zeros(input.rows,input.cols,CV_8UC1);
    emphasised = cv::Mat::zeros(input.rows,input.cols,CV_8UC3);

    particleAnalysis(threshold,ellipse_min,ellipse_max);
    renderAnalyzedImage(_ellipses);
}

void ImageView::setStandard(QPoint pos,double length){
    int pos_x[2];
    cv::Mat lined = input.clone();

    int oldValue = input.at<unsigned char>(pos.y() , pos.x());
    for(int x = pos.x()-1; x >= 0; x--){
        int value = input.at<unsigned char>( pos.y() , x );
        if( fabs(oldValue - value) < 5 )pos_x[0] = x;
        else break;
    }

    oldValue = input.at<unsigned char>(pos.y() , pos.x());
        for(int x = pos.x()+1; x < input.cols; x++){
            int value = input.at<unsigned char>( pos.y() , x );
            if( fabs(oldValue - value) < 5 )pos_x[1] = x;
            else break;
        }

    if( pos_x[1] - pos_x[0] == 0 ){
        emit("Can't establish a standard.\nThe area as a standard could not be found .");
        return;
    }

    double pixels = pos_x[1] - pos_x[0] + 1;
    meter_per_pixel = length / pixels;

    cv::merge({input,input,input},emphasised);
    cv::line(emphasised, cv::Point(pos_x[0], pos.y()), cv::Point(pos_x[1], pos.y()), cv::Scalar(255,0,0), 1, 8);
    emphasised.at<cv::Vec3b>(pos.y(),pos.x()) = cv::Vec3b(255,255,0);
    //cv::circle(emphasised, cv::Point(pos.x(),pos.y()), 2, cv::Scalar(0,0,200), 1, 4);

    cv::Rect region = cv::Rect( pos.x() - pixels, pos.y() - pixels, pixels*2, pixels*2 );
    if(region.x < 0)region.x = 0;
    if(region.y < 0)region.y = 0;
    if(region.x + pixels*2 >= emphasised.cols)region.width = emphasised.cols - region.x - 1;
    if(region.y + pixels*2 >= emphasised.rows)region.height = emphasised.rows - region.y - 1 ;

    cv::Mat show = emphasised(region);
    cv::resize(show,show,cv::Size(input.cols,input.rows));
    cv::imshow("scale standard",show);
    cv::waitKey(-1);
    cv::destroyWindow("scale standard");

    m_img = QImage(emphasised.data, emphasised.cols, emphasised.rows, QImage::Format_RGB888);
    viewport()->update();

    emit log("set standard " + QString::number(meter_per_pixel) + "[μm/pixel]");
}

 void ImageView::renderAnalyzedImage(const std::vector<Ellipse>& ellipses){

     emit log("rendering ellipses...");
     cv::Mat mv(cv::Mat::zeros(input.rows,input.cols,CV_8UC1));

     for(int i=0; i<ellipses.size(); i++){
         /*
         for(int i=0; i<mv[2].size(); i++){
             mv[2].data[i] = mv[i] + ellipse.data[i] < 255 ? mv[i] + ellipse.data[i] : 255;
         }
         */
         mv += ellipses[i].image;
         cv::putText(mv, std::to_string(i), ellipses[i].rect.center, cv::FONT_HERSHEY_SIMPLEX , 0.5, cv::Scalar(128,128,128), 1, CV_AA);
     }

     QImage img = QImage(mv.data, mv.cols, mv.rows, QImage::Format_Indexed8);
     img = img.convertToFormat(QImage::Format_RGB32);

     m_img = img;
     analyzed_image = mv.clone();
     if(ellipses.size() != 0)emphasisEllipse(0);
     viewport()->update();

     emit log("finished redering");
 }

 void ImageView::emphasisEllipse(int id){
    cv::merge({analyzed_image,analyzed_image,analyzed_image},emphasised);
    cv::ellipse(emphasised,_ellipses[id].rect,cv::Scalar(125,0,0),-1);

     m_img = QImage(emphasised.data, emphasised.cols, emphasised.rows, QImage::Format_RGB888);
     viewport()->update();
     //emit log("selected " + QString::number(id));
 }

 void ImageView::deleteEllipse(int id){
     _ellipses.erase( _ellipses.begin() + id );

     renderAnalyzedImage(_ellipses);
     emphasisEllipse(id);
 }

 void ImageView::switchImage(){
     static bool flag = true;
     if(flag){
         m_img = QImage(input.data, input.cols, input.rows, QImage::Format_Indexed8);
         m_img = m_img.convertToFormat(QImage::Format_RGB32);
     }
     else{
        m_img = QImage(emphasised.data, emphasised.cols, emphasised.rows, QImage::Format_RGB888);
     }
     flag = !flag;
     viewport()->update();
 }
