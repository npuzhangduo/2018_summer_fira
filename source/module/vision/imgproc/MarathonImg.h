#ifndef MARATHONIMG_H
#define MARATHONIMG_H

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

class MarathonResult : public ImgProcResult
{
public:
    cv::Point2f line_center;
    cv::Point2f part_line_center[4];

    bool line_valid;
	double slope;

public:
    MarathonResult() :
        line_center(),
        part_line_center(),

        line_valid(false)
    {}

    virtual void operator=(ImgProcResult &res)
    {
        MarathonResult *tmp = dynamic_cast<MarathonResult *>(&res);
        line_center = tmp->line_center;
	for(int i = 0; i < 4; i++)
	{
	    part_line_center[i] = tmp->part_line_center[i];
	} 
        line_valid = tmp->line_valid;
    }
    void operator=(MarathonResult &res)
    {
        line_center = res.line_center;
	for(int i = 0; i < 4; i++)
	{
	    part_line_center[i] = res.part_line_center[i];
	} 

        line_valid = res.line_valid;
    }
};

class Marathon : public ImgProc
// class Marathon 
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
//     void imageProcess(cv::Mat image);
       void imageProcess(cv::Mat img, ImgProcResult *Result);

    Marathon()//默认构造函数
    {
        histSize[0] = 256;//图像像素一共有256个值，所以直方图一共就有256项，每项对应的是像素值为该值的像素个数，或者是占总像素的比例大小
        hranges[0] = 0.0;//像素取值的最小值
        hranges[1] = 255.0;//像素取值的最大值
        ranges[0] = hranges;//浮点指针指向hranges数组 什么意思???
        channels[0] = 0;//默认情况，我们考察0号通道

        hole_center_rows   = -1;//y
        hole_center_cols   = -1;//basket重心x
        hole_valid         = false;

        ball_center_rows   = -1;
        ball_center_cols   = -1;
        ball_valid         = false;

        line_center_rows   = -1;
        line_center_cols   = -1;
        line_valid         = false;
        block_num          = 4;

        start_file_num     = 1;
        max_file_num       = 500;

        canny_thre         = 103;
        canny_thre_max     = 500;

        debug_print        = 0;

        min_hole            = 87;
        max_hole            = 97;
        Load_parameter();
    }


    void Load_std_img();
    void Pretreat(cv::Mat image);
    cv::MatND getHistogram(const cv::Mat& image);//计算1D直方图
    cv::Mat getHistogram_Image(const cv::Mat& image);//计算1D直方图并返回其分布图像，将直方图可视化，变为柱状图
    double Count_possible(cv::Mat bianry_img, cv::Rect roi_rect);
    void Erode_img(cv::Mat src, cv::Mat& dst, int size);//膨胀
    void Dilate_img(Mat src, Mat &dst, int size);
    void Write_img(cv::Mat& src,  const char floder_name[30], int num);

/*********Marathon Hole*******/
//hole Circle member

    cv::Mat std_hole;
    cv::Mat std_hole_hsv_channel[3];
    cv::MatND std_hole_hist;

    cv::Mat hole_project_result;

    double hole_center_rows;//y
    double hole_center_cols;//色标重心x
    bool hole_valid;
    cv::Rect hole_rect;


    int min_hole;
    int max_hole;

//hole Circle function
public:

    void Process_hole(cv::Mat image);
    void Find_hole_center(cv::Mat src);
    int Judge_hole_legal(cv::Mat& src);
    cv::Mat hole_BackProject(const cv::MatND& hist, const cv::Mat& findimage);//反向投影
    void hole_setNotFound();

/*********Marathon Ball*******/
//ball Circle member

    cv::Mat std_ball;
    cv::Mat std_ball_hsv_channel[3];
    cv::MatND std_ball_hist;

    cv::Mat ball_project_result;

    double ball_center_rows;//y
    double ball_center_cols;//色标重心x
    bool ball_valid;
    cv::Rect ball_rect;

//ball Circle function
public:

    void Process_ball(cv::Mat image);
    void Find_ball_center(cv::Mat src);
    int Judge_ball_legal(cv::Mat& src);
    cv::Mat ball_BackProject(const cv::MatND& hist, const cv::Mat& findimage);//反向投影
    void ball_setNotFound();
    int Judge_centerInRect(cv::Point2f center, cv::Rect rect);



/*********Marathon Line*******/
//Line member
    cv::Mat white_thre_result;

    double line_center_rows;//y
    double line_center_cols;//色标重心x
    bool line_valid;
    cv::Rect line_rect;
    cv::Point2f part_masscenter[4]; //每一段的重心
    cv::Point2f mass_center;        //整体的重心
    int block_num;                  //把线分成几段

//Line function
public:
    void Process_line(cv::Mat image);
    void Find_line_center(cv::Mat src);
    int  Judge_line_legal(cv::Mat& src);
    void line_setNotFound();
    cv::Point2f Find_mass_center(Mat& bianry_img, Rect roi_rect);

/*********Parameter*********/
//Parameter member
    int white_gray_thre;
    int white_erode_size;
    int white_dilate_size;

    void Load_parameter();
    void Store_parameter();
    void Set_parameter();

    MarathonResult MyResult;
};

#endif // MARATHON_H
