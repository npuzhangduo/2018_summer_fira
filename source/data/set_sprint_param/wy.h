/****  final version to sprint hsv   *****/
/**********     attention   *************/
/*** trackbar!!! cv::split(hsv_img, trackbar_hsv_channel);****/

#ifndef WY_H
#define WY_H

#include "iostream"
#include "algorithm"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "fstream"
#include "sstream"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class wy
{
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

public:
//     void imageProcess(cv::Mat image, ImgProcResult *Result);
     void imageProcess(cv::Mat image);

     wy()
     {
         histSize[0] = 256;//图像像素一共有256个值，所以直方图一共就有256项，每项对应的是像素值为该值的像素个数，或者是占总像素的比例大小
          hranges[0] = 0.0;//像素取值的最小值
          hranges[1] = 255.0;//像素取值的最大值
          ranges[0] = hranges;//浮点指针指向hranges数组 什么意思???
          channels[0] = 0;//默认情况，我们考察0号通道

          obj_center_rows    = -1;//y
          obj_center_cols    = -1;//basket重心x

          center_rows        = -1;
          center_cols        = -1;
          valid              = false;
          start_file_num     = 1;
          max_file_num       = 500;

          canny_thre         = 103;
          canny_thre_max     = 500;

          debug_print        = 0;

          min_big            = 87;
          max_big            = 97;
          Load_parameter();
     }
     double center_rows;//y
     double center_cols;//色标重心x
     bool   valid;

     void Load_std_img();
     void Pretreat(cv::Mat image);
     cv::MatND getHistogram(const cv::Mat& image);//计算1D直方图
     cv::Mat getHistogram_Image(const cv::Mat& image);//计算1D直方图并返回其分布图像，将直方图可视化，变为柱状图
     void Erode_img(cv::Mat src, cv::Mat& dst, int size);//膨胀
     void Dilate_img(Mat src, Mat &dst, int size);
//     void Write_img(cv::Mat& src,  const char floder_name[30], int num);
     void set_NotFound();

     /*********stick*******/
     //stick member
     cv::Mat std_big;
     cv::Mat std_big_hsv_channel[3];
     cv::MatND std_big_hist;

    double obj_center_rows;
    double obj_center_cols;
    double obj_up;
    double obj_down;
    double obj_angle;

     cv::Mat big_project_result;

     cv::Rect big_rect;
     cv::RotatedRect big_minrect;//new

     int min_big;
     int max_big;
     //stick function

public:
    void Process_big(cv::Mat image);
    void Find_big_center(cv::Mat src);
    int Judge_big_legal(cv::Mat& src);
    cv::Mat big_BackProject(const cv::MatND& hist, const cv::Mat& findimage);//反向投影
    void big_setNotFound();

    //new

//    int big_h_min_thre;
//    int big_h_max_thre;
//    int big_s_thre;

    void Load_parameter();
    void Store_parameter();
    void Set_parameter();

//    wyResult MyResult;
};

#endif // WY_H

