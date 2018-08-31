#include "fira_obstacle_line.h"

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
//void FiraObstacleLine::imageProcess(cv::Mat image, ImgProcResult *Result)
void FiraObstacleLine::imageProcess(cv::Mat image)
{
    line_setNotFound();
    Pretreat(image);
    Process_line(image);
    Set_parameter();

    /*******************************************/
    if (line_rect.x != -1) 
    {
        rectangle(center_img, line_rect, cv::Scalar(255, 0, 0), 2, 8);

        line_center_rows = line_rect.y+line_rect.height*0.5;
        line_center_cols = line_rect.x+line_rect.width*0.5;
        line_angle = line_rect.width / line_rect.height;
        line_valid = true;

        circle(center_img, cv::Point2f(line_center_cols, line_center_rows), 2, cv::Scalar(255, 0, 255), 6, 8, 0);
    }
    else
        line_valid = false;

    Write_img(src_img, "src_img", start_file_num);
    Write_img(center_img, "center_img", start_file_num);

    start_file_num++;

//    MyResult.ball_center.x = ball_center_cols;
//    MyResult.ball_center.y = ball_center_rows;
//    MyResult.ball_valid = ball_valid;

//    MyResult.hole_center.x = hole_center_cols;
//    MyResult.hole_center.y = hole_center_rows;
//    MyResult.hole_valid = hole_valid;
//    (*dynamic_cast<FiraObstacleLineResult*>(Result)) = MyResult;
}

void FiraObstacleLine::Pretreat(cv::Mat image)
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

void FiraObstacleLine::Dilate_img(Mat src, Mat &dst, int size)
{
    Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    dilate(src, dst, element, Point(-1, -1), 1);
}

void FiraObstacleLine::Erode_img(cv::Mat src, cv::Mat &dst, int size)
{
    cv::Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(src, dst, element, Point(-1, -1), 1);
}

void FiraObstacleLine::Write_img(cv::Mat& src,  const char floder_name[30], int num)
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

/******FiraObstacleLine Line******/
void FiraObstacleLine::Process_line(cv::Mat image)
{
    threshold(gray_img, white_thre_result, white_gray_thre, 255, CV_THRESH_BINARY);

    Erode_img(white_thre_result, white_thre_result, white_erode_size);
    Dilate_img(white_thre_result, white_thre_result, white_dilate_size);

    Find_line_center(white_thre_result);
}

void FiraObstacleLine::Find_line_center(cv::Mat src)
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
			line_minRect     = minRect[most_possible];

			Point2f rect_points[4];
			minRect[most_possible].points( rect_points );
			    
			for( int j = 0; j < 4; j++ )
          		line( center_img, rect_points[j], rect_points[(j + 1) % 4], color3, 2, 8 );//绘制斜矩minRect


                if(line_minRect.size.width > line_minRect.size.height)
                	line_angle = - line_minRect.angle;
                else if(line_minRect.size.width < line_minRect.size.height)
                	line_angle = -(90 + line_minRect.angle);
                //printf("angle:::%lf\n", line_angle);


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

int FiraObstacleLine::Judge_line_legal(cv::Mat& src)
{
    if (src.cols <=  30)
        return 0;
    else
        return 1;
}

void FiraObstacleLine::line_setNotFound()
{
    line_center_rows = line_center_cols = -1;
    line_rect.x = line_rect.y = -1;
    line_valid = false;
}

/*********Parameter*********/
void FiraObstacleLine::Load_parameter()
{
//read
//    ifstream ifile("/home/zenghao/HuroCup 2017/Prepare/FiraObstacleLine/test_img/2.txt");
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

void FiraObstacleLine::Store_parameter()
{
//write

//    ofstream out("D:/Robot/HuroCup 2017/Prepare/FiraObstacleLine/test_img/2.txt");
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

void FiraObstacleLine::Set_parameter()
{
    namedWindow("gray_combine");

    createTrackbar("white_gray_thre",   "gray_combine", &white_gray_thre,   255, on_trackbar);
    createTrackbar("white_erode_size",  "gray_combine", &white_erode_size,  6,   on_trackbar);
    createTrackbar("white_dilate_size", "gray_combine", &white_dilate_size, 6,   on_trackbar);

    on_trackbar(white_gray_thre, 0);
}
