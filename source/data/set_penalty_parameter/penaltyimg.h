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
//#include "imgproc.h"

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

//class PenaltyImgResult:public ImgProcResult
//{
//public:
//    cv::Point2f ball_center;
//    cv::Point2f hole_center;

//    bool ball_valid;
//    bool hole_valid;

//public:
//    PenaltyImgResult():
//        ball_center(),
//        hole_center(),

//        ball_center(false),
//        hole_center(false)
//    {}

//    virtual void operator = (ImgProcResult &res)
//    {
//        PenaltyImgResult *tmp = dynamic_cast<PenaltyImgResult *>(&res);
//        ball_center = tmp->ball_center;
//        hole_center = tmp->hole_center;

//        ball_valid = tmp->ball_valid;
//        hole_valid = tmp->hole_valid;
//    }

//    void operator =(PenaltyImgResult &res)
//    {
//        ball_center = res.ball_center;
//        hole_center = res.hole_center;

//        ball_valid = res.ball_valid;
//        hole_valid = res.hole_valid;
//    }
//};

//class PenaltyImg : public ImgProc
class PenaltyImg
{

private:
    int histSize[1];//é¡¹çæ°é å³ç´æ¹å¾çæ¡æ°
    float hranges[2];//åç´ åå¼çæå¤§å¼åæå°å¼
    const float* ranges[1];//ä¸ä¸ªä¿å­æµ®ç¹åæéçæ°ç»ï¼æ°ç»å¤§å°ä¸º1ï¼æåæ¯ä¸é¡¹åå«ä¸¤ä¸ªåç´ çæ°ç»
    int channels[1];//ä»ç¨å°ä¸ä¸ªéé

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
    void imageProcess(cv::Mat image);
    //void imageProcess(cv::Mat img, ImgProcResult *Result);

    PenaltyImg()
    {
        histSize[0] = 256;//å¾ååç´ ä¸å±æ256ä¸ªå¼ï¼æä»¥ç´æ¹å¾ä¸å±å°±æ256é¡¹ï¼æ¯é¡¹å¯¹åºçæ¯åç´ å¼ä¸ºè¯¥å¼çåç´ ä¸ªæ°ï¼æèæ¯å æ»åç´ çæ¯ä¾å¤§å°
        hranges[0] = 0.0;//åç´ åå¼çæå°å¼
        hranges[1] = 255.0;//åç´ åå¼çæå¤§å¼
        ranges[0] = hranges;//æµ®ç¹æéæåhrangesæ°ç» ä»ä¹ææ???
        channels[0] = 0;//é»è®¤æåµï¼æä»¬èå¯0å·éé

        hole_center_rows    = -1;//y
        hole_center_cols    = -1;//holeéå¿x
        hole_valid          = false;

        ball_center_rows = -1;
        ball_center_cols = -1;
        ball_valid       = false;

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
    cv::MatND getHistogram(const cv::Mat& image);//è®¡ç®1Dç´æ¹å¾
    cv::Mat getHistogram_Image(const cv::Mat& image);//è®¡ç®1Dç´æ¹å¾å¹¶è¿åå¶åå¸å¾åï¼å°ç´æ¹å¾å¯è§åï¼åä¸ºæ±ç¶å¾
    double Count_possible(cv::Mat bianry_img, cv::Rect roi_rect);
    void Erode_img(cv::Mat src, cv::Mat& dst, int size);//è¨è
    void Dilate_img(Mat src, Mat &dst, int size);
    void Write_img(cv::Mat& src,  const char floder_name[30], int num);

/*********Penalty Hole************/
//hole  member

    cv::Mat std_hole;
    cv::Mat std_hole_hsv_channel[3];
    cv::MatND std_hole_hist;

    cv::Mat hole_project_result;

    double hole_center_rows;//y
    double hole_center_cols;//è²æ éå¿x
    bool hole_valid;
    cv::Rect hole_rect;


    int min_hole;
    int max_hole;

//hole  function
public:

    void Process_hole(cv::Mat image);
    void Find_hole_center(cv::Mat src);
    int Judge_hole_legal(cv::Mat& src);
    cv::Mat hole_BackProject(const cv::MatND& hist, const cv::Mat& findimage);//ååæå½±
    void hole_setNotFound();


/*********Penalty Ball*******/
//ball  member

        cv::Mat std_ball;
        cv::Mat std_ball_hsv_channel[3];
        cv::MatND std_ball_hist;

        cv::Mat ball_project_result;

        double ball_center_rows;//y
        double ball_center_cols;//è²æ éå¿x
        bool ball_valid;
        cv::Rect ball_rect;

//ball  function
public:

        void Process_ball(cv::Mat image);
        void Find_ball_center(cv::Mat src);
        int Judge_ball_legal(cv::Mat& src);
        cv::Mat ball_BackProject(const cv::MatND& hist, const cv::Mat& findimage);//ååæå½±
        void ball_setNotFound();



/*********Parameter*********/
//Parameter member
//        int hole_h_min_thre;
//        int hole_h_max_thre;
//        int hole_s_thre;

//        int ball_h_min_thre;
//        int ball_h_max_thre;
//        int ball_s_thre;

        void Load_parameter();
        void Store_parameter();
        void Set_parameter();

//        GolfVisionResult MyResult;
};


#endif // PENALTYIMG_H
