/**** final version to sprint hsv***/
/**********     attention   *************/
/*** trackbar!!! cv::split(hsv_img, trackbar_hsv_channel);****/


#include "Ladderrun.h"
/*
int big_h_min_thre;
int big_h_max_thre;
int big_s_thre;

Mat big_s_threimg;
Mat big_h_threimg;
Mat big_and_img;

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

	resize(hsv_combine, hsv_combine, cv::Size(hsv_combine.cols/2, hsv_combine.rows/2));
    imshow("hsv_combine", hsv_combine);

//big circle
    threshold(trackbar_hsv_channel[1], big_s_threimg, big_s_thre, 255, cv::THRESH_BINARY);

    big_h_threimg = trackbar_hsv_channel[0].clone();
    for(int i = 0; i < trackbar_hsv_channel[0].rows; i++){
        for(int j = 0; j < trackbar_hsv_channel[0].cols; j++){
            int value = trackbar_hsv_channel[0].at<uchar>(i,j);
            if(value >= big_h_min_thre && value <= big_h_max_thre){
                big_h_threimg.at<uchar>(i,j) = 255;
            }else{
                big_h_threimg.at<uchar>(i,j) = 0;
            }
        }
    }
    //erode and dilate
    int size = 2;
    cv::Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(big_h_threimg, big_h_threimg, element, Point(-1, -1), 1);
    dilate(big_h_threimg, big_h_threimg, element, Point(-1, -1), 1);

    bitwise_and(big_h_threimg, big_s_threimg, big_and_img);

    //show
    Mat big_combine = Mat(cv::Size(3*trackbar_hsv_channel[0].cols, trackbar_hsv_channel[0].rows), CV_8U);
    cv::Mat combine_rect_and   = big_combine.colRange(0, col_1);
    cv::Mat combine_rect_hthre = big_combine.colRange(col_1, col_2);
    cv::Mat combine_rect_sthre = big_combine.colRange(col_2, col_3);

    big_and_img.copyTo(combine_rect_and);
    big_h_threimg.copyTo(combine_rect_hthre);
    big_s_threimg.copyTo(combine_rect_sthre);

    big_combine.col(col_1).setTo(255);
    big_combine.col(col_2).setTo(255);

	resize(big_combine, big_combine, cv::Size(big_combine.cols/2, big_combine.rows/2));
    imshow("big_combine", big_combine);
}
*/
//void Ladderrun::imageProcess(cv::Mat image)
void Ladderrun:: imageProcess(cv::Mat image, ImgProcResult *Result)
{
    set_NotFound();
    //Load_std_img();
    Pretreat(image);

    Process_big(image);
    Set_parameter();

    if (big_rect.x != -1) {
        rectangle(center_img, big_rect, cv::Scalar(255, 0, 0), 2, 8);
    }
    if (big_rect.x != -1) {
    	center_rows = obj_center_rows;
        center_cols = obj_center_cols;
        down_down    = obj_down;
        valid       = true;
        circle(center_img, cv::Point2f(center_cols, center_rows), 3, Scalar(0, 0, 255), 3, 8, 0);
        printf("exist\n");
     } else {
         valid       = false;
         printf("not exist\n");
     }
    Write_img(src_img, "src_img", start_file_num);
    Write_img(center_img, "center_img", start_file_num);
    Write_img(big_project_result, "project_img", start_file_num);
    start_file_num++;

    MyResult.center.x = obj_center_cols;
    MyResult.center.y = obj_center_rows;
//    MyResult.obj_up   = obj_up;
    MyResult.down = obj_down;
//    MyResult.obj_angle = obj_angle;

    if(obj_center_cols == -1 && obj_center_rows == -1)
    {
            MyResult.valid  = false;
    }
    else
    {
            MyResult.valid  = true;
    }
    (*dynamic_cast<LadderrunResult*>(Result)) = MyResult;

}

void Ladderrun::Load_std_img()
{
    std_big    = imread("gun.png"); 
    cv::cvtColor(std_big, std_big, CV_RGB2HSV);
    cv::split(std_big, std_big_hsv_channel);
    cv::normalize(std_big_hist, std_big, 1.0);
}

void Ladderrun::Pretreat(cv::Mat image)
{
    //basket
    src_img = image.clone();
    center_img = src_img.clone();
    circles_img = src_img.clone();
    cvtColor( src_img, gray_img, CV_BGR2GRAY );//高阶的canny用法，转成灰度图，降噪，用canny
    cv::cvtColor(image, hsv_img, CV_RGB2HSV);
    cv::split(hsv_img, channel);
//    cv::split(hsv_img, trackbar_hsv_channel);

//    imshow("channel[0]", channel[0]);
//    imshow("channel[1]", channel[1]);
//    imshow("channel[2]", channel[2]);

    GaussianBlur( gray_img, blur_img, Size(9, 9), 2, 2 );
    Canny(hsv_img, canny_img, canny_thre, canny_thre_max);
}

void Ladderrun::Dilate_img(Mat src, Mat &dst, int size)
{
    Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    dilate(src, dst, element, Point(-1, -1), 1);
}

void Ladderrun::Erode_img(cv::Mat src, cv::Mat &dst, int size)
{
    cv::Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(src, dst, element, Point(-1, -1), 1);
}

cv::MatND Ladderrun::getHistogram(const cv::Mat &image)
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

cv::Mat Ladderrun::getHistogram_Image(const cv::Mat &image)
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

void Ladderrun::Write_img(cv::Mat& src,  const char floder_name[30], int num)
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

void Ladderrun::set_NotFound()
{
    center_rows = center_cols = -1;
    valid       = false;
}
void Ladderrun::Process_big(cv::Mat image)
{
    image = channel[0].clone();

    //big_project_result = big_BackProject(std_big_hist, image);
    big_project_result = image;

    for(int i = 0; i < channel[0].rows; i++){
        for(int j = 0; j < channel[0].cols; j++){
            int value = channel[0].at<uchar>(i,j);
            if(value >= big_h_min_thre && value <= big_h_max_thre){
                big_project_result.at<uchar>(i,j) = 255;
            }else{
                big_project_result.at<uchar>(i,j) = 0;
            }
        }
    }

    Mat thre_s;
    threshold(channel[1], thre_s, big_s_thre, 255, CV_THRESH_BINARY);
    bitwise_and(thre_s, big_project_result, big_project_result);

//    imshow("big_backproject", big_project_result);

    Find_big_center(big_project_result);
}

void Ladderrun::Find_big_center(cv::Mat src)
{
    obj_center_rows = obj_center_cols = -1;
    cv::Mat img = src.clone();

    big_minrect = RotatedRect( Point2f(0, 0), Size2f(0, 0), 0 );

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
            if (radius[i] > max_radius  )
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
            if (Judge_big_legal(roi_img) == 0)
            {
                set_NotFound();
            }
            else
            {
                obj_center_rows = center[most_possible].y;
                obj_center_cols = center[most_possible].x;
                big_rect        = boundRect[most_possible];
                big_minrect     = minRect[most_possible];

                Point2f rect_points[4];
                minRect[most_possible].points( rect_points );

                for( int j = 0; j < 4; j++ )
                  line( center_img, rect_points[j], rect_points[(j + 1) % 4], color2, 2, 8 );//绘制斜矩minRect
	   		
	   		    double a[4];
            	for( int j = 0; j < 4; j++)
            	{
            		a[j] = rect_points[j].y;
            	}
            	sort(a, a+4);

	    		obj_down = (a[2] + a[3])/2;

            }
        }
        else
        {
            set_NotFound();
        }
    }
    else
    {
        set_NotFound();
    }
}
int Ladderrun::Judge_big_legal(cv::Mat& src)
{
    if (src.rows <= 3 || src.cols <= 10)
        return 0;
    else
        return 1;
}
cv::Mat Ladderrun::big_BackProject(const cv::MatND& hist, const cv::Mat& findimage)
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
void Ladderrun::Load_parameter()
{
//read
//    ifstream ifile("/home/zenghao/HuroCup 2017/Prepare/Ladderrun/test_img/2.txt");
//    ifstream ifile("3.txt");
	ifstream ifile("../source/data/set_ladder_param/3.txt");
    if(!ifile){
        cerr<<"error"<<endl;
    }
    string lineword;

    int i = 0;
    while (ifile >> lineword) {
        cout << lineword << "\n";
        if(i == 0){
            sscanf(lineword.c_str(), "%d", &big_h_min_thre);
        }else if(i == 1){
            sscanf(lineword.c_str(), "%d", &big_h_max_thre);
        }else if(i == 2){
            sscanf(lineword.c_str(), "%d", &big_s_thre);
        }
        i++;
    }

}

void Ladderrun::Store_parameter()
{
//write

//    ofstream out("D:/Robot/HuroCup 2017/Prepare/Ladderrun/test_img/2.txt");
    ofstream out("3.txt");
    if(out.is_open()){
        printf("is_open\n");
        out << big_h_min_thre    << " ";
        out << big_h_max_thre    << " ";
        out << big_s_thre        << " ";
        out.close();
    }else{
        printf("not open\n");
    }
}

void Ladderrun::Set_parameter()
{/*
    namedWindow("big_combine", 1);
    namedWindow("hsv_combine", 1);

    createTrackbar("big_h_min",  "big_combine", &big_h_min_thre, 255, on_trackbar);
    createTrackbar("big_h_max",  "big_combine", &big_h_max_thre, 255, on_trackbar);
    createTrackbar("big_s_thre", "big_combine", &big_s_thre, 255, on_trackbar);

    on_trackbar(big_h_min_thre, 0);*/
}


