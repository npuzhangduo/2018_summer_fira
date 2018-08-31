//**************************************************************************
//--------------------------------------------------------------------------
// Histogram Parameter Set
//
// Hao Zeng
// version 2.1
// Date    2017.7.14
//--------------------------------------------------------------------------
//**************************************************************************

#include "fira_2v2_vision.h"

int main(int argc, char *argv[])
{
    VideoCapture cap(0);

    cv::Mat image;
    Histogram a;
    char c;

    bool is_video = true;
    int i = 1;

    if(!cap.isOpened())
    {
        printf("video is not open\n");
        return -1;
    }

    bool stop = false;
    while(!stop)
    {
        if(is_video){
            cap>>image;
        }else{
            char path[150] = {"D:/Robot/HuroCup 2017/Prepare/Histogram/test_img/4/"};
            char tail[10] = {".jpg"};
            char num[4];
            sprintf(num, "%d", i);
            strcat(path, num);
            strcat(path, tail);
            image = imread(path);
        }

        if(image.empty())
        {
            printf("image not load\n");
        }
        else
        {

            a.imageProcess(image);

            namedWindow("src_img", 0);
            cv::resizeWindow("src_img",320, 250);
            imshow("src_img", image);
            imshow("center_img", a.center_img);

            if(is_video){
                c = waitKey(1000);
            }else{
                c = waitKey(0);
            }

            if(c == 'q'){
                printf("quit\n");
                break;
            }else if(c == 's'){
                printf("save\n");
                a.Store_parameter();
                break;
            }
        }
        i++;
    }
    if(c != 'q' && c != 's'){
        a.Store_parameter();
    }
    return 0;
}
