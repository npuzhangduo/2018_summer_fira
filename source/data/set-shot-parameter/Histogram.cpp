//**************************************************************************
//--------------------------------------------------------------------------
// Histogram Parameter Set
//
// Hao Zeng
// version 2.1
// Date    2017.7.14
//--------------------------------------------------------------------------
//**************************************************************************
/**
 * @todo 要现场采样，更新草场标准图.
 *      1.确认构造函数std_img 路径是否更新
 *          std_img = cv::imread("../source/data/std_img/std_combine.jpg");
 *      2.确认头文件和cpp里面Imgprocess函数是否更改 void Archery::imageProcess(cv::Mat image, ImgProcResult *Result)
 *      3.确认头文件中的类名是否更改class Archery : public ImgProc
 *      4.确认类中是否增加成员变量ArcheryResult MyResult;
 *      5.确认头文件中是否增加class ArcheryResult : public ImgProcResult
 *      6.确认imgprocess函数中是否给Result类返回参数信息
 *
 *        MyResult.center.x = center_cols;
 *        MyResult.center.y = center_rows;
 *        MyResult.valid    = false;
 *        (*dynamic_cast<ArcheryResult*>(Result)) = MyResult;
 *
 *      7.是否在imgprocess函数末尾增加 (*dynamic_cast<ArcheryResult*>(Result)) = MyResult;
 *      8.是否更改了调试时图片的输出路径
 *       	char path[150] = {"../source/data/con_img/"};
 *      9.是否在主函数中使用了imshow，或者waitKey(0);
 *      10.是否在头文件中添加头文件#include "imgproc.h"
 *      11.是否更改参数文件路径ifstream ifile("3.txt");
 */

#include "Histogram.h"

int basket_h_min_thre;
int basket_h_max_thre;
int basket_s_thre;

int ball_h_min_thre;
int ball_h_max_thre;
int ball_s_thre;

Mat basket_s_threimg;
Mat basket_h_threimg;
Mat basket_and_img;

Mat ball_s_threimg;
Mat ball_h_threimg;
Mat ball_and_img;

Mat trackbar_hsv_channel[3];

void on_trackbar( int, void* )
{
//combine hsv
    int col_1 = trackbar_hsv_channel[0].cols;
    int col_2 = 2*col_1;
    int col_3 = col_2 + col_1;

    Mat hsv_combine = Mat(cv::Size(2*trackbar_hsv_channel[0].cols, trackbar_hsv_channel[0].rows), CV_8U);
    cv::Mat combine_h   = hsv_combine.colRange(0, col_1);
    cv::Mat combine_s   = hsv_combine.colRange(col_1, col_2);

    trackbar_hsv_channel[0].copyTo(combine_h);
    trackbar_hsv_channel[1].copyTo(combine_s);

    hsv_combine.col(col_1).setTo(255);

    imshow("hsv_combine", hsv_combine);

//combine basket
    threshold(trackbar_hsv_channel[1], basket_s_threimg, basket_s_thre, 255, cv::THRESH_BINARY);

    basket_h_threimg = trackbar_hsv_channel[0].clone();
    for(int i = 0; i < trackbar_hsv_channel[0].rows; i++){
        for(int j = 0; j < trackbar_hsv_channel[0].cols; j++){
            int value = trackbar_hsv_channel[0].at<uchar>(i,j);
            if(value >= basket_h_min_thre && value <= basket_h_max_thre){
                basket_h_threimg.at<uchar>(i,j) = 255;
            }else{
                basket_h_threimg.at<uchar>(i,j) = 0;
            }
        }
    }

    int size = 2;
    cv::Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(basket_h_threimg, basket_h_threimg, element, Point(-1, -1), 1);

    size = 2;
    element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    dilate(basket_h_threimg, basket_h_threimg, element, Point(-1, -1), 1);

    bitwise_and(basket_h_threimg, basket_s_threimg, basket_and_img);


    Mat basket_combine = Mat(cv::Size(3*trackbar_hsv_channel[0].cols, trackbar_hsv_channel[0].rows), CV_8U);
    cv::Mat combine_rect_and   = basket_combine.colRange(0, col_1);
    cv::Mat combine_rect_hthre = basket_combine.colRange(col_1, col_2);
    cv::Mat combine_rect_sthre = basket_combine.colRange(col_2, col_3);

    basket_and_img.copyTo(combine_rect_and);
    basket_h_threimg.copyTo(combine_rect_hthre);
    basket_s_threimg.copyTo(combine_rect_sthre);




    basket_combine.col(col_1).setTo(255);
    basket_combine.col(col_2).setTo(255);

    imshow("basket_combine", basket_combine);

//combine ball
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

    bitwise_and(ball_h_threimg, ball_s_threimg, ball_and_img);


    Mat ball_combine = Mat(cv::Size(3*trackbar_hsv_channel[0].cols, trackbar_hsv_channel[0].rows), CV_8U);
    combine_rect_and   = ball_combine.colRange(0, col_1);
    combine_rect_hthre = ball_combine.colRange(col_1, col_2);
    combine_rect_sthre = ball_combine.colRange(col_2, col_3);

    ball_and_img.copyTo(combine_rect_and);
    ball_h_threimg.copyTo(combine_rect_hthre);
    ball_s_threimg.copyTo(combine_rect_sthre);

    size = 1;
    element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(ball_combine, ball_combine, element, Point(-1, -1), 1);

    size = 2;
    element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    dilate(ball_combine, ball_combine, element, Point(-1, -1), 1);

    ball_combine.col(col_1).setTo(255);
    ball_combine.col(col_2).setTo(255);

    imshow("ball_combine", ball_combine);

}

/******Public******/
//void Histogram::imageProcess(cv::Mat image, ImgProcResult *Result)
 void Histogram::imageProcess(cv::Mat image)
{
     Not_Find_Basket();
     Not_Find_Ball();
    debug_print = 0;
    if (debug_print)
        printf("1\n");
    Load_std_img();
    if (debug_print)
        printf("2\n");
    Pretreat(image);
    Process_Basket(image);
//    Process_Ball(image);
    Process_Ball_2(image);

    Set_parameter();

//    if(Judge_center_close() == 1)
//    {
//        Not_Find_Basket();
//    }


    Write_img(src_img, "src_img", start_file_num);
    Write_img(center_img, "center_img", start_file_num);

    if (debug_print)printf("area:%lf\n", basket_area);
    if (debug_print)printf("length:%lf\n", basket_length);
//    if(debug_print)printf("basket_center_x:%lf basket_center_y:%lf valid %d\n", basket_center_cols, basket_center_rows,MyResult.basket_valid);
//    printf("2333\n");
    if (debug_print)printf("up:%lf down:%lf\n", basket_up_y, basket_down_y);
//    printf("3222\n");


    if (debug_print)
    {
        imshow("src_img", src_img);
        imshow("center", center_img);
        imshow("circle_img", circles_img);
        imshow("canny_img", canny_img);
        imshow("basket_peoject", basket_project_result);
        waitKey(0);
    }

//    imshow("circle", center_img);
//    waitKey(0);
    start_file_num++;

//    MyResult.ball_center.x = ball_center_cols;
//    MyResult.ball_center.y = ball_center_rows;
//    MyResult.basket_center.x = basket_center_cols;
//    MyResult.basket_center.y = basket_center_rows;
//    MyResult.basket_center_up = basket_up_y;
//    MyResult.basket_center_down = basket_down_y;
//    MyResult.basket_area = basket_area;
//    MyResult.basket_length = basket_length;

//    if (ball_center_cols == -1 && ball_center_rows == -1) {
//        MyResult.ball_valid = false;
//    }
//    else {
//        MyResult.ball_valid = true;
//    }

//    if (basket_center_cols == -1 && basket_center_rows == -1) {
//        MyResult.basket_valid = false;
//    }
//    else {
//        MyResult.basket_valid = true;
//    }
//    (*dynamic_cast<HistogramResult*>(Result)) = MyResult;

}

void Histogram::Load_std_img()
{
//    std_basket = cv::imread("../source/data/std_img/combine_box.jpg");
//    cv::Mat src_std_img_1 = imread("../source/data/std_img/combine_1.jpg");
//    cv::Mat src_std_img_2 = imread("../source/data/std_img/light_green_std_roi.jpg");

     std_basket= cv::imread("combine_box.png");
     cv::Mat src_std_img_1 = imread("combine_1.jpg");
     cv::Mat src_std_img_2 = imread("light_green_std_roi.jpg");

    cv::cvtColor(std_basket, std_basket, CV_RGB2HSV);
    cv::cvtColor(src_std_img_1, std_ball_1, CV_RGB2HSV);
    cv::cvtColor(src_std_img_2, std_ball_2, CV_RGB2HSV);

    cv::split(std_basket, std_basket_hsv_channel);
    cv::split(std_ball_1, std_ball_1_channel);
    cv::split(std_ball_2, std_ball_2_channel);
//    imshow("1", std_basket_hsv_channel[0]);
    std_basket_hist = getHistogram(std_basket_hsv_channel[0]);
    std_ball_1_hist = getHistogram(std_ball_1_channel[0]);
    std_ball_2_hist = getHistogram(std_ball_2_channel[0]);

    cv::normalize(std_basket_hist, std_basket, 1.0);
    cv::normalize(std_ball_1_hist, std_ball_1_hist, 1.0);
    cv::normalize(std_ball_2_hist, std_ball_2_hist, 1.0);
}

void Histogram::Pretreat(cv::Mat image)
{
    //basket
    src_img = image.clone();
    center_img = src_img.clone();
    circles_img = src_img.clone();
    cvtColor( src_img, gray_img, CV_BGR2GRAY );
    cv::cvtColor(image, hsv_img, CV_RGB2HSV);
    cv::split(hsv_img, channel);

    cv::split(hsv_img, trackbar_hsv_channel);

    GaussianBlur( gray_img, blur_img, Size(9, 9), 2, 2 );
    Canny(hsv_img, canny_img, canny_thre, canny_thre_max);
}

void Histogram::Dilate_img(Mat src, Mat &dst, int size)
{
    Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    dilate(src, dst, element, Point(-1, -1), 1);
}

void Histogram::Erode_img(cv::Mat src, cv::Mat &dst, int size)
{
    cv::Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(src, dst, element, Point(-1, -1), 1);
}

cv::MatND Histogram::getHistogram(const cv::Mat &image)
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

cv::Mat Histogram::getHistogram_Image(const cv::Mat &image)
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

void Histogram::Write_img(cv::Mat& src,  const char floder_name[30], int num)
{
    if (start_file_num <= max_file_num)
    {
//        char path[150] = {"../source/data/con_img/"};
        char path[150] = {"/home/zeng/Robot/red2/con_img/"};

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

int Histogram::Judge_center_close()
{
    if (ball_center_cols == -1 || ball_center_rows == -1 || basket_center_cols == -1 || basket_center_rows == -1)
    {
        //printf("-1\n");
        return 0;//中心不重叠
    }
    else
    {
        double x = ball_center_cols - basket_center_cols;
        double y = ball_center_rows - basket_center_rows;
        double dist = sqrt(x * x + y * y);
        if (dist <= ball_radius)
        {
            // printf("chong die\n");
            return 1;
        }
        else
        {
//printf("bu chong die\n");
            return 0;
        }
    }
}

/******Basket******/

void Histogram::Process_Basket(cv::Mat image)
{
    image = channel[0].clone();

    basket_project_result = Basket_BackProject(std_basket_hist, image);

    for(int i = 0; i < channel[0].rows; i++){
        for(int j = 0; j < channel[0].cols; j++){
            int value = channel[0].at<uchar>(i,j);
            if(value >= basket_h_min_thre && value <= basket_h_max_thre){
                basket_project_result.at<uchar>(i,j) = 255;
            }else{
                basket_project_result.at<uchar>(i,j) = 0;
            }
        }
    }

    Mat thre_s;
    threshold(channel[1], thre_s, basket_s_thre, 255, CV_THRESH_BINARY);
    bitwise_and(thre_s, basket_project_result, basket_project_result);

    imshow("big_backproject", basket_project_result);

    Find_Basket_center(basket_project_result);

}

void Histogram::Find_Basket_center(cv::Mat src)
{
    cv::Mat img = src.clone();

    std::vector<vector<Point> > contours;
    std::vector<vector<Point> > contours_poly;
    std::vector<RotatedRect> minRect;

    Scalar color = Scalar( 125, 16, 210 );   //定义画的边框颜色
    Scalar color2 = Scalar(210, 16, 125);
    Scalar color3 = Scalar(16, 210, 125);
//    Scalar color4 = Scalar(0, 0, 255);   //定义画的边框颜色

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

//    printf("contours.size:%d\n", contours.size());

    for (unsigned i = 0; i < contours.size(); i++ )
    {
        circle( drawing_approx, center[i], (int)radius[i], color3, 2, 8, 0 ); //画圆形
    }
//    imshow("drawing", drawing_approx);
    if (contours.size() > 0)
    {
        float max_radius = 0.0;
        int most_possible = -1;
        for (unsigned int i = 0; i < contours.size(); i++ )
        {
            //            if(radius[i] > max_radius && boundRect[i].width <= 0.8*img.cols && boundRect[i].height <= 0.8*img.rows && ((1.0*boundRect[i].width/boundRect[i].height <= 2.5) && (1.0*boundRect[i].height/boundRect[i].width <= 2.5)) && boundRect[i].x >= 0.1*img.cols && (boundRect[i].x + boundRect[i].width) <= 0.9*img.cols)
            if (radius[i] > max_radius )
            {
                cv::Mat tmp_roi = src(cv::Rect(boundRect[i].x, boundRect[i].y, boundRect[i].width, boundRect[i].height));
//                double now_similar = Calc_Similar(tmp_roi);
                if ( Judge_Basket_legal(tmp_roi) == 1)
                {
                    most_possible = i;
                    max_radius = radius[i];//找出最大的半径，也就是最可能的值
                }
            }
        }//找最大半径
//        printf("most_possible:%d\n", most_possible);
//        cv::Mat tmp233 = src_img.clone();
//        cv::rectangle(tmp233, boundRect[most_possible], cv::Scalar(255, 0, 255), 2, 8, 0);
//        imshow("233", tmp233);
//        waitKey(0);
        if (most_possible != -1)
        {
            circle( drawing_approx, center[most_possible], (int)radius[most_possible], color2, 2, 8, 0 ); //画圆
//                            imshow("darwing", drawing_approx);
            cv::Mat roi_img = src(cv::Rect(boundRect[most_possible].x, boundRect[most_possible].y, boundRect[most_possible].width, boundRect[most_possible].height));
            if (Judge_Basket_legal(roi_img) == 0)
            {
                Not_Find_Basket();
                if (debug_print)printf("size too small\n");
//                printf("roi_rows:%d roi_cols:%d\n", roi_img.rows, roi_img.cols);
            }
            else
            {
                cv::Mat tmp = roi_img.clone();
                basket_center_rows = center[most_possible].y;
                basket_center_cols = center[most_possible].x;

                basket_up_y = boundRect[most_possible].y;
                basket_down_y = boundRect[most_possible].y + boundRect[most_possible].height;

                basket_area = boundRect[most_possible].width * boundRect[most_possible].height;
                basket_length = 2 * (boundRect[most_possible].width + boundRect[most_possible].height);

                circle(center_img, Point2f(boundRect[most_possible].x, basket_up_y), 5, Scalar(0, 255, 255), 2, 8, 0 );
                circle(center_img, Point2f(boundRect[most_possible].x, basket_down_y), 5, Scalar(0, 255, 255), 2, 8, 0 );
                circle(center_img, center[most_possible], 3, Scalar(255, 255, 0), 2, 8, 0 );


                if (debug_print)printf("area:%lf\n", basket_area);
                if (debug_print)printf("length:%lf\n", basket_length);

                //printf("most_similar:%lf\n", most_similar);
            }
        }
        else
        {
            //printf("most = -1\n");
            Not_Find_Basket();
        }
    }
    else
    {
        //printf("size <= 0\n");
        Not_Find_Basket();
    }
//    printf("rows:%d cols:%d\n", int(basket_center_rows) , int(basket_center_cols));
}

int Histogram::Judge_Basket_legal(cv::Mat& src)
{
    if (src.rows <= 7 || src.cols <= 7)
        return 0;
    else
        return 1;
}

double Histogram::Calc_Similar(cv::Mat& src)
{
    double max_possible = 0.8;
    double cnt = 0, total = src.rows * src.cols;
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            if (src.at<uchar>(i, j) == 255)
                cnt++;
        }
    }
    cnt /= total;
//    printf("possible:%lf\n", cnt);
//    imshow("possible", src);
//    waitKey(0);
    if (cnt <= max_possible)
    {
        if (debug_print)printf("no legal now_possible:%lf\n", cnt);
        return -1;
    }
    else
        return cnt;
}

cv::Mat Histogram::Basket_BackProject(const cv::MatND& hist, const cv::Mat& findimage)
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

    double threshold = 0.001;//大于此概率的像素点全部变为255
    cv::threshold(result, result, 255 * threshold, 255, cv::THRESH_BINARY); //阈值化变为二值图像
    //应为之前缩放了255倍所以这边阈值threshold也要乘以255倍
//    Erode_img(result, result, 4);
    Dilate_img(result, result, 6);
    return result;
}

void Histogram::Not_Find_Basket()
{
    basket_area = -1;
    basket_length = -1;

    basket_center_cols = -1;
    basket_center_rows = -1;

    basket_up_y = -1;
    basket_down_y = -1;
}

/******Ball******/

//void Histogram::imageProcess(cv::Mat image,ImgProcResult *Result)
void Histogram::Process_Ball(cv::Mat image)
{
    ball_center_rows = ball_center_cols  = -1;

    vector<Vec3f> circles;

    /// Apply the Hough Transform to find the circles
//    HoughCircles( gray_img, circles, CV_HOUGH_GRADIENT, 1, gray_img.rows/8, 70, 20, 5, 40);
//    HoughCircles( gray_img, circles, CV_HOUGH_GRADIENT, 1, 50, 50, 20, 5, 40);
    HoughCircles( canny_img, circles, CV_HOUGH_GRADIENT, 1, 50, 50, 20, 5, 40);

    int max_num = -1;
    float max_radius = 0;
    for ( size_t i = 0; i < circles.size(); i++ )
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        circle( circles_img, center, 3, Scalar(0, 255, 0), -1, 8, 0 );
        circle( circles_img, center, radius, Scalar(0, 0, 255), 3, 8, 0 );
        Write_img(circles_img, "circles_img", start_file_num);
        if (Judge_Ball_Legal(circles[i]) == 1)
        {
            if (max_radius < radius)
            {
                max_num = i;
                max_radius = radius;
            }
        }
    }
    if (max_num != -1)
    {
        Point center(cvRound(circles[max_num][0]), cvRound(circles[max_num][1]));
//        int radius = cvRound(circles[max_num][2]);
//        circle( center_img, center, 3, Scalar(0,255,0), -1, 8, 0 );
//        circle( center_img, center, radius, Scalar(255,0,255), 3, 8, 0 );
//        Write_img(center_img, "center_img", start_file_num);

        ball_center_rows = circles[max_num][1];
        ball_center_cols = circles[max_num][0];
        ball_radius = cvRound(circles[max_num][2]);
//        imshow("center", center_img);
    }
    else
    {
        ball_center_rows = ball_center_cols  = -1;
    }

//    MyResult.ball_center.x = ball_center_cols;
//    MyResult.ball_center.y = ball_center_rows;
//    if(ball_center_cols == -1 && ball_center_rows == -1){
//        MyResult.ball_valid = false;
//    }
//    else{
//        MyResult.ball_valid = true;
//    }
//    (*dynamic_cast<HistogramResult*>(Result)) = MyResult;
}

int Histogram::Judge_Ball_Legal(Vec3f circle)
{
    //circles[i][0] col/x, circles[i][1] row/y, circles[i][2] radius/r;
    float x = circle[0], y = circle[1], radius = circle[2];
    float half_width = sqrt(2) * radius / 2;
    if (radius <= 8)
        return 0;
    else
    {
        cv::Mat roi_img;
        float leftup_x, leftup_y, rightdown_x, rightdown_y;

        leftup_x = x - half_width;
        leftup_y = y - half_width;
        rightdown_x = x + half_width;
        rightdown_y = y + half_width;

        if (leftup_x < 0)
            leftup_x = 0;
        if (leftup_y < 0)
            leftup_y = 0;
        if (rightdown_x >= hsv_img.cols)
            rightdown_x = hsv_img.cols - 1;
        if (rightdown_y >= hsv_img.rows)
            rightdown_y = hsv_img.rows - 1;

        roi_img = hsv_img(cv::Rect(leftup_x, leftup_y, rightdown_x - leftup_x, rightdown_y - leftup_y));

        resize(roi_img, roi_img, cv::Size(200, 200));
        if (Judge_Ball_Color_Legal(roi_img) == 1)
        {
//            printf("color_yes\n");
            return 1;
        }
        else
        {
//            printf("color_no\n");
            return 0;
        }
    }
}

int Histogram::Judge_Ball_Color_Legal(cv::Mat& src)
{
    double max_possible = 0.85;
    double sum_1 = 0, sum_2 = 0;
    cv::Mat roi_hsv = src.clone();

    cv::Mat roi_1_project = Ball_BackProject(std_ball_1_hist, roi_hsv);
    cv::Mat roi_2_project = Ball_BackProject(std_ball_2_hist, roi_hsv);
    for (int i = 0; i < roi_hsv.rows; i++)
    {
        for (int j = 0; j < roi_hsv.cols; j++)
        {
            if (roi_1_project.at<uchar>(i, j) == 255)
            {
                sum_1++;
            }
            if (roi_2_project.at<uchar>(i, j) == 255)
            {
                sum_2++;
            }
        }
    }
    sum_1 /= (roi_hsv.rows * roi_hsv.cols);
    sum_2 /= (roi_hsv.rows * roi_hsv.cols);

    if (debug_print)
        if (debug_print)printf("sum1:%lf sum2:%lf\n", sum_1, sum_2);

//        if(sum_1 >= max_possible && sum_2 <= 0.5)
    if (sum_1 >= max_possible)
        return 1;
    else
        return 0;
}

cv::Mat Histogram::Ball_BackProject(const cv::MatND& hist, const cv::Mat& findimage)
{

    cv::Mat result;//保存生成的反向投影图像

    //反投影直方图的作用在与将输入图像中的每一个像素值变为已经计算好的归一化直方图中该像素对应的概率值
    //生成的是一副概率图，每个像素的颜色表示属于参考区域的概率，亮的地方概率低，暗的地方概率高
    //或者白色低概率，黑色高概率???
    //我怎么觉得是白色是高概率，黑色是低概率

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
    Erode_img(result, result, 1);
    Dilate_img(result, result, 2);
    return result;
}

void Histogram::Process_Ball_2(Mat image)
{
    image = channel[0].clone();

    ball_project_result = Ball_BackProject(std_ball_1_hist, image);

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

    Erode_img(ball_project_result, ball_project_result, 1);
    Dilate_img(ball_project_result, ball_project_result, 2);

    Find_Ball_center(ball_project_result);
}

void Histogram::Find_Ball_center(cv::Mat src)
{

    cv::Mat img = src.clone();

    std::vector<vector<Point> > contours;
    std::vector<vector<Point> > contours_poly;
    std::vector<RotatedRect> minRect;

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

    if (contours.size() > 0)
    {
        float max_radius = 0.0;
        int most_possible = -1;
        for (unsigned int i = 0; i < contours.size(); i++ )
        {
            cv::Mat tmp_roi = src(cv::Rect(boundRect[i].x, boundRect[i].y, boundRect[i].width, boundRect[i].height));
            if(Judge_Ball_Legal_2(tmp_roi) == 1)
            {
                if(radius[i] > max_radius)
                {
                    most_possible = i;
                    max_radius = radius[i];//找出最大的半径，也就是最可能的值
                }
            }
        }//找最大半径

        if (most_possible != -1)
        {
            ball_center_rows = center[most_possible].y;
            ball_center_cols = center[most_possible].x;
            ball_radius = radius[most_possible];

            circle( center_img, center[most_possible], 3, Scalar(0, 255, 0), -1, 8, 0 );
            circle( center_img, center[most_possible], radius[most_possible], Scalar(255, 0, 255), 3, 8, 0 );
        }
        else
        {
            Not_Find_Ball();
        }
    }
    else
    {
        Not_Find_Ball();
    }
}

int Histogram::Judge_Ball_Legal_2(cv::Mat &src)
{
    double div1 = 1.0*src.cols/src.rows;
    double div2 = 1.0*src.rows/src.cols;

    if(src.rows <= 7 || src.cols <= 7)
        return 0;
    else if(src.rows >= 150 || src.cols >= 150)
        return 0;
    else if(div1 >= 2 || div2 >= 2)
        return 0;
    else
        return 1;
}

void Histogram::Not_Find_Ball()
{
    ball_center_rows = ball_center_cols = ball_radius = -1.0;
}

//**************************************************************************
//--------------------------------------------------------------------------
// Set Parameter
//--------------------------------------------------------------------------
//**************************************************************************
void Histogram::Set_parameter()
{
    namedWindow("basket_combine", 0);
    namedWindow("ball_combine", 0);

    createTrackbar("basket_h_min", "basket_combine", &basket_h_min_thre, 255, on_trackbar);
    createTrackbar("basket_h_max", "basket_combine", &basket_h_max_thre, 255, on_trackbar);
    createTrackbar("basket_s_thre", "basket_combine", &basket_s_thre, 255, on_trackbar);

    createTrackbar("ball_h_min", "ball_combine", &ball_h_min_thre, 255, on_trackbar);
    createTrackbar("ball_h_max", "ball_combine", &ball_h_max_thre, 255, on_trackbar);
    createTrackbar("ball_s_thre", "ball_combine", &ball_s_thre, 255, on_trackbar);

    on_trackbar(basket_h_min_thre, 0);
}

void Histogram::Load_parameter()
{
//read
//    ifstream ifile("/home/zenghao/HuroCup 2017/Prepare/Archery/test_img/2.txt");
    ifstream ifile("3.txt");
    if(!ifile){
        cerr<<"erroe"<<endl;
    }
    string lineword;

    int i = 0;
    while (ifile >> lineword) {
        cout << lineword << "\n";
        if(i == 0){
            sscanf(lineword.c_str(), "%d", &basket_h_min_thre);
        }else if(i == 1){
            sscanf(lineword.c_str(), "%d", &basket_h_max_thre);
        }else if(i == 2){
            sscanf(lineword.c_str(), "%d", &basket_s_thre);
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

void Histogram::Store_parameter()
{
//write
//    ofstream out("D:/Robot/HuroCup 2017/Prepare/Archery/test_img/2.txt");
    ofstream out("3.txt");
    if(out.is_open()){
        printf("is_open\n");
        out << basket_h_min_thre << " ";
        out << basket_h_max_thre << " ";
        out << basket_s_thre << " ";
        out << ball_h_min_thre << " ";
        out << ball_h_max_thre << " ";
        out << ball_s_thre << "\n";
        printf("ball_s_thre:%d\n", ball_s_thre);
        out.close();
    }else{
        printf("not open\n");
    }
}

