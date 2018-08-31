#ifndef PENALTYIMG_H
#define PENALTYIMG_H



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

using namespace cv;
using namespace std;

class PenaltyImgResult:public ImgProcResult
//class PenaltyImgResult
{
public:
    cv::Point2f ball_center;
    cv::Point2f hole_center;

    bool ball_valid;
    bool hole_valid;

    ///AB
    bool hole_right_vaild;

public:
    PenaltyImgResult():
        ball_center(),
        hole_center(),

        ball_valid(false),
        hole_valid(false),

        ///AB
        hole_right_vaild(false)
    {}

    virtual void operator = (ImgProcResult &res)
    {
        PenaltyImgResult *tmp = dynamic_cast<PenaltyImgResult *>(&res);
        ball_center = tmp->ball_center;
        hole_center = tmp->hole_center;

        ball_valid = tmp->ball_valid;
        hole_valid = tmp->hole_valid;
	

        ///AB
        hole_right_vaild = tmp->hole_right_vaild;
    }

    void operator =(PenaltyImgResult &res)
    {
        ball_center = res.ball_center;
        hole_center = res.hole_center;

        ball_valid = res.ball_valid;
        hole_valid = res.hole_valid;

        ///AB
        hole_right_vaild = res.hole_right_vaild;
    }
};

class PenaltyImg:public ImgProc
//class PenaltyImg
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

//public function
public:
    // void imageProcess(cv::Mat image);
   void imageProcess(cv::Mat img, ImgProcResult *Result);

    PenaltyImg()
    {
        histSize[0] = 256;//图像像素一共有256个值，所以直方图一共就有256项，每项对应的是像素值为该值的像素个数，或者是占总像素的比例大小
        hranges[0] = 0.0;//像素取值的最小值
        hranges[1] = 255.0;//像素取值的最大值
        ranges[0] = hranges;//浮点指针指向hranges数组 什么意思???
        channels[0] = 0;//默认情况，我们考察0号通道



        ball_center_rows = -1;
        ball_center_cols = -1;
        ball_valid       = false;

        start_file_num     = 1;
        max_file_num       = 500;

        canny_thre         = 103;
        canny_thre_max     = 500;

        debug_print        = 1;

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

/*********Penalty Hole************/
//hole  member
    bool hole_vaild;
    bool hole_right_vaild;
    int hole_edge;

    int kmeans_left;
    int kmeans_right;

    cv::Mat lo_used_channel;
    cv::Mat lo_used_channel_without_FBground;

    std::vector<double> vertical_projection;
    std::vector<double> horizontal_projection;

//hole  function
public:

    ///AB
    void get_uesd_channels();
    void clear_FBground();
    void threshold_projection_image();
    void get_object_position();

    int Judge_hole_legal();
    void hole_setNotFound();
    /*backup functions*/
    void get_vertical_projection(cv::Mat inputImg);
    void get_horizontal_projection(cv::Mat inputImg);



/*********Penalty Ball*******/
//ball  member

    cv::Mat std_ball;
    cv::Mat std_ball_hsv_channel[3];
    cv::MatND std_ball_hist;

    cv::Mat ball_project_result;

    double ball_center_rows;//y
    double ball_center_cols;//色标重心x
    bool ball_valid;
    cv::Rect ball_rect;

//ball  function
public:

    void Process_ball();
    void Find_ball_center(cv::Mat src);
    int Judge_ball_legal(cv::Mat& src);
    cv::Mat ball_BackProject(const cv::MatND& hist, const cv::Mat& findimage);//反向投影
    void ball_setNotFound();



/*********Parameter*********/
//Parameter member

    int ball_h_min_thre;
    int ball_h_max_thre;
    int ball_s_thre;

    void Load_parameter();
    void Store_parameter();
    void Set_parameter();

    PenaltyImgResult MyResult;
};


#endif // PENALTYIMG_H
