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

class HistogramResult : public ImgProcResult
{
public:
   cv::Point2f ball_center;
   cv::Point2f basket_center;
   double basket_center_up;
   double basket_center_down;

   double basket_area;
   double basket_length;
   bool ball_valid;
   bool basket_valid;

public:
   HistogramResult() :
       ball_center(),
       basket_center(),
        basket_center_up(),
        basket_center_down(),

        basket_area(-1),
        basket_length(-1),

       ball_valid(false),
       basket_valid(false)
   {}

   virtual void operator=(ImgProcResult &res)
   {
   HistogramResult *tmp = dynamic_cast<HistogramResult*>(&res);
       ball_center = tmp->ball_center;
       basket_center = tmp->basket_center;
        basket_center_up = tmp -> basket_center_up;
        basket_center_down = tmp -> basket_center_down;

        basket_area = tmp->basket_area;
        basket_length = tmp->basket_length;
        ball_valid = tmp->ball_valid;
        basket_valid = tmp->basket_valid;
   }
   void operator=(HistogramResult &res)
   {
       ball_center = res.ball_center;
       basket_center = res.basket_center;

        basket_center_up = res.basket_center_up;
        basket_center_down = res.basket_center_down;

        basket_area = res.basket_area;
        basket_length = res.basket_length;
		
   ball_valid = res.ball_valid;
   basket_valid = res.basket_valid;
   }
};

class Histogram : public ImgProc
// class Histogram
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

    Histogram()//默认构造函数
    {
        histSize[0] = 256;//图像像素一共有256个值，所以直方图一共就有256项，每项对应的是像素值为该值的像素个数，或者是占总像素的比例大小
        hranges[0] = 0.0;//像素取值的最小值
        hranges[1] = 255.0;//像素取值的最大值
        ranges[0] = hranges;//浮点指针指向hranges数组 什么意思???
        channels[0] = 0;//默认情况，我们考察0号通道

        basket_center_rows = -1;//y
        basket_center_cols = -1;//basket重心x

        ball_center_rows = -1;//y
        ball_center_cols = -1;//ball重心x

        start_file_num = 1;
        max_file_num = 800;

        canny_thre = 103;
        canny_thre_max = 500;

        debug_print = 0;

        Load_parameter();
    }

    void Load_std_img();
    void Pretreat(cv::Mat image);
    cv::MatND getHistogram(const cv::Mat& image);//计算1D直方图
    cv::Mat getHistogram_Image(const cv::Mat& image);//计算1D直方图并返回其分布图像，将直方图可视化，变为柱状图
    void Erode_img(cv::Mat src, cv::Mat& dst, int size);//膨胀
    void Dilate_img(Mat src, Mat &dst, int size);
    void Write_img(cv::Mat& src,  const char floder_name[30], int num);
    int Judge_center_close();

/*********Basket*******/
//basket member
    cv::Mat std_basket;
    cv::Mat std_basket_hsv_channel[3];
    cv::MatND std_basket_hist;

    cv::Mat basket_project_result;

    double basket_center_rows;//y
    double basket_center_cols;//色标重心x

    double basket_up_y;
    double basket_down_y;
    double basket_length;
    double basket_area;

//basket function
public:
    void Process_Basket(cv::Mat image);
    void Find_Basket_center(cv::Mat src);
    int Judge_Basket_legal(cv::Mat& src);
    double Calc_Similar(cv::Mat& src);
    cv::Mat Basket_BackProject(const cv::MatND& hist, const cv::Mat& findimage);//反向投影
    void Not_Find_Basket();

/******Ball******/
//ball member

    cv::Mat std_ball_1;
    cv::Mat std_ball_2;

    cv::Mat std_ball_1_channel[3];
    cv::Mat std_ball_2_channel[3];

    cv::MatND std_ball_1_hist;
    cv::MatND std_ball_2_hist;

    cv::Mat ball_project_result;

    double ball_center_rows;//y
    double ball_center_cols;//色标重心x

    double ball_radius;

//ball function
public:
    void Process_Ball(cv::Mat image);//hough circle
    void Process_Ball_2(cv::Mat image);//thre
    void Find_Ball_center(cv::Mat src);
    int Judge_Ball_Legal(Vec3f circle);//判断合法性
    int Judge_Ball_Legal_2(cv::Mat &src);
    int Judge_Ball_Color_Legal(cv::Mat& src);
    cv::Mat Ball_BackProject(const cv::MatND& hist, const cv::Mat& findimage);//反向投影
    void Not_Find_Ball();

//Parameter
    int basket_h_min_thre;
    int basket_h_max_thre;
    int basket_s_thre;

    int ball_h_min_thre;
    int ball_h_max_thre;
    int ball_s_thre;

    void Set_parameter();
    void Load_parameter();
    void Store_parameter();

   HistogramResult MyResult;
};
