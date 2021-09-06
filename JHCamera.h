#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>

struct Parameter
{
    int width = 1920;//图像宽
    int height = 1080;//图像高
    int Exposure = 1084;//曝光值 【1，32768】1084
    int Gain = 180;//增益值 【1，255】，低于32属于模拟增益，高于32属于数字增益
    int AdvancedGain = 0;//高级增益 【0，5】
    int refer = 128;//亮度参考值
    double Gamma = 2;//伽马值 【0.3，2.0】 
    double ConTrast = 0.3;//图像对比度 【0.3，2.0】
    double Saturation = 0.3;//颜色饱和度 【0.0，2.0】
    int BlackLevel = 0;//黑电平 【0，255】
    int Delay = 0;//延迟
    int FrameRate = 120;//设置输出帧率为最大值以下的某个值，可返回最大帧率
    double rg = 0;//白平衡红色增益 【0.0，3.0】
    double gg = 1.0;//白平衡绿色增益 【0.0，3.0】通常固定为1.0
    double bg = 0;//白平衡蓝色增益 【0.0，3.0】
    int LineHeight = 2;//线扫高度 【2，32】
    int LineTrigger = 0;//行触发模式，包括相机以最大速度触发；外部信号用于行触发
    double unit = 0;//单位曝光时间
    
    bool Speed = true;//相机高速模式
    bool Denoise = true;//软件降噪模式
    bool FPN = false;//修复条纹瑕疵
    bool AEC = false;//自动曝光
    bool AGC = true;//自动增益
    bool AWB = true;//自动白平衡
    bool Enhancement = false;//相机颜色增强模式，开启可能降低帧率
    bool AntiFlicker = false;//防闪烁模式，仅在启动自动曝光时生效
};

class Camera
{
    public:
        unsigned char* imgbuf;//相机获取的图像
        int length;//imgbuf数据长度
        cv::Mat frame;
        Parameter parameter;//相机参数

    public:
        /**
        * @brief Set camera index
        * @param index: open which camera
        */
        Camera(int index);
        ~Camera();

        /**
        * @brief Initialize the camera settings
        */
        void cameraSet();

        /**
        * @brief get camera setting-information
        */
        void cameraInformation();

        /**
        * @brief read photo
        * @return API_OK
        */
        int openVideo();

        /**
        * @brief show camera debug information
        */
        void debugInformation();

    private:
        int index;//相机编号

    private:
        /**
        * @brief it can set the camera gain automatically
        */
        void setBrightness();

};

#endif
