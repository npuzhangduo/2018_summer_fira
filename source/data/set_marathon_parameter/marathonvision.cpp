//**************************************************************************
//--------------------------------------------------------------------------
// Set Marathon Paramenter by HSV
//
// Hao Zeng
// version 1.0
// Date    2017.9.19
//--------------------------------------------------------------------------
//**************************************************************************
/**
 * @todo 要现场采样，更新草场标准图.
 *      1.确认构造函数std_img 路径是否更新
 *          std_img = cv::imread("../source/data/std_img/std_combine.jpg");
 *      2.确认头文件和cpp里面Imgprocess函数是否更改 void Marathon::imageProcess(cv::Mat image, ImgProcResult *Result)
 *      3.确认头文件中的类名是否更改class Marathon : public ImgProc
 *      4.确认类中是否增加成员变量MarathonResult MyResult;
 *      5.确认头文件中是否增加class MarathonResult : public ImgProcResult
 *      6.确认imgprocess函数中是否给Result类返回参数信息
 *
 *        MyResult.center.x = center_cols;
 *        MyResult.center.y = center_rows;
 *        MyResult.valid    = false;
 *        (*dynamic_cast<MarathonResult*>(Result)) = MyResult;
 *
 *      7.是否在imgprocess函数末尾增加 (*dynamic_cast<MarathonResult*>(Result)) = MyResult;
 *      8.是否更改了调试时图片的输出路径
 *       	char path[150] = {"../source/data/con_img/"};
 *      9.是否在主函数中使用了imshow，或者waitKey(0);
 *      10.是否在头文件中添加头文件#include "imgproc.h"
 *      11.是否更改参数文件路径ifstream ifile("3.txt");
 */

#include "marathonvision.h"

int white_gray_thre;
int white_erode_size;
int white_dilate_size;

Mat white_gray_threimg;
Mat grayimg;

void on_trackbar( int, void* )
{
//universal setting
    int col_1 = grayimg.cols;
    int col_2 = 2*col_1;

//gray threimg

    threshold(grayimg, white_gray_threimg, white_gray_thre, 255, CV_THRESH_BINARY);

    cv::Mat erode_element = getStructuringElement(MORPH_RECT, Size(2 * white_erode_size + 1, 2 * white_erode_size + 1), Point(white_erode_size, white_erode_size) );
    erode(white_gray_threimg, white_gray_threimg, erode_element, Point(-1, -1), 1);

    cv::Mat dilate_element = getStructuringElement(MORPH_RECT, Size(2 * white_dilate_size + 1, 2 * white_dilate_size + 1), Point(white_dilate_size, white_dilate_size) );
    dilate(white_gray_threimg, white_gray_threimg, dilate_element, Point(-1, -1), 1);

    //show
    Mat gray_combine = Mat(cv::Size(2*grayimg.cols, grayimg.rows), CV_8U);
    cv::Mat combine_left   = gray_combine.colRange(0, col_1);
    cv::Mat combine_right  = gray_combine.colRange(col_1, col_2);

    grayimg.copyTo(combine_left);
    white_gray_threimg.copyTo(combine_right);

    gray_combine.col(col_1).setTo(255);

    imshow("gray_combine", gray_combine);
}


/******Public******/
//void Marathon::imageProcess(cv::Mat image, ImgProcResult *Result)
void Marathon::imageProcess(cv::Mat image)
{
    ball_setNotFound();
    hole_setNotFound();
    line_setNotFound();

//    Load_std_img();
    Pretreat(image);

    Process_ball(image);
    Process_hole(image);
    Process_line(image);

    Set_parameter();

    /*******************************************/
    if (line_rect.x != -1) {
        rectangle(center_img, line_rect, cv::Scalar(255, 0, 0), 2, 8);

        line_center_rows = line_rect.y+line_rect.height*0.5;
        line_center_cols = line_rect.x+line_rect.width*0.5;
        line_valid = true;

        circle(center_img, cv::Point2f(line_center_cols, line_center_rows), 2, cv::Scalar(255, 0, 255), 6, 8, 0);
    }else{
        line_valid = false;
        printf("ball not exist\n");
    }


    /*******************************************/

    Write_img(src_img, "src_img", start_file_num);
    Write_img(center_img, "center_img", start_file_num);
    Write_img(hole_project_result, "holeproject_img", start_file_num);
    Write_img(ball_project_result, "ballproject_img", start_file_num);

    start_file_num++;

//////////////////////////////////////////

//    MyResult.ball_center.x = ball_center_cols;
//    MyResult.ball_center.y = ball_center_rows;
//    MyResult.ball_valid = ball_valid;

//    MyResult.hole_center.x = hole_center_cols;
//    MyResult.hole_center.y = hole_center_rows;
//    MyResult.hole_valid = hole_valid;
//    (*dynamic_cast<MarathonResult*>(Result)) = MyResult;
}

void Marathon::Load_std_img()
{
//    std_hole    = imread("D:/Robot/HuroCup 2017/Prepare/Marathon/std_img/1/y.png"); //yellow
//    std_ball = imread("D:/Robot/HuroCup 2017/Prepare/Marathon/std_img/1/r1.png");//red

    // std_hole= cv::imread("/home/zenghao/HuroCup 2017/Prepare/Marathon/std_img/1/y1.png");
//    std_hole    = imread("/home/zenghao/HuroCup 2017/Prepare/Marathon/std_img/1/y.png"); //yellow
//    std_ball = imread("/home/zenghao/HuroCup 2017/Prepare/Marathon/std_img/1/r1.png");//red
    std_hole    = imread("y.png"); //yellow
    std_ball = imread("r1.png");//red

    cv::cvtColor(std_hole, std_hole, CV_RGB2HSV);
    cv::cvtColor(std_ball, std_ball, CV_RGB2HSV);

    cv::split(std_hole, std_hole_hsv_channel);
    cv::split(std_ball, std_ball_hsv_channel);

//    imshow("std_hole_channel", std_hole_hsv_channel[0]);
//    imshow("std_ball_channel", std_ball_hsv_channel[0]);

    std_hole_hist    = getHistogram(std_hole_hsv_channel[0]);
    std_ball_hist = getHistogram(std_ball_hsv_channel[0]);

    cv::normalize(std_hole_hist, std_hole, 1.0);
    cv::normalize(std_ball_hist, std_ball_hist, 1.0);

}

void Marathon::Pretreat(cv::Mat image)
{
    //basket
    src_img = image.clone();
    center_img = src_img.clone();

    cvtColor( src_img, gray_img, CV_BGR2GRAY );//高阶的canny用法，转成灰度图，降噪，用canny
    grayimg = gray_img;

//    cv::cvtColor(image, hsv_img, CV_RGB2HSV);
//    cv::split(hsv_img, channel);
//    cv::split(hsv_img, trackbar_hsv_channel);

//    GaussianBlur( gray_img, blur_img, Size(9, 9), 2, 2 );
//    Canny(hsv_img, canny_img, canny_thre, canny_thre_max);
}

void Marathon::Dilate_img(Mat src, Mat &dst, int size)
{
    Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    dilate(src, dst, element, Point(-1, -1), 1);
}

void Marathon::Erode_img(cv::Mat src, cv::Mat &dst, int size)
{
    cv::Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(src, dst, element, Point(-1, -1), 1);
}

cv::MatND Marathon::getHistogram(const cv::Mat &image)
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

cv::Mat Marathon::getHistogram_Image(const cv::Mat &image)
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

void Marathon::Write_img(cv::Mat& src,  const char floder_name[30], int num)
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

/******hole Circle******/

void Marathon::Process_hole(cv::Mat image)
{
//    image = channel[0].clone();

//    hole_project_result = hole_BackProject(std_hole_hist, image);

//    for(int i = 0; i < channel[0].rows; i++){
//        for(int j = 0; j < channel[0].cols; j++){
//            int value = channel[0].at<uchar>(i,j);
//            if(value >= hole_h_min_thre && value <= hole_h_max_thre){
//                hole_project_result.at<uchar>(i,j) = 255;
//            }else{
//                hole_project_result.at<uchar>(i,j) = 0;
//            }
//        }
//    }

//    Mat thre_s;
//    threshold(channel[1], thre_s, hole_s_thre, 255, CV_THRESH_BINARY);
//    bitwise_and(thre_s, hole_project_result, hole_project_result);

//    imshow("hole_backproject", hole_project_result);

//    Find_hole_center(hole_project_result);
}

void Marathon::Find_hole_center(cv::Mat src)
{
    hole_center_rows = hole_center_cols = -1;
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
            if (radius[i] > max_radius && boundRect[i].width <= 0.9 * img.cols && boundRect[i].height <= 0.9 * img.rows )
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
            if (Judge_hole_legal(roi_img) == 0)
            {
                hole_setNotFound();
            }
            else
            {
                hole_center_rows = center[most_possible].y;
                hole_center_cols = center[most_possible].x;
                hole_rect        = boundRect[most_possible];
            }
        }
        else
        {
            hole_setNotFound();
        }
    }
    else
    {
        hole_setNotFound();
    }
}

int Marathon::Judge_hole_legal(cv::Mat& src)
{
    if (src.rows <= 15 || src.cols <= 15)
        return 0;
    else
        return 1;
}

cv::Mat Marathon::hole_BackProject(const cv::MatND& hist, const cv::Mat& findimage)
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

void Marathon::hole_setNotFound()
{
    hole_center_rows = hole_center_cols = -1;
    hole_rect.x = hole_rect.y = -1;
    hole_valid = false;
}

/******ball Circle******/

void Marathon::Process_ball(cv::Mat image)
{
//    image = channel[0].clone();

//    ball_project_result = ball_BackProject(std_ball_hist, image);

//    for(int i = 0; i < channel[0].rows; i++){
//        for(int j = 0; j < channel[0].cols; j++){
//            int value = channel[0].at<uchar>(i,j);
//            if(value >= ball_h_min_thre && value <= ball_h_max_thre){
//                ball_project_result.at<uchar>(i,j) = 255;
//            }else{
//                ball_project_result.at<uchar>(i,j) = 0;
//            }
//        }
//    }

//    Mat thre_s;
//    threshold(channel[1], thre_s, ball_s_thre, 255, CV_THRESH_BINARY);
//    bitwise_and(thre_s, ball_project_result, ball_project_result);

//    imshow("ball_backproject", ball_project_result);

//    Find_ball_center(ball_project_result);
}

void Marathon::Find_ball_center(cv::Mat src)
{
    ball_center_rows = ball_center_cols = -1;
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
            if(Judge_ball_legal(tmp_roi) == 0)
            {
                continue;
            }
            else
            {
                cv::Mat img1 = src_img.clone();
                double thre_ball_cnt = 0.7;

                double ball_possi = Count_possible(ball_project_result, boundRect[i]);
//                printf("ball_possi:%lf\n", ball_possi);

                if(ball_possi <= thre_ball_cnt)
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
            ball_center_rows = center[most_possible].y;
            ball_center_cols = center[most_possible].x;
            ball_rect        = boundRect[most_possible];

        }
        else
        {
            ball_setNotFound();
        }
    }
    else
    {
        ball_setNotFound();
    }
}

int Marathon::Judge_ball_legal(cv::Mat& src)
{
    if (src.rows >= 50 || src.cols >= 50 || src.rows <= 5 || src.cols <= 5)
        return 0;
    else
        return 1;
}

cv::Mat Marathon::ball_BackProject(const cv::MatND& hist, const cv::Mat& findimage)
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

void Marathon::ball_setNotFound()
{
    ball_center_rows = ball_center_cols = -1;
    ball_rect.x = ball_rect.y = -1;
    ball_valid = false;
}

int Marathon::Judge_centerInRect(Point2f center, Rect rect)
{
    if (center.x >= rect.x && center.x <= rect.x + rect.width && center.y >= rect.y && center.y <= rect.y + rect.height) {
        return 1;
    } else {
        return 0;
    }
}

double Marathon::Count_possible(Mat bianry_img, Rect roi_rect)
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

/******Marathon Line******/
void Marathon::Process_line(cv::Mat image)
{
    threshold(gray_img, white_thre_result, white_gray_thre, 255, CV_THRESH_BINARY);

    Erode_img(white_thre_result, white_thre_result, white_erode_size);
    Dilate_img(white_thre_result, white_thre_result, white_dilate_size);

    Find_line_center(white_thre_result);
}

void Marathon::Find_line_center(cv::Mat src)
{
    line_setNotFound();

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
            if(Judge_line_legal(tmp_roi) == 0)
            {
                continue;
            }
            else if(radius[i] > max_radius)
            {
                most_possible = i;
                max_radius = radius[i];//找出最大的半径，也就是最可能的值
            }
        }//找最大半径


        if (most_possible != -1)
        {
            line_center_rows = center[most_possible].y;
            line_center_cols = center[most_possible].x;
            line_rect        = boundRect[most_possible];

            cv::Point2f mass_center = Find_mass_center(src, line_rect);
            circle(center_img, mass_center, 2, cv::Scalar(0, 0, 255), 6, 8, 0);
        }
        else
        {
            line_setNotFound();
        }
    }
    else
    {
        line_setNotFound();
    }
}

int Marathon::Judge_line_legal(cv::Mat& src)
{
    if (src.rows <= 30 || src.cols <= 20)
        return 0;
    else
        return 1;
}

cv::Point2f Marathon::Find_mass_center(Mat &bianry_img, Rect roi_rect)
{
    //整体的重心
    unsigned int row_sum = 0;
    unsigned int col_sum = 0;

    double cnt = 0, width = roi_rect.width, height = roi_rect.height;
    double area = width * height;

    //计算分段的重心
    int block_num = 4;  // 段数
    int block_height = (roi_rect.height/block_num)+1;  // 每一段的高度
    int block_row_sum[block_num];
    int block_col_sum[block_num];
    int block_cnt[block_num];

    int block_index = 0;

    memset(block_row_sum, 0, sizeof(block_row_sum));
    memset(block_col_sum, 0, sizeof(block_col_sum));
    memset(block_cnt, 0, sizeof(block_cnt));

    for(int r = roi_rect.tl().y; r < roi_rect.br().y; r++)
    {
        block_index = (r-roi_rect.tl().y+1)/block_height;
        for(int c = roi_rect.tl().x; c < roi_rect.br().x; c++)
        {
            if(bianry_img.at<uchar>(r, c) == 255)
            {
                row_sum += r;
                col_sum += c;

                block_row_sum[block_index] += r;
                block_col_sum[block_index] += c;

                block_cnt[block_index]++;
                cnt++;
            }
        }
    }

    cv::Point2f mass_center = cv::Point2f(1.0*col_sum/cnt, 1.0*row_sum/cnt);

    for(int i = 0; i < block_num; i++)
    {
        circle(center_img, cv::Point2f(1.0*block_col_sum[i]/block_cnt[i], 1.0*block_row_sum[i]/block_cnt[i]), 2, cv::Scalar(0, 0, 0), 2, 8, 0);
    }

    return mass_center;
}


void Marathon::line_setNotFound()
{
    line_center_rows = line_center_cols = -1;
    line_rect.x = line_rect.y = -1;
    line_valid = false;
}

/*********Parameter*********/
void Marathon::Load_parameter()
{
//read
//    ifstream ifile("/home/zenghao/HuroCup 2017/Prepare/Marathon/test_img/2.txt");
    ifstream ifile("3.txt");
    if(!ifile){
        cerr<<"error"<<endl;
    }
    string lineword;

    int i = 0;
    while (ifile >> lineword) {
        cout << lineword << "\n";
        if(i == 0){
            sscanf(lineword.c_str(), "%d", &white_gray_thre);
        }else if(i == 1){
            sscanf(lineword.c_str(), "%d", &white_erode_size);
        }else if(i == 2){
            sscanf(lineword.c_str(), "%d", &white_dilate_size );
        }
        i++;
    }

}

void Marathon::Store_parameter()
{
//write

//    ofstream out("D:/Robot/HuroCup 2017/Prepare/Marathon/test_img/2.txt");
    ofstream out("3.txt");
    if(out.is_open()){
        printf("is_open\n");
        out << white_gray_thre    << " ";
        out << white_erode_size   << " ";
        out << white_dilate_size  << "\n";
        printf("white_dilate_size:%d\n", white_dilate_size);
        out.close();
    }else{
        printf("not open\n");
    }
}

void Marathon::Set_parameter()
{
    namedWindow("gray_combine");

    createTrackbar("white_gray_thre",   "gray_combine", &white_gray_thre,   255, on_trackbar);
    createTrackbar("white_erode_size",  "gray_combine", &white_erode_size,  6,   on_trackbar);
    createTrackbar("white_dilate_size", "gray_combine", &white_dilate_size, 6,   on_trackbar);

    on_trackbar(white_gray_thre, 0);
}
