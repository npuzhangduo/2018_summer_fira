#ifndef FIRA_OBSTACLE_VISION_H
#define FIRA_OBSTACLE_VISION_H

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


class FiraObstacleVisionResult : public ImgProcResult
{
public:
   cv::Point2f color1_center;
   cv::Point2f color2_center;
   cv::Point2f color3_center;

   bool color1_valid;
   bool color2_valid;
   bool color3_valid;

   double color1_area;
   double color2_area;
   double color3_area;

   double color1_height;
   double color2_height;
   double color3_height;

   cv::Point2f line_center;
   double line_angle;

public:
   FiraObstacleVisionResult() :
       color1_center(),
       color2_center(),
       color3_center(),

       color1_area(0),
       color2_area(0),
       color3_area(0),

       color1_valid(false),
       color2_valid(false),
        color3_valid(false),

      color1_height(0),
      color2_height(0),
      color3_height(0),

      line_center(),
      line_angle(0)
   {}

   virtual void operator=(ImgProcResult &res)
   {
   FiraObstacleVisionResult *tmp = dynamic_cast<FiraObstacleVisionResult*>(&res);
       color1_center = tmp->color1_center;
       color2_center = tmp->color2_center;
       color3_center = tmp->color3_center;

       color1_area = tmp -> color1_area;
       color2_area = tmp -> color2_area;
       color3_area = tmp -> color3_area;

   color1_valid = tmp->color1_valid;
   color2_valid = tmp->color2_valid;
    color3_valid = tmp->color2_valid;

      color1_height = tmp -> color1_height;
      color2_height = tmp -> color2_height;
      color3_height = tmp -> color3_height;

      line_center = tmp -> line_center;
      line_angle = tmp -> line_angle;
   }
   void operator=(FiraObstacleVisionResult &res)
   {
       color1_center = res.color1_center;
       color2_center = res.color2_center;
       color3_center = res.color3_center;

       color1_area = res.color1_area;
       color2_area = res.color2_area;
       color3_area = res.color3_area;

   color1_valid = res.color1_valid;
   color2_valid = res.color2_valid;
    color3_valid = res.color3_valid;

      color1_height = res.color1_height;
      color2_height = res.color2_height;
      color3_height = res.color3_height;

      line_center = res.line_center;
      line_angle = res.line_angle;
   }
};


class FiraObstacleVision : public ImgProc
// class FiraObstacleVision
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

    double line_center_row;
    double line_center_col;
    double line_angle;

//public function
public:
//     void imageProcess(cv::Mat image);
       void imageProcess(cv::Mat img, ImgProcResult *Result);

    FiraObstacleVision()//默认构造函数
    {
        histSize[0] = 256;//图像像素一共有256个值，所以直方图一共就有256项，每项对应的是像素值为该值的像素个数，或者是占总像素的比例大小
        hranges[0] = 0.0;//像素取值的最小值
        hranges[1] = 255.0;//像素取值的最大值
        ranges[0] = hranges;//浮点指针指向hranges数组 什么意思???
        channels[0] = 0;//默认情况，我们考察0号通道

        color2_center_rows    = -1;//y
        color2_center_cols    = -1;//color2重心x
        color2_area = -1;
        color2_valid          = false;

        color3_center_rows    = -1;//y
        color3_center_cols    = -1;//color2重心x
        color3_area = -1;
        color3_valid          = false;

        color1_center_rows = -1;
        color1_center_cols = -1;
        color1_area = -1;
        color1_valid       = false;

        start_file_num     = 1;
        max_file_num       = 500;

        canny_thre         = 103;
        canny_thre_max     = 500;

        debug_print        = 0;

        min_color2            = 87;
        max_color2            = 97;
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

/*********FiraObstacleVision color3*******/
//color3 Circle member

    cv::Mat std_color3;
    cv::Mat std_color3_hsv_channel[3];
    cv::MatND std_color3_hist;

    cv::Mat color3_project_result;

    double color3_center_rows;//y
    double color3_center_cols;//色标重心x
    double color3_height;
    bool color3_valid;
    double color3_area;
    cv::Rect color3_rect;

    int min_color3;
    int max_color3;

//color3 Circle function
public:

    void Process_color3(cv::Mat image);
    void Find_color3_center(cv::Mat src);
    int Judge_color3_legal(cv::Mat& src);
    cv::Mat color3_BackProject(const cv::MatND& hist, const cv::Mat& findimage);//反向投影
    void color3_setNotFound();

/*********FiraObstacleVision color2*******/
//color2 Circle member

    cv::Mat std_color2;
    cv::Mat std_color2_hsv_channel[3];
    cv::MatND std_color2_hist;

    cv::Mat color2_project_result;

    double color2_center_rows;//y
    double color2_center_cols;//色标重心x
    bool color2_valid;
        double color2_height;
    double color2_area;
    cv::Rect color2_rect;

    int min_color2;
    int max_color2;

//color2 Circle function
public:

    void Process_color2(cv::Mat image);
    void Find_color2_center(cv::Mat src);
    int Judge_color2_legal(cv::Mat& src);
    cv::Mat color2_BackProject(const cv::MatND& hist, const cv::Mat& findimage);//反向投影
    void color2_setNotFound();

/*********FiraObstacleVision color1*******/
//color1 Circle member

    cv::Mat std_color1;
    cv::Mat std_color1_hsv_channel[3];
    cv::MatND std_color1_hist;

    cv::Mat color1_project_result;

    double color1_center_rows;//y
    double color1_center_cols;//色标重心x
        double color1_height;
    bool color1_valid;
    double color1_area;
    cv::Rect color1_rect;

//color1 Circle function
public:

    void Process_color1(cv::Mat image);
    void Find_color1_center(cv::Mat src);
    int Judge_color1_legal(cv::Mat& src);
    cv::Mat color1_BackProject(const cv::MatND& hist, const cv::Mat& findimage);//反向投影
    void color1_setNotFound();
    int Judge_centerInRect(cv::Point2f center, cv::Rect rect);


//new

int color1_h_min_thre;
int color1_h_max_thre;
int color1_s_thre;

int color2_h_min_thre;
int color2_h_max_thre;
int color2_s_thre;

int color3_h_min_thre;
int color3_h_max_thre;
int color3_s_thre;

    void Load_parameter();
    void Store_parameter();
    void Set_parameter();

   FiraObstacleVisionResult MyResult;
};


#endif // FiraObstacleVision_H
