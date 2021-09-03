#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#define NIL 0

#include "header.h"
#include <time.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>
#include "/opt/MVS/include/MvCameraControl.h"
#include "/opt/MVS/include/CameraParams.h"
using namespace std;
using namespace cv;

void PressEnterToExit();

enum State
{
    //状态标识
    CLOSED,  //未开启
    OPENED,  //打开
    GRABBING //正扑捉画面

};
class Camera
{
private:
    int index;                       //设备序号
    int height = 1080, width = 1440; //图像宽[32,1440]/高[32,1080]
    unsigned char *pData;            //图像数据指针
    State state;

public:
    void *handle = NULL; //设备句柄

    MV_FRAME_OUT_INFO_EX stImageInfo = {0}; //输出帧结构体
    float frameRate = 150;                  //帧率
    int redbalance,bluebalance,greenbalance;
    float gain = 10;                        //增益[0,17]
    float exposureTime = 4000;              //曝光时间[15,10000000]
    float gamma = 0.7;                      //伽马值[0,4]
    Mat R, T;                               //相机外参
    Mat camera_matrix;                      //相机内参
    vector<Rect> roi;                       //感兴趣的区域
    int pic_size;                               //相机拍摄图片尺寸
    // 无法测量/测量异常的参数：brightness,hue,saturation,sharpness,offsetX,offsetY

    Camera(int index);
    Camera();
    ~Camera();

    void get_height();
    void get_width();

    void readRT();                        //读入R、T 
    void zhuanhuanRT();                   //最终点转换
    void makeHandle(void **handle);       //创造设备句柄
    void openDevice();                    //打开设备
    void initCamera();                    //初始化设置相机参数
    void setLight(float gain,float exposure);                       //设置增益值、曝光时间
    void displayCamera();                 //显示相机当前图像
    void grabImage();                     //通过主动取图方式获取相机图像（无释放图像缓存）
    bool getImage(Mat &frame);            //获得图像,返回值为是否成功
    bool getImage(unsigned char *pData2); //获得图像，得到缓存，raw
    
    void camera_calibration_external();               //外参标定
    Point world_to_pixel(Point3f point);              //世界坐标到像素点
    Point camera_to_pixel(Point3f point);             //世界坐标到像素点
    Point3f camera_to_world(Point3f in, Point3f out); //相机坐标转世界坐标

    bool l_r;//左右手系
};

#endif
