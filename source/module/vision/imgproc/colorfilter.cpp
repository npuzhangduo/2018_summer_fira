#include "colorfilter.h"



ColorFilter::ColorFilter()
{
    images.clear();
    contours.clear();
    contours_poly.clear();
    boundRects.clear();
    sumMaxH = 0,sumMinH = 0,sumMaxL = 0,sumMinL = 0,sumMaxS = 0,sumMinS = 0;
}

void ColorFilter::getThresValue(Mat& src)
{
    Mat image = src.clone();
    Rect rect(100,60,80,80);
    rectangle(image,rect,Scalar(255,255,255),2,8);
    imshow("result",image);

    if(waitKey(3) == 't')
    {
       cout<<"take picture successfully"<<endl;
       images.push_back(image);
       Mat roiImg = src(rect);
       cvtColor(roiImg,labImg,CV_BGR2Lab);
       cvtColor(roiImg,hsvImg,CV_BGR2HSV);
       split(labImg,lab);
       split(hsvImg,hsv);
       lImg = lab[0].clone();
       hImg = hsv[0].clone();
       sImg = hsv[1].clone();
       minH = 250;minS = 250;minL = 250;
       maxH = 0;maxS = 0;maxL = 0;
       for(int i = 0; i < roiImg.cols; i++)
       {
           for(int j = 0;j < roiImg.rows; j++)
           {
               if(int(lImg.at<uchar>(j,i)) < minL)
               {
                   minL = int(lImg.at<uchar>(j,i));
               }

               if(int(lImg.at<uchar>(j,i)) > maxL)
               {
                   maxL = int(lImg.at<uchar>(j,i));
               }

               if(int(hImg.at<uchar>(j,i)) < minH)
               {
                   minH = int(hImg.at<uchar>(j,i));
               }

               if(int(hImg.at<uchar>(j,i)) > maxH)
               {
                   maxH = int(hImg.at<uchar>(j,i));
               }

               if(int(sImg.at<uchar>(j,i)) < minS)
               {
                   minS = int(sImg.at<uchar>(j,i));
               }

               if(int(sImg.at<uchar>(j,i)) > maxS)
               {
                   maxS = int(sImg.at<uchar>(j,i));
               }
           }
       }
      sumMaxH += maxH;sumMaxS += maxS;sumMaxL += maxL;
      sumMinH += minH;sumMinS += minS;sumMinL += minL;
    }

    //cout<<"size: "<<images.size()<<endl;
    if(images.size() >= 10)
    {
        maxL = sumMaxL/images.size();
        maxH = sumMaxH/images.size();
        maxS = sumMaxS/images.size();

        minL = sumMinL/images.size();
        minH = sumMinH/images.size();
        minS = sumMinS/images.size();

        minValue.resize(3);
        maxValue.resize(3);

        minValue[0] = minL;minValue[1] = minH;minValue[2] = minS;
        maxValue[0] = maxL;maxValue[1] = maxH;maxValue[2] = maxS;

        cout<<"value: "<<maxL<<" "<<maxH<<" "<<maxS<<" "<<minL<<" "<<minH<<" "<<minS<<endl;

        FileStorage fs("data.xml", FileStorage::WRITE);
             fs << "minValue" << minValue;
             fs << "maxValue" << maxValue;
        fs.release();
        return;
    }
}

void ColorFilter::loadSvm()
{
    minValue.clear();
    maxValue.clear();
    minValue.resize(3);
    maxValue.resize(3);
    FileStorage fs("data.xml", FileStorage::READ);

        fs["minValue"] >> minValue;
        fs["maxValue"] >> maxValue;

        cout<<"value: "<<minValue[0]<<" "<<minValue[1]<<" "<<minValue[2]
           <<" "<<maxValue[0]<<" "<<maxValue[1]<<" "<<maxValue[2]<<endl;

      minL = minValue[0];minH = minValue[1];minS = minValue[2];
      maxL = maxValue[0];maxH = maxValue[1];maxS = maxValue[2];
}

void ColorFilter::imageProcess(cv::Mat img, ImgProcResult *Result)
{
    xsum = 0;
    ysum = 0;
    pixelsum = 0;
    Point value;
   // cout<<"channels: "<<img.channels()<<endl;
    cvtColor(img,labImg,CV_BGR2Lab);
    cvtColor(img,hsvImg,CV_BGR2HSV);
    split(labImg,lab);
    split(hsvImg,hsv);
    lImg = lab[0].clone();
    hImg = hsv[0].clone();
    sImg = hsv[1].clone();

    Mat grayImg = Mat::zeros(img.size(),CV_8UC1);

    for(int i = 0; i < img.cols; i++)
    {
        for(int j = 0;j < img.rows; j++)
        {
            if(int(lImg.at<uchar>(j,i)) > minL && int(lImg.at<uchar>(j,i)) < maxL
                    && int(hImg.at<uchar>(j,i)) > minH && int(hImg.at<uchar>(j,i)) < maxH
                    && int(sImg.at<uchar>(j,i)) > minS && int(sImg.at<uchar>(j,i)) < maxS)
            {
                grayImg.at<uchar>(j,i) = 255;
                xsum = xsum + j;
                ysum = ysum + i;
                pixelsum++;
            }
        }
    }
   if(pixelsum == 0)
   {
       value.x = -1;
       value.y = -1;
   }
   else
   {
   value.x = xsum / pixelsum;
   value.y = ysum / pixelsum;
   }
   if ( value.x == -1 || value.y == -1) {
        MyResult.valid = false;
   }
   else {
        MyResult.center = value;
        MyResult.valid = true;
        circle(img,value,3,Scalar(255,0,0),2,8,0);
   }
    (*dynamic_cast<ColorFilterResult*>(Result)) = MyResult;
   //imshow("result1",img);

  // cout<<"value: "<<value<<endl;
  // waitKey(10);
   //return value;
}
