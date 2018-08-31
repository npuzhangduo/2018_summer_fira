#include "penaltyimg.h"
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




void PenaltyImg::imageProcess(Mat image, ImgProcResult *Result){
//void PenaltyImg::imageProcess(Mat image){
    this->ball_setNotFound();
    this->hole_setNotFound();

    // this->Load_std_img();
    this->Pretreat(image);

    this->Process_ball();

    ///AB
    this->vertical_projection.clear();
    this->horizontal_projection.clear();

    this->get_uesd_channels();
    this->clear_FBground();
    this->threshold_projection_image();
    this->get_object_position();
    ///end of AB

    ///judge the ball
    if (ball_rect.x != -1) {
        cv::rectangle(center_img, ball_rect, cv::Scalar(255, 0, 0), 2, 8);

        ball_center_rows = ball_rect.y+ball_rect.height*0.5;
        ball_center_cols = ball_rect.x+ball_rect.width*0.5;
        ball_valid = true;
    }else{
        ball_valid = false;
        printf("ball not exist\n");
    }
    ///judge the hole
    if ( abs(this->kmeans_left - this->kmeans_right) == 1 ){
        this->hole_vaild = true;
        if ( this->horizontal_projection[kmeans_left] > this->horizontal_projection[kmeans_right] ){
            this->hole_right_vaild = true;
            this->hole_edge = this->kmeans_left;
        }
        else {
            this->hole_right_vaild = false;
            this->hole_edge = this->kmeans_right;
        }
        cv::line(this->center_img,
                 cv::Point(this->hole_edge,0),cv::Point(this->hole_edge,this->center_img.rows),
                 cv::Scalar(0,0,255),2);
        if (this->hole_right_vaild){
            cout<<"detect the right edge"<<endl;
        }
        else {
            cout<<"detect the left edge"<<endl;
        }
    }
    else{
        this->hole_vaild = false;
        printf("hole not exist\n");
    }

    this->Write_img(this->src_img,"src_img",this->start_file_num);
    this->Write_img(this->center_img,"center_img",this->start_file_num);

    this->start_file_num++;
    printf("num:::%d\n", start_file_num);


    MyResult.ball_center.x = ball_center_cols;
    MyResult.ball_center.y = ball_center_rows;
    MyResult.ball_valid = ball_valid;

    MyResult.hole_right_vaild = this->hole_right_vaild;
    MyResult.hole_valid = this->hole_vaild;
    MyResult.hole_center.y = this->hole_edge;
    (*dynamic_cast<PenaltyImgResult*>(Result)) = MyResult;
}
void PenaltyImg::ball_setNotFound(){
    ball_center_rows = ball_center_cols = -1;
    ball_rect.x = ball_rect.y = -1;
    ball_valid = false;
}
void PenaltyImg::hole_setNotFound(){
    this->hole_edge = -1;
    this->kmeans_left = -1;
    this->kmeans_right = -1;
    this->hole_vaild = false;
}

void PenaltyImg::Load_std_img(){
    std_ball = cv::imread("/home/robotis/Desktop/2017 Fira compete(zhang)/source/data/yellowbox.jpg");
//    std_ball = imread("G:/pic/sample/cat1.jpg");
//    std_ball = imread("/home/robotis/Desktop/2017 Fira compete/source/data/std_img/std_img.jpg");//yellow
    cv::cvtColor(std_ball, std_ball, CV_RGB2HSV);
    std_ball_hist = getHistogram(std_ball_hsv_channel[0]);
    cv::normalize(std_ball_hist, std_ball_hist, 1.0);
}

void PenaltyImg::Pretreat(cv::Mat image){
    this->src_img = image.clone();
    this->center_img = image.clone();
    this->circles_img = image.clone();

    cv::cvtColor( this->src_img, this->gray_img, CV_BGR2GRAY );
    cv::cvtColor( this->src_img, this->hsv_img, CV_BGR2HSV );
    cv::split( this->hsv_img, this->channel );

    cv::GaussianBlur( this->gray_img, this->blur_img, cv::Size(9,9), 2, 2 );
    cv::Canny( this->hsv_img, this->canny_img, this->canny_thre, this->canny_thre_max );
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

//new
void PenaltyImg::Load_parameter()
{
//read
//    ifstream ifile("/home/zenghao/HuroCup 2017/Prepare/PenaltyImg/test_img/2.txt");
    ifstream ifile("../source/data/set_penalty_parameter/3.txt");//set_penalty_parameter
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

void PenaltyImg::Write_img(Mat &src, const char floder_name[], int num){
    if (start_file_num <= max_file_num){
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

void PenaltyImg::Process_ball(){
    cv::Mat t_H_channel = this->channel[0].clone();

    // this->ball_project_result = this->ball_BackProject( this->std_ball_hist, t_H_channel );
    this->ball_project_result = t_H_channel.clone();

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
    cv::Mat thre_s;
    cv::threshold(channel[1], thre_s, ball_s_thre, 255, CV_THRESH_BINARY);
    cv::bitwise_and(thre_s, ball_project_result, ball_project_result);

    this->Find_ball_center(this->ball_project_result);
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


///AB
void PenaltyImg::get_uesd_channels(){
    std::vector<cv::Mat> t_BGR_channels;
    cv::split( this->src_img, t_BGR_channels );
    this->lo_used_channel = t_BGR_channels[2];
}
void PenaltyImg::clear_FBground(){
    this->get_vertical_projection( this->lo_used_channel );

    std::vector<double>::iterator t_min_index = std::min_element( this->vertical_projection.begin(), this->vertical_projection.end() );
    int t_threshold_rows = (int)(t_min_index - this->vertical_projection.begin());

    if ( this->debug_print ){
        cout<<"the threshold row is: "<<t_threshold_rows;
    }
    int t_cols = this->lo_used_channel.cols;
    int t_rows = this->lo_used_channel.rows;
    for (int i = t_threshold_rows; i < t_rows; i++){
        cv::line( this->lo_used_channel,
                  cv::Point(0,i),cv::Point(t_cols,i),0);
    }
    //再通过L通道 去除白线
    cv::Mat t_HLS_image;
    std::vector<cv::Mat> t_HLS_channels;
    cv::cvtColor(this->src_img, t_HLS_image,CV_BGR2HLS);
    cv::split(t_HLS_image, t_HLS_channels);

    cv::threshold(t_HLS_channels[1],t_HLS_channels[1],100,255,CV_THRESH_OTSU|CV_THRESH_BINARY_INV);
    cv::bitwise_and(t_HLS_channels[1],this->lo_used_channel,this->lo_used_channel);

    this->lo_used_channel.copyTo( this->lo_used_channel_without_FBground );
}
void PenaltyImg::threshold_projection_image(){
    cv::threshold( this->lo_used_channel_without_FBground, this->lo_used_channel_without_FBground, 0, 255.,CV_THRESH_OTSU );
    cv::equalizeHist( this->lo_used_channel_without_FBground, this->lo_used_channel_without_FBground );
}
void PenaltyImg::get_object_position(){
    this->get_horizontal_projection( this->lo_used_channel_without_FBground );

    ///use kmeans algoritm to get the edge
    cv::Mat t_kmeans_sample( this->horizontal_projection.size(), 1, CV_32FC3 );
    cv::Mat t_labels( this->horizontal_projection.size(), 1, CV_32SC1 );
    int k = 2;
    cv::Mat t_centers(k,1,t_kmeans_sample.type());

    for ( size_t i = 0; i < this->horizontal_projection.size(); i++ ){
        t_kmeans_sample.at<cv::Vec3f>(i,0)[0] = this->horizontal_projection[i];
        t_kmeans_sample.at<cv::Vec3f>(i,0)[1] = 0;
        t_kmeans_sample.at<cv::Vec3f>(i,0)[2] = 0;
    }
    cv::kmeans( t_kmeans_sample, k, t_labels,
                cv::TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 0.1 ),
                3, KMEANS_PP_CENTERS, t_centers
                );
    //找kmeans_left
    for (int i = 0; i < t_labels.rows-1; i++){
        if ( t_labels.at<int>(i, 0) != t_labels.at<int>(i+1,0) ){
            this->kmeans_left = i;
            break;
        }
    }
    for (int i = t_labels.rows-1; i >= 1; i--){
        if ( t_labels.at<int>(i, 0) != t_labels.at<int>(i-1,0) ){
            this->kmeans_right = i;
            break;
        }
    }
//    if (this->debug_print){
        cout<<"k left: "<<this->kmeans_left<<' '<<"k right: "<<this->kmeans_right<<endl;
//    }
}
///AB's backup function

void PenaltyImg::get_vertical_projection(cv::Mat inputImg){
    int rows = inputImg.rows;
    int cols = inputImg.cols;

    double t_sum;
    for (int i = 0; i < rows; i++){
        t_sum = 0.0;
        for (int j = 0; j < cols; j++){
            t_sum += inputImg.at<uchar>(i,j);
        }
        this->vertical_projection.push_back(t_sum);
    }
}

void PenaltyImg::get_horizontal_projection(cv::Mat inputImg){
    int rows = inputImg.rows;
    int cols = inputImg.cols;

    double t_sum;
    for (int i = 0; i< cols; i++){
        t_sum = 0.0;
        for (int j = 0; j < rows; j++){
//            t_sum += inputImg.at<uchar>(j,i);
            if ( inputImg.at<uchar>(j,i) == 255 ){
                t_sum += 1;
            }
        }
        this->horizontal_projection.push_back(t_sum);
    }

}
