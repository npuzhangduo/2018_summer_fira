#include "opencv2/opencv.hpp"

#include "iostream"
#include "algorithm"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "imgproc.h"

using namespace cv;
using namespace std;

class Find_ColorblockResult : public ImgProcResult
{
public:
    cv::Point2f center;

    bool valid;
    
public:
    Find_ColorblockResult() :
        center(),

        valid(false)
    {}

    virtual void operator=(ImgProcResult &res)
    {
	Find_ColorblockResult *tmp = dynamic_cast<Find_ColorblockResult*>(&res);
        center = tmp->center;

	valid = tmp->valid;
    }
    void operator=(Find_ColorblockResult &res)
    {
        center = res.center;

	valid = res.valid;
    }
};

class Find_Colorblock : public ImgProc
{
public:
    cv::Mat src_img;
    cv::Mat gray_img;
    cv::Mat binary_img;
    cv::Mat hsv_img;
    cv::Mat hsv_channels[3];
    cv::Mat hsv_binary[3];
    cv::Mat and_img;
    cv::Mat erode_img;
    cv::Mat roi_img;

    double center_rows;//y
    double center_cols;//色标重心x

    int threshval_binary ;//灰度图的阈值
    int threshval_hsv_channel ;//H通道的阈值

    int start_file_num;//**change**
    int max_file_num;//**change**
public:
    Find_Colorblock()
   {
	center_rows = -1;//y
    	center_cols = -1;//色标重心x

       //threshval_binary = 131;//灰度图的阈值
       //threshval_hsv_channel = 123;//H通道的阈值

	threshval_binary = 141;//灰度图的阈值
       threshval_hsv_channel = 123;//H通道的阈值
       //threshval_hsv_channel = 133;//H通道的阈值

       start_file_num = 1;//**change**
        max_file_num = 800;//**change**
    }

    void imageProcess(cv::Mat img, ImgProcResult *Result);//总的对外接口(新)

    void Load_src_img(const char path_name[100]);//加载要处理的图片(老)
    void Get_HSV_img(cv::Mat src);//获取hsv图(老)
    void Get_Gray_img(cv::Mat src);//获取灰度图(老)
    void Pretreatment_Find_Colorblock();//预处理图像(新)

    void Find_center(cv::Mat src);//找到色标中心(新)**new**
    int Judge_legal(cv::Mat& src);//判断得到ROI大小是否合法(new)

    void Erode_img(cv::Mat src, cv::Mat& dst, int size);//膨胀(old)
    void Dilate_img(cv::Mat src, cv::Mat& dst, int size);//腐蚀(old)

    void Write_img(cv::Mat& src,  const char floder_name[30], int num);//****change****
    Find_ColorblockResult MyResult;
};
