#include "Find_Colorblock.h"

void Find_Colorblock::Load_src_img(const char path_name[])
{
    src_img = cv::imread(path_name);
}

void Find_Colorblock::imageProcess(cv::Mat img, ImgProcResult *Result)
{
    //printf("\nImage:%d\n", start_file_num);
    src_img = img.clone();
    Write_img(src_img, "src_img", start_file_num);//***change****

    //    printf("216546463\n");
    Get_HSV_img(src_img);
    //Write_img(hsv_img, "hsv_img", start_file_num);//***change****

    //    printf("345251233521\n");
    Get_Gray_img(src_img);
    //Write_img(gray_img, "gray_img", start_file_num);//***change****

    //    printf("4\n");
    Pretreatment_Find_Colorblock();
    //Write_img(erode_img, "erode_img", start_file_num);//***change****

    //    printf("5\n");
    Find_center(erode_img);//**new**
    //    printf("6\n");

    start_file_num++;//***change****
	MyResult.center.x = center_cols;
	MyResult.center.y = center_rows;
	if(center_cols == -1 && center_rows == -1){
		MyResult.valid = false;
	}
	else{
		MyResult.valid = true;
	}
	(*dynamic_cast<Find_ColorblockResult*>(Result)) = MyResult;
}

void Find_Colorblock::Pretreatment_Find_Colorblock()
{
    cv::threshold(gray_img, binary_img, threshval_binary, 255, cv::THRESH_BINARY);
    bitwise_not(binary_img, binary_img);
    cv::threshold(hsv_channels[0], hsv_binary[0], threshval_hsv_channel, 255, cv::THRESH_BINARY);
    bitwise_and(hsv_binary[0], binary_img, and_img);
    Erode_img(and_img, erode_img, 1);
    Dilate_img(erode_img, erode_img, 1);
    //Erode_img(and_img, erode_img, 2);
    //Erode_img(erode_img, erode_img, 2);
}

void Find_Colorblock::Get_HSV_img(cv::Mat src)
{
    cv::cvtColor(src, hsv_img, CV_RGB2HSV);
    cv::split(hsv_img, hsv_channels);
}

void Find_Colorblock::Get_Gray_img(cv::Mat src)
{
    cv::cvtColor(src, gray_img, CV_RGB2GRAY);
}

void Find_Colorblock::Erode_img(Mat src, Mat &dst, int size)
{
    Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    erode(src, dst, element, Point(-1, -1), 1);
}

void Find_Colorblock::Find_center(Mat src)//**new**
{
    cv::Mat img = src.clone();

    std::vector<vector<Point> > contours;
    std::vector<vector<Point> > contours_poly;
    std::vector<RotatedRect> minRect;

    Scalar color = Scalar( 125, 16, 210 );   //定义画的边框颜色
    //    Scalar color2 = Scalar(210, 16, 125);
    Scalar color3 = Scalar(16, 210, 125);
//    Scalar color4 = Scalar(0, 0, 255);   //定义画的边框颜色

    //    printf("52\n");
    cv::findContours(img, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0)); //画出img的轮廓 但貌似会改变原图像
    contours_poly.resize(contours.size());//一定要重新改变size使其与contours的一样，否则会崩溃
    vector<Rect> boundRect( contours.size() );//包围点集的最小矩形vector
    vector<Point2f>center( contours.size() ); //包围点集的最小圆形vector
    vector<float>radius( contours.size() ); //包围点集的最小圆形半径vector
    minRect.resize(contours.size());

    //    printf("54\n");
    for(unsigned int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP(contours[i], contours_poly[i], 3, true );//对轮廓点进行多边形拟合，因为原来直接得到的太支离破碎，把边缘近似为直线
        minRect[i] = minAreaRect(contours_poly[i]);//zh:通过轮廓信息获得轮廓包含轮廓外围的最小矩形,是可以旋转的
        boundRect[i] = boundingRect(contours_poly[i]);//计算并返回包围轮廓点集的最小矩形,但不能画旋转矩形 好像两种都ok
        minEnclosingCircle( contours_poly[i], center[i], radius[i] );//计算并返回包围轮廓点集的最小圆形及其半径
    }
    Mat drawing = Mat::zeros( img.size(), CV_8UC3 );//返回零数组
    Mat drawing_approx = Mat::zeros( img.size(), CV_8UC3 );//返回零数组

//    printf("56\n");
    for(unsigned int i = 0; i < contours.size(); i++ )
    {
        drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );//画没有拟合前的边框
        drawContours( drawing_approx, contours_poly, i, color, 2, 8, vector<Vec4i>(), 0, Point() );//画拟合后的边框//***change****
    }//画边界轮廓

    //printf("contours.size:%d\n", contours.size());
    //Write_img(drawing_approx, "contours_img", start_file_num);//***change****

    //    for( int i = 0; i < contours.size(); i++ )
    //    {
    //        drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );//画没有拟合前的边框
    //        drawContours( drawing_approx, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );//画拟合后的边框

    //        rectangle( drawing_approx, boundRect[i].tl(), boundRect[i].br(), color5, 2, 8, 0 );   //画矩形，tl矩形左上角，br右上角

    //        //retangle只能画正的矩形，

    //        //以下是画斜的矩形
    //        Point2f rect_points[4];
    //        minRect[i].points( rect_points ); //提取旋转矩形的四个角点
    //        for( int j = 0; j < 4; j++ )
    //            line( drawing_approx, rect_points[j], rect_points[(j + 1) % 4], color6, 2, 8 ); //通过四个点画四条直线

    //        circle( drawing_approx, center[i], (int)radius[i], color7, 2, 8, 0 ); //画圆形
    //    }
    if(contours.size()>0)
    {
       // printf("contours.size()>0\n");

        float max_radius = 0.0;
        int most_possible = 0;//****change****//
        for(unsigned int i = 0; i < contours.size(); i++ )
        {
            if(radius[i] > max_radius && boundRect[i].width <= 0.8*img.cols && boundRect[i].height <= 0.8*img.rows && ((1.0*boundRect[i].width/boundRect[i].height <= 2.5) && (1.0*boundRect[i].height/boundRect[i].width <= 2.5)) && boundRect[i].x >= 0.1*img.cols && (boundRect[i].x + boundRect[i].width) <= 0.9*img.cols)
            {
                most_possible = i;
                max_radius = radius[i];//找出最大的半径，也就是最可能的值
            }
        }//找最大半径

        //    rectangle( drawing_approx, boundRect[most_possible].tl(), boundRect[most_possible].br(), color2, 2, 8, 0 );
        //retangle只能画正的矩形，画矩形，tl矩形左上角，br右上角

        Point2f rect_points[4];
        minRect[most_possible].points( rect_points ); //提取旋转矩形的四个角点

//        printf("most_possible_contours:num:%d\n", most_possible);

        for( int j = 0; j < 4; j++ )
            line( drawing_approx, rect_points[j], rect_points[(j + 1) % 4], color3, 4, 8, 0); //通过四个点画四条直线//***change****
        //Write_img(drawing_approx, "rect_img", start_file_num);//***change****

        cv::Mat src_roi = src_img(cv::Rect(boundRect[most_possible].x, boundRect[most_possible].y, boundRect[most_possible].width, boundRect[most_possible].height));
        roi_img = src_roi.clone();
        if(Judge_legal(roi_img) == 0)
        {
            center_rows = center_cols = -1;
           // printf("size too small\n\n");
           // printf("roi_rows:%d roi_cols:%d\n", roi_img.rows, roi_img.cols);
        }
        else
        {
            //printf("size ok\n\n");
            //printf("roi_rows:%d roi_cols:%d\n", roi_img.rows, roi_img.cols);



        Mat hsv_tmp;
        cv::cvtColor(src_img, hsv_tmp, CV_RGB2HSV);
        double avg_h, avg_s, avg_v;
        avg_h = avg_s = avg_v = 0;



            double avg_red, avg_green, avg_blue, sum_point;
            int rows = boundRect[most_possible].height, cols = boundRect[most_possible].width;
            cv::Mat tmp = src_img.clone();

            sum_point = rows*cols;
            avg_blue = avg_green = avg_red = 0;

            for(int i = boundRect[most_possible].y; i < rows+boundRect[most_possible].y; i++)
            {
                for(int j = boundRect[most_possible].x; j < cols+boundRect[most_possible].x; j++)
                {
                    avg_red += src_img.at<Vec3b>(i,j)[0];
                    avg_green += src_img.at<Vec3b>(i,j)[1];
                    avg_blue += src_img.at<Vec3b>(i,j)[2];

                avg_h += hsv_tmp.at<Vec3b>(i,j)[0];
                avg_s += hsv_tmp.at<Vec3b>(i,j)[1];
                avg_v += hsv_tmp.at<Vec3b>(i,j)[2];

                }
            }

            avg_red /= sum_point;
            avg_green /= sum_point;
            avg_blue /= sum_point;

        avg_h /= sum_point;
        avg_s /= sum_point;
        avg_v /= sum_point;


        //printf("\nHSV:avg_h:%lf avg_s:%lf avg_v:%lf\n", avg_h, avg_s, avg_v);

            //        if(avg_red > 70 && avg_green < 50 && avg_blue > 120)
            //if(avg_red > 15 && avg_green < 50 && avg_blue > 50)
           // {
                //printf("color ok\n");
                Scalar color4 = Scalar(255, 0, 0);
                center_rows = 1.0*(boundRect[most_possible].y+rows/2);
                center_cols = 1.0*(boundRect[most_possible].x+cols/2);

                circle(tmp, Point2f(center_cols, center_rows), 2, color4, 2, 8, 0 );//**change**
                Write_img(tmp, "center_img", start_file_num);//**change**
           // }
            /*else
            {
                center_rows = center_cols = -1;
                //printf("not color\n");
                //printf("avg_red:%lf avg_green:%lf avg_blue:%lf\n", avg_red, avg_green, avg_blue);
            }*/
        }
        //printf("center_rows:%lf center_cols:%lf\n", center_rows, center_cols);//***change****
    }
    else
    {
        //printf("contours.size()<= 0 black  not_process\n\n");
    }
    //printf("\n\n");
}

void Find_Colorblock::Write_img(cv::Mat& src,  const char floder_name[30], int num)//****change****
{
    if(start_file_num <= max_file_num)
    {
        char path[150] = {"../source/data/con_img/"};
        //char path[150] = {"/home/zeng/Robot/Find_Colorblock/create_img/"};

        char tail[10] = {".jpg"};
        char number[4];
        sprintf(number, "%d", num);//***change****
        strcat(path,floder_name);
        strcat(path, "/");
        strcat(path, number);
        strcat(path,tail);
//        puts(path);

        cv::imwrite(path, src);
    }
}

void Find_Colorblock::Dilate_img(Mat src, Mat &dst, int size)
{
    Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size) );
    dilate(src, dst, element, Point(-1, -1), 1);
}


//****这次新加的函数****//
int Find_Colorblock::Judge_legal(cv::Mat& src)
{
    if(src.rows >= 3 || src.cols >= 3)
        return 1;
    else
        return 0;
}
