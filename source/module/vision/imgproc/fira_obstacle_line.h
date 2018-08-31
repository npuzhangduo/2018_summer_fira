#ifndef FiraObstacleLine_H
#define FiraObstacleLine_H

#include "iostream"
#include "algorithm"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "fstream"
#include "sstream"
#include "imgproc.h"

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class FiraObstacleLineResult : public ImgProcResult
{
public:
    cv::Point2f line_center;
    double line_angle;
    bool line_valid;

public:
    FiraObstacleLineResult() :
        line_center(),
        line_angle(0),
        line_valid(0)
    {}

    virtual void operator=(ImgProcResult &res)
    {
        FiraObstacleLineResult *tmp = dynamic_cast<FiraObstacleLineResult *>(&res);
        line_center = tmp -> line_center;
        line_angle = tmp -> line_angle;
        line_valid = tmp -> line_valid;
    }
    void operator=(FiraObstacleLineResult &res)
    {
        line_center = res.line_center;
        line_angle = res.line_angle;
        line_valid = res.line_valid;
    }
};

class FiraObstacleLine : public ImgProc
//class FiraObstacleLine
{
/******Public******/
//public member
private:
    int histSize[1];//项的数量 即直方图的条数
    float hranges[2];//像素取值的最大值和最小值
    const float* ranges[1];//一个保存浮点型指针的数组，数组大小为1，指向每一项包含两个元素的数组
    int channels[1];//仅用到一个通道

public:
    cv::Mat src_img;
    cv::Mat hsv_img;
    cv::Mat gray_img;
    cv::Mat blur_img;
    cv::Mat center_img;
    cv::Mat circles_img;
    cv::Mat canny_img;//new

    cv::Mat channel[3];
    cv::Mat img_h_channel;

    int start_file_num;
    int max_file_num;

    int canny_thre;
    int canny_thre_max;

    int debug_print;

//public function
public:
     //void imageProcess(cv::Mat image);
       void imageProcess(cv::Mat img, ImgProcResult *Result);

    FiraObstacleLine()//默认构造函数
    {
        histSize[0] = 256;//图像像素一共有256个值，所以直方图一共就有256项，每项对应的是像素值为该值的像素个数，或者是占总像素的比例大小
        hranges[0] = 0.0;//像素取值的最小值
        hranges[1] = 255.0;//像素取值的最大值
        ranges[0] = hranges;//浮点指针指向hranges数组 什么意思???
        channels[0] = 0;//默认情况，我们考察0号通道

        line_center_rows   = -1;
        line_center_cols   = -1;
        line_angle = 0;
        line_valid         = false;

        start_file_num     = 1;
        max_file_num       = 500;

        canny_thre         = 103;
        canny_thre_max     = 500;

        debug_print        = 0;

        Load_parameter();
    }


    void Load_std_img();
    void Pretreat(cv::Mat image);
    void Erode_img(cv::Mat src, cv::Mat& dst, int size);//膨胀
    void Dilate_img(Mat src, Mat &dst, int size);
    void Write_img(cv::Mat& src,  const char floder_name[30], int num);

/*********FiraObstacleLine Line*******/
//Line member
    cv::Mat white_thre_result;

    double line_center_rows;//y
    double line_center_cols;//色标重心x
    double line_angle;
    bool line_valid;
    cv::Rect line_rect;
	cv::RotatedRect line_minRect;

//Line function
public:
    void Process_line(cv::Mat image);
    void Find_line_center(cv::Mat src);
    int  Judge_line_legal(cv::Mat& src);
    void line_setNotFound();

/*********Parameter*********/
//Parameter member
    int white_gray_thre;
    int white_erode_size;
    int white_dilate_size;

    void Load_parameter();
    void Store_parameter();
    void Set_parameter();

   FiraObstacleLineResult MyResult;
};

#endif // FiraObstacleLine_H
