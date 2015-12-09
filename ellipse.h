#ifndef ELLIPSE_H
#define ELLIPSE_H

#include<opencv2/opencv.hpp>

struct Ellipse
{
public:
    Ellipse()=default;
    Ellipse(cv::RotatedRect,double,cv::Mat);
    //Ellipse(Ellipse&&) noexcept{};

    double area;
    cv::RotatedRect rect;
    cv::Mat image;
};

#endif // ELLIPSE_H
