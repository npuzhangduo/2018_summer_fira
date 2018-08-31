//**************************************************************************
//--------------------------------------------------------------------------
// Set FiraObstacleVision Paramenter by HSV
//
// Hao Zeng
// version 1.0
// Date    2017.9.19
//--------------------------------------------------------------------------
//**************************************************************************

#include "fira_obstacle_vision.h"

int main(int argc, char *argv[])
{
    VideoCapture cap(0);

    cv::Mat image;
    FiraObstacleVision g;
    char c;

    bool is_video = true;
    int i = 1;

    if(!cap.isOpened() && is_video == true)
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
            char path[150] ={"D:/Robot/FIRA 2017/Prepare/FiraObstacleVision/test_img/1/"};
            char tail[10] = {".jpg"};
            char num[4];
            sprintf(num, "%d", i);
            strcat(path, num);
            strcat(path, tail);
            image = imread(path);
        }

        if(image.empty() && is_video)
        {
            printf("image not load\n");
        }
        else
        {

            g.imageProcess(image);

            namedWindow("src_img", 0);
            cv::resizeWindow("src_img",320, 250);
            imshow("src_img", image);
            imshow("center_img", g.center_img);

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
                g.Store_parameter();
                break;
            }
        }
        i++;
    }
    if(c != 'q' && c != 's'){
        g.Store_parameter();
    }
    return 0;
}

