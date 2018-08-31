//**************************************************************************
//--------------------------------------------------------------------------
// Set fira_obstacle Paramenter by HSV
//
// ZheYu Ma
// version 1.0
// Date    2017.9.24
//--------------------------------------------------------------------------
//**************************************************************************
/**
 * @todo 要现场采样，更新草场标准图.
 *      1.确认构造函数std_img 路径是否更新
 *          std_img = cv::imread("../source/data/std_img/std_combine.jpg");
 *      2.确认头文件和cpp里面Imgprocess函数是否更改 void FiraObstacleVision::imageProcess(cv::Mat image, ImgProcResult *Result)
 *      3.确认头文件中的类名是否更改class FiraObstacleVision : public ImgProc
 *      4.确认类中是否增加成员变量FiraObstacleVisionResult MyResult;
 *      5.确认头文件中是否增加class FiraObstacleVisionResult : public ImgProcResult
 *      6.确认imgprocess函数中是否给Result类返回参数信息
 *
 *        MyResult.center.x = center_cols;
 *        MyResult.center.y = center_rows;
 *        MyResult.valid    = false;
 *        (*dynamic_cast<FiraObstacleVisionResult*>(Result)) = MyResult;
 *
 *      7.是否在imgprocess函数末尾增加 (*dynamic_cast<FiraObstacleVisionResult*>(Result)) = MyResult;
 *      8.是否更改了调试时图片的输出路径
 *       	char path[150] = {"../source/data/con_img/"};
 *      9.是否在主函数中使用了imshow，或者waitKey(0);
 *      10.是否在头文件中添加头文件#include "imgproc.h"
 *      11.是否更改参数文件路径ifstream ifile("3.txt");
 */

#include "fira_obstacle_vision.h"

int color1_h_min_thre;
int color1_h_max_thre;
int color1_s_thre;

int color2_h_min_thre;
int color2_h_max_thre;
int color2_s_thre;

int color3_h_min_thre;
int color3_h_max_thre;
int color3_s_thre;

Mat color1_s_threimg;
Mat color1_h_threimg;
Mat color1_and_img;

Mat color2_s_threimg;
Mat color2_h_threimg;
Mat color2_and_img;

Mat color3_s_threimg;
Mat color3_h_threimg;
Mat color3_and_img;

Mat trackbar_hsv_channel[3];

void on_trackbar( int, void* )
{
//universal setting
    int col_1 = trackbar_hsv_channel[0].cols;
    int col_2 = 2*col_1;
    int col_3 = col_2 + col_1;

//hsv combine
    Mat hsv_combine = Mat(cv::Size(2*trackbar_hsv_channel[0].cols, trackbar_hsv_channel[0].rows), CV_8U);
    cv::Mat combine_h   = hsv_combine.colRange(0, col_1);
    cv::Mat combine_s   = hsv_combine.colRange(col_1, col_2);

    trackbar_hsv_channel[0].copyTo(combine_h);
    trackbar_hsv_channel[1].copyTo(combine_s);

    hsv_combine.col(col_1).setTo(255);

    imshow("hsv_combine", hsv_combine);

//
    threshold(trackbar_hsv_channel[1], color1_s_threimg, color1_s_thre, 255, cv::THRESH_BINARY);

    color1_h_threimg = trackbar_hsv_channel[0].clone();
    for(int i = 0; i < trackbar_hsv_channel[0].rows; i++){
        for(int j = 0; j < trackbar_hsv_channel[0].cols; j++){
            int value = trackbar_hsv_channel[0].at<uchar>(i,j);
            if(value >= color1_h_min_thre && value <= color1_h_max_thre){
                color1_h_threimg.at<uchar>(i,j) = 255;
            }else{
                color1_h_threimg.at<uchar>(i,j) = 0;
            }
        }
    }
    //erode and dilate
    int size = 2;
    cv::Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(color1_h_threimg, color1_h_threimg, element, Point(-1, -1), 1);
    dilate(color1_h_threimg, color1_h_threimg, element, Point(-1, -1), 1);

    bitwise_and(color1_h_threimg, color1_s_threimg, color1_and_img);

    //show
    Mat color1_combine = Mat(cv::Size(3*trackbar_hsv_channel[0].cols, trackbar_hsv_channel[0].rows), CV_8U);
    cv::Mat combine_rect_and   = color1_combine.colRange(0, col_1);
    cv::Mat combine_rect_hthre = color1_combine.colRange(col_1, col_2);
    cv::Mat combine_rect_sthre = color1_combine.colRange(col_2, col_3);

    color1_and_img.copyTo(combine_rect_and);
    color1_h_threimg.copyTo(combine_rect_hthre);
    color1_s_threimg.copyTo(combine_rect_sthre);

    color1_combine.col(col_1).setTo(255);
    color1_combine.col(col_2).setTo(255);

    imshow("color1_combine", color1_combine);

//color2
    threshold(trackbar_hsv_channel[1], color2_s_threimg, color2_s_thre, 255, cv::THRESH_BINARY);

    color2_h_threimg = trackbar_hsv_channel[0].clone();
    for(int i = 0; i < trackbar_hsv_channel[0].rows; i++){
        for(int j = 0; j < trackbar_hsv_channel[0].cols; j++){
            int value = trackbar_hsv_channel[0].at<uchar>(i,j);
            if(value >= color2_h_min_thre && value <= color2_h_max_thre){
                color2_h_threimg.at<uchar>(i,j) = 255;
            }else{
                color2_h_threimg.at<uchar>(i,j) = 0;
            }
        }
    }
    //erode and dilate
    size = 1;
    element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(color2_h_threimg, color2_h_threimg, element, Point(-1, -1), 1);
    dilate(color2_h_threimg, color2_h_threimg, element, Point(-1, -1), 1);

    bitwise_and(color2_h_threimg, color2_s_threimg, color2_and_img);

    //show
    Mat color2_combine = Mat(cv::Size(3*trackbar_hsv_channel[0].cols, trackbar_hsv_channel[0].rows), CV_8U);
    combine_rect_and   = color2_combine.colRange(0, col_1);
    combine_rect_hthre = color2_combine.colRange(col_1, col_2);
    combine_rect_sthre = color2_combine.colRange(col_2, col_3);

    color2_and_img.copyTo(combine_rect_and);
    color2_h_threimg.copyTo(combine_rect_hthre);
    color2_s_threimg.copyTo(combine_rect_sthre);

    color2_combine.col(col_1).setTo(255);
    color2_combine.col(col_2).setTo(255);

    imshow("color2_combine", color2_combine);

//color3
    threshold(trackbar_hsv_channel[1], color3_s_threimg, color3_s_thre, 255, cv::THRESH_BINARY);

    color3_h_threimg = trackbar_hsv_channel[0].clone();
    for(int i = 0; i < trackbar_hsv_channel[0].rows; i++){
        for(int j = 0; j < trackbar_hsv_channel[0].cols; j++){
            int value = trackbar_hsv_channel[0].at<uchar>(i,j);
            if(value >= color3_h_min_thre && value <= color3_h_max_thre){
                color3_h_threimg.at<uchar>(i,j) = 255;
            }else{
                color3_h_threimg.at<uchar>(i,j) = 0;
            }
        }
    }
    //erode and dilate
    size = 1;
    element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(color3_h_threimg, color3_h_threimg, element, Point(-1, -1), 1);
    dilate(color3_h_threimg, color3_h_threimg, element, Point(-1, -1), 1);

    bitwise_and(color3_h_threimg, color3_s_threimg, color3_and_img);

    //show
    Mat color3_combine = Mat(cv::Size(3*trackbar_hsv_channel[0].cols, trackbar_hsv_channel[0].rows), CV_8U);
    combine_rect_and   = color3_combine.colRange(0, col_1);
    combine_rect_hthre = color3_combine.colRange(col_1, col_2);
    combine_rect_sthre = color3_combine.colRange(col_2, col_3);

    color3_and_img.copyTo(combine_rect_and);
    color3_h_threimg.copyTo(combine_rect_hthre);
    color3_s_threimg.copyTo(combine_rect_sthre);

    color3_combine.col(col_1).setTo(255);
    color3_combine.col(col_2).setTo(255);

    imshow("color3_combine", color3_combine);
}

/******Public******/
//void FiraObstacleVision::imageProcess(cv::Mat image, ImgProcResult *Result)
void FiraObstacleVision::imageProcess(cv::Mat image)
{
    color2_setNotFound();
    color1_setNotFound();
    color3_setNotFound();

    Load_std_img();
    Pretreat(image);

    Process_color2(image);
    Process_color1(image);
    Process_color3(image);

    Set_parameter();

    /*******************************************/
    if (color2_rect.x != -1) 
    {
        rectangle(center_img, color2_rect, cv::Scalar(255, 0, 0), 2, 8);

        color2_center_rows = color2_rect.y+color2_rect.height*0.5;
        color2_center_cols = color2_rect.x+color2_rect.width*0.5;
        color2_valid = true;
    }
    else
    {
        color2_valid = false;
        //printf("color2 not exist\n");
    }

    if (color1_rect.x != -1) 
    {
        rectangle(center_img, color1_rect, cv::Scalar(255, 0, 0), 2, 8);

        color1_center_rows = color1_rect.y+color1_rect.height*0.5;
        color1_center_cols = color1_rect.x+color1_rect.width*0.5;
        color1_valid = true;
    }
    else
    {
        color1_valid = false;
//        printf("color1 not exist\n");
    }

    if (color3_rect.x != -1) 
    {
        rectangle(center_img, color3_rect, cv::Scalar(255, 0, 0), 2, 8);

        color3_center_rows = color3_rect.y+color3_rect.height*0.5;
        color3_center_cols = color3_rect.x+color3_rect.width*0.5;
        color3_valid = true;
    }
    else
    {
        color3_valid = false;
        //printf("color2 not exist\n");
    }

    /*******************************************/

    Write_img(src_img, "src_img", start_file_num);
    Write_img(center_img, "center_img", start_file_num);
    Write_img(color1_project_result, "color1project_img", start_file_num);
    Write_img(color2_project_result, "color2project_img", start_file_num);

    start_file_num++;

//    MyResult.center.x = center_cols;
//    MyResult.center.y = center_rows;
//    MyResult.valid    = false;
//    (*dynamic_cast<FiraObstacleVisionResult*>(Result)) = MyResult;

}

void FiraObstacleVision::Load_std_img()
{
//    std_color1    = imread("D:/Robot/HuroCup 2017/Prepare/FiraObstacleVision/std_img/1/y.png"); //yellow
//    std_color2 = imread("D:/Robot/HuroCup 2017/Prepare/FiraObstacleVision/std_img/1/r1.png");//red

    // std_color1= cv::imread("/home/zenghao/HuroCup 2017/Prepare/FiraObstacleVision/std_img/1/y1.png");
//    std_color1    = imread("/home/zenghao/HuroCup 2017/Prepare/FiraObstacleVision/std_img/1/y.png"); //yellow
//    std_color2 = imread("/home/zenghao/HuroCup 2017/Prepare/FiraObstacleVision/std_img/1/r1.png");//red

    std_color1    = imread("y.png"); //yellow
    std_color2 = imread("r1.png");//red

    cv::cvtColor(std_color1, std_color1, CV_RGB2HSV);
    cv::cvtColor(std_color2, std_color2, CV_RGB2HSV);

    cv::split(std_color1, std_color1_hsv_channel);
    cv::split(std_color2, std_color2_hsv_channel);

//    imshow("std_color1_channel", std_color1_hsv_channel[0]);
//    imshow("std_color2_channel", std_color2_hsv_channel[0]);

    std_color1_hist    = getHistogram(std_color1_hsv_channel[0]);
    std_color2_hist = getHistogram(std_color2_hsv_channel[0]);

    cv::normalize(std_color1_hist, std_color1, 1.0);
    cv::normalize(std_color2_hist, std_color2_hist, 1.0);

}

void FiraObstacleVision::Pretreat(cv::Mat image)
{
    //basket
    src_img = image.clone();
    center_img = src_img.clone();
    circles_img = src_img.clone();
    cvtColor( src_img, gray_img, CV_BGR2GRAY );//高阶的canny用法，转成灰度图，降噪，用canny
    cv::cvtColor(image, hsv_img, CV_RGB2HSV);
    cv::split(hsv_img, channel);
    cv::split(hsv_img, trackbar_hsv_channel);

//    imshow("channel[0]", channel[0]);
//    imshow("channel[1]", channel[1]);
//    imshow("channel[2]", channel[2]);

    GaussianBlur( gray_img, blur_img, Size(9, 9), 2, 2 );
    Canny(hsv_img, canny_img, canny_thre, canny_thre_max);
}

void FiraObstacleVision::Dilate_img(Mat src, Mat &dst, int size)
{
    Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    dilate(src, dst, element, Point(-1, -1), 1);
}

void FiraObstacleVision::Erode_img(cv::Mat src, cv::Mat &dst, int size)
{
    cv::Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(src, dst, element, Point(-1, -1), 1);
}

cv::MatND FiraObstacleVision::getHistogram(const cv::Mat &image)
{
    cv::MatND hist;//创建直方图
    cv::calcHist(//计算直方图
        &image,//待计算的图像
        1,//计算单张图像的直方图
        channels,//通道数量
        cv::Mat(),//不使用图像作为掩码
        hist,//返回的直方图
        1,//这是1D的直方图
        histSize,//项的数量，即直方图的条目数
        ranges//像素值的范围，是一组每项包含两个元素的数组
    );
    return hist;
}

cv::Mat FiraObstacleVision::getHistogram_Image(const cv::Mat &image)
{
    cv::MatND hist = getHistogram(image);//首先计算直方图

    double maxVal = 0;
    double minVal = 0;
    cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);//获取最大值和最小值

    //显示直方图的图像
    cv::Mat histImg(histSize[0], histSize[0], CV_8U, cv::Scalar(255));//赋值为白色,长度和宽度都为255

    int hpt = static_cast<int>(0.9 * histSize[0]); //设置最高点为nbins的90%
    for (int h = 0; h < histSize[0]; h++) //每个条目都绘制一条垂直线
    {
        float binVal = hist.at<float>(h);
        int intensity = static_cast<int>(hpt * (binVal / maxVal));
        cv::line(histImg, cv::Point(h, histSize[0]), cv::Point(h, histSize[0] - intensity), cv::Scalar::all(0));
    }
    return histImg;
}

void FiraObstacleVision::Write_img(cv::Mat& src,  const char floder_name[30], int num)
{
    if (start_file_num <= max_file_num)
    {
        char path[150] = {"../source/data/con_img/"};

        char tail[10] = {".jpg"};
        char number[4];
        sprintf(number, "%d", num);
        strcat(path, floder_name);
        strcat(path, "/");
        strcat(path, number);
        strcat(path, tail);

        cv::imwrite(path, src);
    }
}

/******color1 Circle******/

void FiraObstacleVision::Process_color1(cv::Mat image)
{
    image = channel[0].clone();

    color1_project_result = color1_BackProject(std_color1_hist, image);

    for(int i = 0; i < channel[0].rows; i++){
        for(int j = 0; j < channel[0].cols; j++){
            int value = channel[0].at<uchar>(i,j);
            if(value >= color1_h_min_thre && value <= color1_h_max_thre){
                color1_project_result.at<uchar>(i,j) = 255;
            }else{
                color1_project_result.at<uchar>(i,j) = 0;
            }
        }
    }

    Mat thre_s;
    threshold(channel[1], thre_s, color1_s_thre, 255, CV_THRESH_BINARY);
    bitwise_and(thre_s, color1_project_result, color1_project_result);

    imshow("color1_backproject", color1_project_result);

    Find_color1_center(color1_project_result);
}

void FiraObstacleVision::Find_color1_center(cv::Mat src)
{
    color1_center_rows = color1_center_cols = -1;
    cv::Mat img = src.clone();

    std::vector<vector<Point> > contours;
    std::vector<vector<Point> > contours_poly;
    std::vector<RotatedRect> minRect;

    Scalar color = Scalar( 125, 16, 210 );   //定义画的边框颜色
    Scalar color2 = Scalar(210, 16, 125);
    Scalar color3 = Scalar(16, 210, 125);

    cv::findContours(img, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0)); //画出img的轮廓 但貌似会改变原图像
    contours_poly.resize(contours.size());//一定要重新改变size使其与contours的一样，否则会崩溃
    vector<Rect> boundRect( contours.size() );//包围点集的最小矩形vector
    vector<Point2f>center( contours.size() ); //包围点集的最小圆形vector
    vector<float>radius( contours.size() ); //包围点集的最小圆形半径vector
    minRect.resize(contours.size());

    for (unsigned int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP(contours[i], contours_poly[i], 3, true );//对轮廓点进行多边形拟合，因为原来直接得到的太支离破碎，把边缘近似为直线
        minRect[i] = minAreaRect(contours_poly[i]);//zh:通过轮廓信息获得轮廓包含轮廓外围的最小矩形,是可以旋转的
        boundRect[i] = boundingRect(contours_poly[i]);//计算并返回包围轮廓点集的最小矩形,但不能画旋转矩形 好像两种都ok
        minEnclosingCircle( contours_poly[i], center[i], radius[i] );//计算并返回包围轮廓点集的最小圆形及其半径
    }
    Mat drawing = Mat::zeros( img.size(), CV_8UC3 );//返回零数组
    Mat drawing_approx = Mat::zeros( img.size(), CV_8UC3 );//返回零数组

    for (unsigned int i = 0; i < contours.size(); i++ )
    {
        drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );//画没有拟合前的边框
        drawContours( drawing_approx, contours_poly, i, color, 2, 8, vector<Vec4i>(), 0, Point() );//画拟合后的边框
    }//画边界轮廓

//    imshow("drawcontous", drawing);

    for (unsigned i = 0; i < contours.size(); i++ )
    {
        circle( drawing_approx, center[i], (int)radius[i], color3, 2, 8, 0 ); //画圆形
    }

    if (contours.size() > 0)
    {
        float max_radius = 0.0;
        int most_possible = -1;
        for (unsigned int i = 0; i < contours.size(); i++ )
        {
            cv::Mat roi_img = src(cv::Rect(boundRect[i].x, boundRect[i].y, boundRect[i].width, boundRect[i].height));
			if (Judge_color1_legal(roi_img) == 0)
	        	continue;
            else if (radius[i] > max_radius)
            {
                most_possible = i;
                max_radius = radius[i];//找出最大的半径，也就是最可能的值
            }
        }//找最大半径
//////////////////////////////////////

        if (most_possible != -1)
        {
            circle( drawing_approx, center[most_possible], (int)radius[most_possible], color2, 2, 8, 0 ); //画圆

            cv::Mat roi_img = src(cv::Rect(boundRect[most_possible].x, boundRect[most_possible].y, boundRect[most_possible].width, boundRect[most_possible].height));
            if (Judge_color1_legal(roi_img) == 0)
            {
                color1_setNotFound();
            }
            else
            {
                color1_center_rows = center[most_possible].y;
                color1_center_cols = center[most_possible].x;
                color1_rect        = boundRect[most_possible];
            }
        }
        else
        {
            color1_setNotFound();
        }
    }
    else
    {
        color1_setNotFound();
    }
}

int FiraObstacleVision::Judge_color1_legal(cv::Mat& src)
{
    if (src.rows <= 15 || src.cols <= 15)
	{
		printf("hhh\n");
        return 0;
	}
    else
        return 1;
}

cv::Mat FiraObstacleVision::color1_BackProject(const cv::MatND& hist, const cv::Mat& findimage)
{

    cv::Mat result;//保存生成的反向投影图像
    cv::calcBackProject(&findimage,
                        1,//一副图像
                        channels,//通道数量
                        hist,//进行反向投影的直方图
                        result,//生成的反投影图像
                        ranges,//每个维度的值域
                        255.0//缩放因子
                       );

    double threshold = 0.03;//大于此概率的像素点全部变为255
    cv::threshold(result, result, 255 * threshold, 255, cv::THRESH_BINARY); //阈值化变为二值图像
    //应为之前缩放了255倍所以这边阈值threshold也要乘以255倍
    Erode_img(result, result, 2);
    Dilate_img(result, result, 2);
    return result;
}

void FiraObstacleVision::color1_setNotFound()
{
    color1_center_rows = color1_center_cols = -1;
    color1_rect.x = color1_rect.y = -1;
    color1_valid = false;
}

/******color2 Circle******/

void FiraObstacleVision::Process_color2(cv::Mat image)
{
    image = channel[0].clone();

    color2_project_result = color2_BackProject(std_color2_hist, image);

    for(int i = 0; i < channel[0].rows; i++){
        for(int j = 0; j < channel[0].cols; j++){
            int value = channel[0].at<uchar>(i,j);
            if(value >= color2_h_min_thre && value <= color2_h_max_thre){
                color2_project_result.at<uchar>(i,j) = 255;
            }else{
                color2_project_result.at<uchar>(i,j) = 0;
            }
        }
    }

    Mat thre_s;
    threshold(channel[1], thre_s, color2_s_thre, 255, CV_THRESH_BINARY);
    bitwise_and(thre_s, color2_project_result, color2_project_result);

    imshow("color2_backproject", color2_project_result);

    Find_color2_center(color2_project_result);
}

void FiraObstacleVision::Find_color2_center(cv::Mat src)
{
    color2_center_rows = color2_center_cols = -1;
    cv::Mat img = src.clone();

    std::vector<vector<Point> > contours;
    std::vector<vector<Point> > contours_poly;
    std::vector<RotatedRect> minRect;

    Scalar color = Scalar( 125, 16, 210 );   //定义画的边框颜色
    Scalar color3 = Scalar(16, 210, 125);

    cv::findContours(img, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0)); //画出img的轮廓 但貌似会改变原图像
    contours_poly.resize(contours.size());//一定要重新改变size使其与contours的一样，否则会崩溃
    vector<Rect> boundRect( contours.size() );//包围点集的最小矩形vector
    vector<Point2f>center( contours.size() ); //包围点集的最小圆形vector
    vector<float>radius( contours.size() ); //包围点集的最小圆形半径vector
    minRect.resize(contours.size());

    for (unsigned int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP(contours[i], contours_poly[i], 3, true );//对轮廓点进行多边形拟合，因为原来直接得到的太支离破碎，把边缘近似为直线
        minRect[i] = minAreaRect(contours_poly[i]);//zh:通过轮廓信息获得轮廓包含轮廓外围的最小矩形,是可以旋转的
        boundRect[i] = boundingRect(contours_poly[i]);//计算并返回包围轮廓点集的最小矩形,但不能画旋转矩形 好像两种都ok
        minEnclosingCircle( contours_poly[i], center[i], radius[i] );//计算并返回包围轮廓点集的最小圆形及其半径
    }
    Mat drawing = Mat::zeros( img.size(), CV_8UC3 );//返回零数组
    Mat drawing_approx = Mat::zeros( img.size(), CV_8UC3 );//返回零数组

    for (unsigned int i = 0; i < contours.size(); i++ )
    {
        drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );//画没有拟合前的边框
        drawContours( drawing_approx, contours_poly, i, color, 2, 8, vector<Vec4i>(), 0, Point() );//画拟合后的边框
    }//画边界轮廓

    for (unsigned i = 0; i < contours.size(); i++ )
    {
        circle( drawing_approx, center[i], (int)radius[i], color3, 2, 8, 0 ); //画圆形
    }

    if (contours.size() > 0)
    {
        float max_radius = 0.0;
        int most_possible = -1;
        for (unsigned int i = 0; i < contours.size(); i++ )
        {
            cv::Mat roi_img = src(cv::Rect(boundRect[i].x, boundRect[i].y, boundRect[i].width, boundRect[i].height));
			if (Judge_color1_legal(roi_img) == 0)
	        	continue;
            else if (radius[i] > max_radius)
            {
                most_possible = i;
                max_radius = radius[i];//找出最大的半径，也就是最可能的值
            }
        }//找最大半径
//////////////////////////////////////

        if (most_possible != -1)
        {
            color2_center_rows = center[most_possible].y;
            color2_center_cols = center[most_possible].x;
            color2_rect        = boundRect[most_possible];

        }
        else
        {
            color2_setNotFound();
        }
    }
    else
    {
        color2_setNotFound();
    }
}

int FiraObstacleVision::Judge_color2_legal(cv::Mat& src)
{
    if (src.rows >= 50 || src.cols >= 50 || src.rows <= 5 || src.cols <= 5)
        return 0;
    else
        return 1;
}

cv::Mat FiraObstacleVision::color2_BackProject(const cv::MatND& hist, const cv::Mat& findimage)
{

    cv::Mat result;//保存生成的反向投影图像
    cv::calcBackProject(&findimage,
                        1,//一副图像
                        channels,//通道数量
                        hist,//进行反向投影的直方图
                        result,//生成的反投影图像
                        ranges,//每个维度的值域
                        255.0//缩放因子
                       );

    double threshold = 0.03;//大于此概率的像素点全部变为255
    cv::threshold(result, result, 255 * threshold, 255, cv::THRESH_BINARY); //阈值化变为二值图像
    //应为之前缩放了255倍所以这边阈值threshold也要乘以255倍
    Erode_img(result, result, 2);
    Dilate_img(result, result, 4);
    return result;
}

void FiraObstacleVision::color2_setNotFound()
{
    color2_center_rows = color2_center_cols = -1;
    color2_rect.x = color2_rect.y = -1;
    color2_valid = false;
}

//color3
void FiraObstacleVision::Process_color3(cv::Mat image)
{
    image = channel[0].clone();

    //color3_project_result = color3_BackProject(std_color3_hist, image);
    color3_project_result = image;

    for(int i = 0; i < channel[0].rows; i++){
        for(int j = 0; j < channel[0].cols; j++){
            int value = channel[0].at<uchar>(i,j);
            if(value >= color3_h_min_thre && value <= color3_h_max_thre){
                color3_project_result.at<uchar>(i,j) = 255;
            }else{
                color3_project_result.at<uchar>(i,j) = 0;
            }
        }
    }

    Mat thre_s;
    threshold(channel[1], thre_s, color3_s_thre, 255, CV_THRESH_BINARY);
    bitwise_and(thre_s, color3_project_result, color3_project_result);

    imshow("color3_backproject", color3_project_result);

    Find_color3_center(color3_project_result);
}

void FiraObstacleVision::Find_color3_center(cv::Mat src)
{
    color3_center_rows = color3_center_cols = -1;
    cv::Mat img = src.clone();

    std::vector<vector<Point> > contours;
    std::vector<vector<Point> > contours_poly;
    std::vector<RotatedRect> minRect;

    Scalar color = Scalar( 125, 16, 210 );   //定义画的边框颜色
    Scalar color3 = Scalar(16, 210, 125);

    cv::findContours(img, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0)); //画出img的轮廓 但貌似会改变原图像
    contours_poly.resize(contours.size());//一定要重新改变size使其与contours的一样，否则会崩溃
    vector<Rect> boundRect( contours.size() );//包围点集的最小矩形vector
    vector<Point2f>center( contours.size() ); //包围点集的最小圆形vector
    vector<float>radius( contours.size() ); //包围点集的最小圆形半径vector
    minRect.resize(contours.size());

    for (unsigned int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP(contours[i], contours_poly[i], 3, true );//对轮廓点进行多边形拟合，因为原来直接得到的太支离破碎，把边缘近似为直线
        minRect[i] = minAreaRect(contours_poly[i]);//zh:通过轮廓信息获得轮廓包含轮廓外围的最小矩形,是可以旋转的
        boundRect[i] = boundingRect(contours_poly[i]);//计算并返回包围轮廓点集的最小矩形,但不能画旋转矩形 好像两种都ok
        minEnclosingCircle( contours_poly[i], center[i], radius[i] );//计算并返回包围轮廓点集的最小圆形及其半径
    }
    Mat drawing = Mat::zeros( img.size(), CV_8UC3 );//返回零数组
    Mat drawing_approx = Mat::zeros( img.size(), CV_8UC3 );//返回零数组

    for (unsigned int i = 0; i < contours.size(); i++ )
    {
        drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );//画没有拟合前的边框
        drawContours( drawing_approx, contours_poly, i, color, 2, 8, vector<Vec4i>(), 0, Point() );//画拟合后的边框
    }//画边界轮廓

    for (unsigned i = 0; i < contours.size(); i++ )
    {
        circle( drawing_approx, center[i], (int)radius[i], color3, 2, 8, 0 ); //画圆形
    }

    if (contours.size() > 0)
    {
        float max_radius = 0.0;
        int most_possible = -1;
        for (unsigned int i = 0; i < contours.size(); i++ )
        {
            cv::Mat tmp_roi = src(cv::Rect(boundRect[i].x, boundRect[i].y, boundRect[i].width, boundRect[i].height));
            if(Judge_color3_legal(tmp_roi) == 0)
            {
                continue;
            }
            else
            {
                cv::Mat img1 = src_img.clone();
                double thre_color3_cnt = 0.6;

                double color3_possi = Count_possible(color3_project_result, boundRect[i]);
                printf("color3_possi:%lf\n", color3_possi);

                if(color3_possi <= thre_color3_cnt)
                {
                    continue;
                }
                else if(radius[i] > max_radius)
                {
                    most_possible = i;
                    max_radius = radius[i];//找出最大的半径，也就是最可能的值
                }
            }
        }//找最大半径
//////////////////////////////////////

        if (most_possible != -1)
        {
            color3_center_rows = center[most_possible].y;
            color3_center_cols = center[most_possible].x;
            color3_rect        = boundRect[most_possible];

        }
        else
        {
            color3_setNotFound();
        }
    }
    else
    {
        color3_setNotFound();
    }
}

int FiraObstacleVision::Judge_color3_legal(cv::Mat& src)
{
    if (src.rows <= 5 || src.cols <= 5)
        return 0;
    else
        return 1;
}

cv::Mat FiraObstacleVision::color3_BackProject(const cv::MatND& hist, const cv::Mat& findimage)
{

    cv::Mat result;//保存生成的反向投影图像
    cv::calcBackProject(&findimage,
                        1,//一副图像
                        channels,//通道数量
                        hist,//进行反向投影的直方图
                        result,//生成的反投影图像
                        ranges,//每个维度的值域
                        255.0//缩放因子
                       );

    double threshold = 0.03;//大于此概率的像素点全部变为255
    cv::threshold(result, result, 255 * threshold, 255, cv::THRESH_BINARY); //阈值化变为二值图像
    //应为之前缩放了255倍所以这边阈值threshold也要乘以255倍
    Erode_img(result, result, 2);
    Dilate_img(result, result, 4);
    return result;
}

void FiraObstacleVision::color3_setNotFound()
{
    color3_center_rows = color3_center_cols = -1;
    color3_rect.x = color3_rect.y = -1;
    color3_valid = false;
}


int FiraObstacleVision::Judge_centerInRect(Point2f center, Rect rect)
{
    if (center.x >= rect.x && center.x <= rect.x + rect.width && center.y >= rect.y && center.y <= rect.y + rect.height) {
        return 1;
    } else {
        return 0;
    }
}

double FiraObstacleVision::Count_possible(Mat bianry_img, Rect roi_rect)
{
    double cnt = 0, width = roi_rect.width, height = roi_rect.height;
    double area = width * height;

    for(int r = roi_rect.tl().y; r < roi_rect.br().y; r++)
    {
        for(int c = roi_rect.tl().x; c < roi_rect.br().x; c++)
        {
            if(bianry_img.at<uchar>(r, c) == 255)
                cnt++;
        }
    }

    cnt = cnt / area;

    return cnt;
}

//new
void FiraObstacleVision::Load_parameter()
{
//read
//    ifstream ifile("/home/zenghao/HuroCup 2017/Prepare/FiraObstacleVision/test_img/2.txt");
    ifstream ifile("3.txt");
    if(!ifile){
        cerr<<"error"<<endl;
    }
    string lineword;

    int i = 0;
    while (ifile >> lineword) {
        cout << lineword << "\n";
        if(i == 0)
            sscanf(lineword.c_str(), "%d", &color1_h_min_thre);
        else if(i == 1)
            sscanf(lineword.c_str(), "%d", &color1_h_max_thre);
        else if(i == 2)
            sscanf(lineword.c_str(), "%d", &color1_s_thre);
        else if(i == 3)
            sscanf(lineword.c_str(), "%d", &color2_h_min_thre);
        else if(i == 4)
            sscanf(lineword.c_str(), "%d", &color2_h_max_thre);
        else if(i == 5)
            sscanf(lineword.c_str(), "%d", &color2_s_thre); 
        else if(i == 6)
            sscanf(lineword.c_str(), "%d", &color3_h_min_thre);
        else if(i == 7)
            sscanf(lineword.c_str(), "%d", &color3_h_max_thre);
        else if(i == 8)
            sscanf(lineword.c_str(), "%d", &color3_s_thre);
        i++;
    }

}

void FiraObstacleVision::Store_parameter()
{
//write

//    ofstream out("D:/Robot/HuroCup 2017/Prepare/FiraObstacleVision/test_img/2.txt");
    ofstream out("3.txt");
    if(out.is_open()){
        printf("is_open\n");
        out << color1_h_min_thre    << " ";
        out << color1_h_max_thre    << " ";
        out << color1_s_thre        << " ";
        out << color2_h_min_thre << " ";
        out << color2_h_max_thre << " ";
        out << color2_s_thre     << " ";
        out << color3_h_min_thre << " ";
        out << color3_h_max_thre << " ";
        out << color3_s_thre     << "\n";
        printf("color2_s_thre:%d\n", color2_s_thre);
        out.close();
    }
    else
        printf("not open\n");
}

void FiraObstacleVision::Set_parameter()
{
    namedWindow("color1_combine", 1);
    namedWindow("hsv_combine", 1);
    namedWindow("color2_combine", 1);
    namedWindow("color3_combine", 1);

    createTrackbar("color1_h_min",  "color1_combine", &color1_h_min_thre, 255, on_trackbar);
    createTrackbar("color1_h_max",  "color1_combine", &color1_h_max_thre, 255, on_trackbar);
    createTrackbar("color1_s_thre", "color1_combine", &color1_s_thre, 255, on_trackbar);

    createTrackbar("color2_h_min",  "color2_combine", &color2_h_min_thre, 255, on_trackbar);
    createTrackbar("color2_h_max",  "color2_combine", &color2_h_max_thre, 255, on_trackbar);
    createTrackbar("color2_s_thre", "color2_combine", &color2_s_thre, 255, on_trackbar);

    createTrackbar("color3_h_min",  "color3_combine", &color3_h_min_thre, 255, on_trackbar);
    createTrackbar("color3_h_max",  "color3_combine", &color3_h_max_thre, 255, on_trackbar);
    createTrackbar("color3_s_thre", "color3_combine", &color3_s_thre, 255, on_trackbar);

    on_trackbar(color1_h_min_thre, 0);
}
