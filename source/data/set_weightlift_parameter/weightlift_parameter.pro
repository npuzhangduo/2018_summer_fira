#-------------------------------------------------
#
# Project created by QtCreator 2017-07-22T19:02:04
#
#-------------------------------------------------
INCLUDEPATH += D:\OpenCV\include\opencv\
                    D:\OpenCV\include\opencv2\
                    D:\OpenCV\include

LIBS += D:\OpenCV\lib\libopencv_calib3d2413.dll.a\
        D:\OpenCV\lib\libopencv_contrib2413.dll.a\
        D:\OpenCV\lib\libopencv_core2413.dll.a\
        D:\OpenCV\lib\libopencv_features2d2413.dll.a\
        D:\OpenCV\lib\libopencv_flann2413.dll.a\
        D:\OpenCV\lib\libopencv_gpu2413.dll.a\
        D:\OpenCV\lib\libopencv_highgui2413.dll.a\
        D:\OpenCV\lib\libopencv_imgproc2413.dll.a\
        D:\OpenCV\lib\libopencv_legacy2413.dll.a\
        D:\OpenCV\lib\libopencv_ml2413.dll.a\
        D:\OpenCV\lib\libopencv_objdetect2413.dll.a\
        D:\OpenCV\lib\libopencv_video2413.dll.a

QT       += core

QT       -= gui

TARGET = weightlift_parameter
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    weightliftingvis.cpp

HEADERS += \
    weightliftingvis.h


/*
QT += core
QT -= gui

CONFIG += c++11

TARGET = weightlifting_vis_qt_put_darwin
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    weightliftingvis.cpp

INCLUDEPATH += .
INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2
INCLUDEPATH += /usr/local/include/opencv2/core
INCLUDEPATH += /usr/local/include/opencv2/highgui
INCLUDEPATH += /usr/local/include/opencv2/imgproc
INCLUDEPATH += /usr/local/include/opencv2/flann
INCLUDEPATH += /usr/local/include/opencv2/photo
INCLUDEPATH += /usr/local/include/opencv2/video
INCLUDEPATH += /usr/local/include/opencv2/features2d
INCLUDEPATH += /usr/local/include/opencv2/objdetect
INCLUDEPATH += /usr/local/include/opencv2/calib3d
INCLUDEPATH += /usr/local/include/opencv2/ml
INCLUDEPATH += /usr/local/include/opencv2/contrib
LIBS += `pkg-config opencv --cflags --libs`

HEADERS += \
    weightliftingvis.h
*/
