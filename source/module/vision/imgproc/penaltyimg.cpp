#include "penaltyimg.h"
/*
int hole_h_min_thre;
int hole_h_max_thre;
int hole_s_thre;

int ball_h_min_thre;
int ball_h_max_thre;
int ball_s_thre;

Mat hole_s_threimg;
Mat hole_h_threimg;
Mat hole_and_img;

Mat ball_s_threimg;
Mat ball_h_threimg;
Mat ball_and_img;

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

//hole circle
    threshold(trackbar_hsv_channel[1], hole_s_threimg, hole_s_thre, 255, cv::THRESH_BINARY);

    hole_h_threimg = trackbar_hsv_channel[0].clone();
    for(int i = 0; i < trackbar_hsv_channel[0].rows; i++){
        for(int j = 0; j < trackbar_hsv_channel[0].cols; j++){
            int value = trackbar_hsv_channel[0].at<uchar>(i,j);
            if(value >= hole_h_min_thre && value <= hole_h_max_thre){
                hole_h_threimg.at<uchar>(i,j) = 255;
            }else{
                hole_h_threimg.at<uchar>(i,j) = 0;
            }
        }
    }
    //erode and dilate
    int size = 2;
    cv::Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(hole_h_threimg, hole_h_threimg, element, Point(-1, -1), 1);
    dilate(hole_h_threimg, hole_h_threimg, element, Point(-1, -1), 1);

    bitwise_and(hole_h_threimg, hole_s_threimg, hole_and_img);

    //show
    Mat hole_combine = Mat(cv::Size(3*trackbar_hsv_channel[0].cols, trackbar_hsv_channel[0].rows), CV_8U);
    cv::Mat combine_rect_and   = hole_combine.colRange(0, col_1);
    cv::Mat combine_rect_hthre = hole_combine.colRange(col_1, col_2);
    cv::Mat combine_rect_sthre = hole_combine.colRange(col_2, col_3);

    hole_and_img.copyTo(combine_rect_and);
    hole_h_threimg.copyTo(combine_rect_hthre);
    hole_s_threimg.copyTo(combine_rect_sthre);

    hole_combine.col(col_1).setTo(255);
    hole_combine.col(col_2).setTo(255);

    imshow("hole_combine", hole_combine);

//ball circle
    threshold(trackbar_hsv_channel[1], ball_s_threimg, ball_s_thre, 255, cv::THRESH_BINARY);

    ball_h_threimg = trackbar_hsv_channel[0].clone();
    for(int i = 0; i < trackbar_hsv_channel[0].rows; i++){
        for(int j = 0; j < trackbar_hsv_channel[0].cols; j++){
            int value = trackbar_hsv_channel[0].at<uchar>(i,j);
            if(value >= ball_h_min_thre && value <= ball_h_max_thre){
                ball_h_threimg.at<uchar>(i,j) = 255;
            }else{
                ball_h_threimg.at<uchar>(i,j) = 0;
            }
        }
    }
    //erode and dilate
    size = 3;
    element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(ball_h_threimg, ball_h_threimg, element, Point(-1, -1), 1);
    dilate(ball_h_threimg, ball_h_threimg, element, Point(-1, -1), 1);

    bitwise_and(ball_h_threimg, ball_s_threimg, ball_and_img);

    //show
    Mat ball_combine = Mat(cv::Size(3*trackbar_hsv_channel[0].cols, trackbar_hsv_channel[0].rows), CV_8U);
    combine_rect_and   = ball_combine.colRange(0, col_1);
    combine_rect_hthre = ball_combine.colRange(col_1, col_2);
    combine_rect_sthre = ball_combine.colRange(col_2, col_3);

    ball_and_img.copyTo(combine_rect_and);
    ball_h_threimg.copyTo(combine_rect_hthre);
    ball_s_threimg.copyTo(combine_rect_sthre);

    ball_combine.col(col_1).setTo(255);
    ball_combine.col(col_2).setTo(255);

    imshow("ball_combine", ball_combine);
}
*/

void PenaltyImg::imageProcess(cv::Mat image, ImgProcResult *Result)
//void PenaltyImg::imageProcess(Mat image)
{
    ball_setNotFound();
    hole_setNotFound();

    Load_std_img();
    Pretreat(image);

    Process_ball(image);
    Process_hole(image);

    Set_parameter();

    /*******************************************/
    if (ball_rect.x != -1) {
        rectangle(center_img, ball_rect, cv::Scalar(255, 0, 0), 2, 8);

        ball_center_rows = ball_rect.y+ball_rect.height*0.5;
        ball_center_cols = ball_rect.x+ball_rect.width*0.5;
        ball_valid = true;
    }else{
        ball_valid = false;
        printf("ball not exist\n");
    }

    if (hole_rect.x != -1) {
        rectangle(center_img, hole_rect, cv::Scalar(0, 255, 0), 2, 8);

        hole_center_rows = hole_rect.y+hole_rect.height*0.5;
        hole_center_cols = hole_rect.x+hole_rect.width*0.5;
        hole_valid = true;
    }else{
        hole_valid = false;
        printf("hole not exist\n");
    }

    /*******************************************/

    Write_img(src_img, "src_img", start_file_num);
    Write_img(center_img, "center_img", start_file_num);
//    Write_img(hole_project_result, "holeproject_img", start_file_num);
//    Write_img(ball_project_result, "ballproject_img", start_file_num);

    start_file_num++;
    printf("num:::%d\n", start_file_num);

//////////////////////////////////////////

    MyResult.ball_center.x = ball_center_cols;
    MyResult.ball_center.y = ball_center_rows;
    MyResult.ball_valid = ball_valid;

    MyResult.hole_center.x = hole_center_cols;
    MyResult.hole_center.y = hole_center_rows;
    MyResult.hole_valid = hole_valid;
    (*dynamic_cast<PenaltyImgResult*>(Result)) = MyResult;
}

void PenaltyImg::Load_std_img()
{
//    std_hole = imread("../source/data/std_img/y.png"); //yellow
//    std_ball = imread("../source/data/std_img/r1.png");//red
// /home/robotis/Desktop/2017 Fira compete(zhang)/source/data/red2.jpg
///home/robotis/Desktop/2017 Fira compete(zhang)/source/data/yellowbox.jpg

    std_hole = imread("/home/robotis/Desktop/2017 Fira compete(zhang)/source/data/red2.jpg"); //
    std_ball = imread("/home/robotis/Desktop/2017 Fira compete(zhang)/source/data/yellowbox.jpg");//yellow

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

void PenaltyImg::Pretreat(Mat image)
{
    src_img = image.clone();
    center_img = src_img.clone();
    circles_img = src_img.clone();
    cvtColor( src_img, gray_img, CV_BGR2GRAY );//高阶的canny用法，转成灰度图，降噪，用canny
    cv::cvtColor(image, hsv_img, CV_RGB2HSV);
    cv::split(hsv_img, channel);
//    cv::split(hsv_img, trackbar_hsv_channel);

    GaussianBlur( gray_img, blur_img, Size(9, 9), 2, 2 );
    Canny(hsv_img, canny_img, canny_thre, canny_thre_max);
}

void PenaltyImg::Dilate_img(Mat src, Mat &dst, int size)
{
    Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    dilate(src, dst, element, Point(-1, -1), 1);
}

void PenaltyImg::Erode_img(cv::Mat src, cv::Mat &dst, int size)
{
    cv::Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(src, dst, element, Point(-1, -1), 1);
}

cv::MatND PenaltyImg::getHistogram(const cv::Mat &image)
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

cv::Mat PenaltyImg::getHistogram_Image(const cv::Mat &image)
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

void PenaltyImg::Write_img(cv::Mat& src,  const char floder_name[30], int num)
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
        //printf("written\n");
    }
}

void PenaltyImg::Process_hole(cv::Mat image)
{
    image = channel[0].clone();

    hole_project_result = hole_BackProject(std_hole_hist, image);

    for(int i = 0; i < channel[0].rows; i++){
        for(int j = 0; j < channel[0].cols; j++){
            int value = channel[0].at<uchar>(i,j);
            if(value >= hole_h_min_thre && value <= hole_h_max_thre){
                hole_project_result.at<uchar>(i,j) = 255;
            }else{
                hole_project_result.at<uchar>(i,j) = 0;
            }
        }
    }


    Mat thre_s;
    threshold(channel[1], thre_s, hole_s_thre, 255, CV_THRESH_BINARY);
    bitwise_and(thre_s, hole_project_result, hole_project_result);
      
    //imshow("hole_backproject", hole_project_result);

    Find_hole_center(hole_project_result);
}

void PenaltyImg::Find_hole_center(cv::Mat src)
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
            if (radius[i] > max_radius)
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
			printf("222here hole not exist\n");
        }
    }
    else
    {
        hole_setNotFound();
		printf("333here hole not exist\n");
    }
}

int PenaltyImg::Judge_hole_legal(cv::Mat& src)
{
    if (src.rows <= 15 || src.cols <= 5)
	{
        return 0;
		printf("111here hole not exist\n");
	}
    else
        return 1;
}

cv::Mat PenaltyImg::hole_BackProject(const cv::MatND& hist, const cv::Mat& findimage)
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
    Erode_img(result, result, 3);
    Dilate_img(result, result, 4);
    return result;
}

void PenaltyImg::hole_setNotFound()
{
    hole_center_rows = hole_center_cols = -1;
    hole_rect.x = hole_rect.y = -1;
    hole_valid = false;
}

void PenaltyImg::Process_ball(cv::Mat image)
{
    image = channel[0].clone();

    ball_project_result = ball_BackProject(std_ball_hist, image);

    for(int i = 0; i < channel[0].rows; i++){
        for(int j = 0; j < channel[0].cols; j++){
            int value = channel[0].at<uchar>(i,j);
            if(value >= ball_h_min_thre && value <= ball_h_max_thre){
                ball_project_result.at<uchar>(i,j) = 255;
            }else{
                ball_project_result.at<uchar>(i,j) = 0;
            }
        }
    }
    Mat thre_s;
    threshold(channel[1], thre_s, ball_s_thre, 255, CV_THRESH_BINARY);
    bitwise_and(thre_s, ball_project_result, ball_project_result);
   Erode_img(ball_project_result, ball_project_result, 3);
    Dilate_img(ball_project_result, ball_project_result, 6);
//    imshow("ball_backproject", ball_project_result);

    Find_ball_center(ball_project_result);
}

void PenaltyImg::Find_ball_center(cv::Mat src)
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
                double thre_ball_cnt = 0.5;

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

int PenaltyImg::Judge_ball_legal(cv::Mat& src)
{
    if (src.rows >= 150 || src.cols >= 150 || src.rows <= 5 || src.cols <= 5)
        return 0;
    else
        return 1;
}

cv::Mat PenaltyImg::ball_BackProject(const cv::MatND& hist, const cv::Mat& findimage)
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

void PenaltyImg::ball_setNotFound()
{
    ball_center_rows = ball_center_cols = -1;
    ball_rect.x = ball_rect.y = -1;
    ball_valid = false;
}

double PenaltyImg::Count_possible(Mat bianry_img, Rect roi_rect)
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
void PenaltyImg::Load_parameter()
{
//read
//    ifstream ifile("/home/zenghao/HuroCup 2017/Prepare/PenaltyImg/test_img/2.txt");
    ifstream ifile("../source/data/set_penalty_parameter/3.txt");
    if(!ifile){
        cerr<<"error"<<endl;
    }
    string lineword;

    int i = 0;
    while (ifile >> lineword) {
        cout << lineword << "\n";
        if(i == 0){
            sscanf(lineword.c_str(), "%d", &hole_h_min_thre);
        }else if(i == 1){
            sscanf(lineword.c_str(), "%d", &hole_h_max_thre);
        }else if(i == 2){
            sscanf(lineword.c_str(), "%d", &hole_s_thre);
        }else if(i == 3){
            sscanf(lineword.c_str(), "%d", &ball_h_min_thre);
        }else if(i == 4){
            sscanf(lineword.c_str(), "%d", &ball_h_max_thre);
        }else if(i == 5){
            sscanf(lineword.c_str(), "%d", &ball_s_thre);
        }
        i++;
    }

}

void PenaltyImg::Store_parameter()
{
//write

//    ofstream out("D:/Robot/HuroCup 2017/Prepare/PenaltyImg/test_img/2.txt");
    ofstream out("../source/data/set_penalty_parameter/3.txt");
    if(out.is_open()){
        printf("is_open\n");
        out << hole_h_min_thre    << " ";
        out << hole_h_max_thre    << " ";
        out << hole_s_thre        << " ";
        out << ball_h_min_thre << " ";
        out << ball_h_max_thre << " ";
        out << ball_s_thre     << "\n";
        printf("ball_s_thre:%d\n", ball_s_thre);
        out.close();
    }else{
        printf("not open\n");
    }
}

void PenaltyImg::Set_parameter()
{
//    namedWindow("hole_combine", 1);
//    namedWindow("hsv_combine", 1);
 //   namedWindow("ball_combine", 1);

 //   createTrackbar("hole_h_min",  "hole_combine", &hole_h_min_thre, 255, on_trackbar);
//    createTrackbar("hole_h_max",  "hole_combine", &hole_h_max_thre, 255, on_trackbar);
 //   createTrackbar("hole_s_thre", "hole_combine", &hole_s_thre, 255, on_trackbar);

  //  createTrackbar("ball_h_min",  "ball_combine", &ball_h_min_thre, 255, on_trackbar);
 //   createTrackbar("ball_h_max",  "ball_combine", &ball_h_max_thre, 255, on_trackbar);
 //   createTrackbar("ball_s_thre", "ball_combine", &ball_s_thre, 255, on_trackbar);

 //   on_trackbar(hole_h_min_thre, 0);
}

