#ifndef COLORFILTER_H
#define COLORFILTER_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "imgproc.h"

using namespace std;
using namespace cv;

class ColorFilterResult: public ImgProcResult
{
public:
    //ColorFilter();
    Point center;
    bool valid;    
    ColorFilterResult() :
        center(-1,-1),
        valid(false)
    {}
public:

    virtual void operator=(ImgProcResult &res)
    {
	ColorFilterResult *tmp = dynamic_cast<ColorFilterResult*>(&res);
        center = tmp->center;
	valid = tmp->valid;
    }
    void operator=(ColorFilterResult &res)
    {
        center = res.center;
    valid = res.valid;
    }
};


class ColorFilter: public ImgProc
{
public:
    ColorFilter();
    void getThresValue(Mat& src);
    //Point filter(Mat& src);
    void loadSvm();
    virtual void imageProcess(cv::Mat img, ImgProcResult *Result);

private:
    vector<Mat>bgr;
    vector<Mat>images;
    vector<vector<Point> > contours;
    vector<Rect> boundRects;
    vector<vector<Point> > contours_poly;
    Mat labImg;
    Mat hsvImg;
    vector<Mat>lab;
    vector<Mat>hsv;
    vector<int>maxValue;
    vector<int>minValue;
    Mat hImg,sImg,lImg;
    int maxH,minH,maxL,minL,maxS,minS;
    int sumMaxH,sumMinH,sumMaxL,sumMinL,sumMaxS,sumMinS;
    int xsum,ysum;
    int pixelsum;
    ColorFilterResult MyResult;
};

#endif // COLORFILTER_H
