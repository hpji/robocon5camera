#include "camera.h"

unsigned int g_nPayloadSize = 0;
int fps = 0;
time_t org_t = time(0);
time_t now_t;

Camera::Camera(int index)
{
    this->index = index;
    this->makeHandle(&this->handle);
    this->state = CLOSED;
    this->openDevice();
}

Camera::~Camera()
{
}
Camera::Camera()
{
}

void Camera::readRT()
{
    // R = Mat(3, 3, CV_32F);
    // std::ifstream file("/home/gohu3/PGLUI/WC-DM3/result.txt");
    // double f[3][3];
    // for (int i = 0; i < 3; i++)
    //     for (int j = 0; j < 3; j++)
    //     {
    //         file >> f[i][j];
    //         R.at<double>(i, j) = f[i][j];
    //     }
    // T = Mat(3, 1, CV_32F);
    // for (int i = 0; i < 3; i++)
    // {
    //     file >> T.at<double>(0, i);
    // }
    // char c;
    // file >> c;
    // cout << R << T << endl;
    FileStorage fsread("stereo.xml", FileStorage::READ);
    fsread["R"] >> R;
    fsread["T"] >> T;
    cout << R << T << endl;
    int c;
    fsread["r_l"] >> c;
    cout << c << endl;
    if (c == 1)
        l_r = 1;
    else
        l_r = 0;
    fsread.release();
}

//创造设备句柄
void Camera::makeHandle(void **handle)
{
    int nRet = MV_OK;

    // 初始化设备信息列表结构体变量
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    // 枚举设备
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if (MV_OK != nRet)
    {
        printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
    }

    // nDeviceNum在线设备数量
    if (stDeviceList.nDeviceNum > 0)
    {
        for (int i = 0; i < stDeviceList.nDeviceNum; i++)
        {
            printf("[device %d]:", i);
            MV_CC_DEVICE_INFO *pDeviceInfo = stDeviceList.pDeviceInfo[i];
            if (NULL == pDeviceInfo)
            {
                break;
            }
            else
            {
                printf("Info get\n");
            }
        }
    }
    else
    {
        printf("Find No Devices!\n");
    }

    // 选择设备并创建句柄
    nRet = MV_CC_CreateHandle(&this->handle, stDeviceList.pDeviceInfo[this->index]);
    if (MV_OK != nRet)
    {
        printf("MV_CC_CreateHandle fail! nRet [%x]\n", nRet);
    }
    else
        printf("make handle OK\n");
}

//打开设备
void Camera::openDevice()
{
    int nRet;

    printf("start to open device\n");
    nRet = MV_CC_OpenDevice(this->handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_OpenDevice fail! nRet [%x]\n", nRet);
    }
    else
    {
        printf("open device ok\n");
        this->state = OPENED;
    }
}

//初始化设置相机参数
void Camera::initCamera()
{
    int nRet = MV_OK;

    printf("\nstart to set params:\n");
    //关闭采集帧率控制
    nRet = MV_CC_SetBoolValue(this->handle, "AcquisitionFrameRateEnable", 0);
    if (nRet == MV_OK)
    {
        std::cout << "acquisition limit:off" << std::endl;
    }
    else
    {
        std::cout << "acquisition limit:on" << std::endl;
    }

    nRet = MV_CC_SetIntValue(this->handle, "Width", this->width);
    if (nRet == MV_OK)
    {
        std::cout << "Width set cuccessed" << std::endl;
    }
    else
    {
        std::cout << "Width set failed" << std::endl;
    }

    nRet = MV_CC_SetIntValue(this->handle, "Height", this->height);
    if (nRet == MV_OK)
    {
        std::cout << "Height set cuccessed" << std::endl;
    }
    else
    {
        std::cout << "Height set failed" << std::endl;
    }

    MVCC_INTVALUE balance = {0};
    nRet = MV_CC_GetBalanceRatioRed(this->handle, &balance);
    std::cout<<this->index<<' '<<"red balance:"<<balance.nCurValue<<' '<<balance.nMin<<' '<<balance.nMax<<std::endl;
    this->redbalance=balance.nCurValue;
    nRet = MV_CC_GetBalanceRatioBlue(this->handle, &balance);
    std::cout << "blue balance:" << balance.nCurValue << ' ' << balance.nMin << ' ' << balance.nMax << std::endl;
    this->bluebalance=balance.nCurValue;
    nRet = MV_CC_GetBalanceRatioGreen(this->handle, &balance);
    std::cout << "green balance:" << balance.nCurValue << ' ' << balance.nMin << ' ' << balance.nMax << std::endl;
    this->greenbalance=balance.nCurValue;

    nRet = MV_CC_SetFloatValue(this->handle, "ExposureTime", this->exposureTime);
    if (nRet != MV_OK)
    {
        std::cout << "ExposureTime set failed" << std::endl;
    }
    else
    {
        std::cout << "ExposureTime set successed" << std::endl;
    }

    nRet = MV_CC_SetFloatValue(this->handle, "Gain", this->gain);
    if (nRet != MV_OK)
    {
        std::cout << "Gain set failed" << std::endl;
    }
    else
    {
        std::cout << "Gain set successed" << std::endl;
    }

    nRet = MV_CC_SetGamma(this->handle, this->gamma);
    if (nRet == MV_OK)
    {
        std::cout << "Gamma set successed" << std::endl;
    }
    else
    {
        std::cout << "Gamma set failed" << std::endl;
    }

    nRet = MV_CC_SetFrameRate(this->handle, this->frameRate);
    if (nRet == MV_OK)
    {
        std::cout << "FrameRate set successed" << std::endl;
    }
    else
    {
        std::cout << "FrameRate set failed" << std::endl;
    }

    // 获取一帧数据的大小
    MVCC_INTVALUE stParam;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    nRet = MV_CC_GetIntValue(this->handle, "PayloadSize", &stParam);
    if (MV_OK != nRet)
    {
        printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);
    }
    g_nPayloadSize = stParam.nCurValue;
    pic_size = stParam.nCurValue;

    memset(&this->stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));

    this->pData = (unsigned char *)malloc(sizeof(unsigned char) * (g_nPayloadSize));

    if (nRet == MV_OK)
    {
        printf("Init finished successfully\n");
    }
}

void Camera::setLight(float gain, float exposure)
{
    int nRet = MV_OK;
    nRet = MV_CC_SetFloatValue(this->handle, "Gain", gain);
    if (nRet == MV_OK)
    {
        std::cout << "Gain set successed:" << gain << std::endl;
        this->gain = gain;
    }
    else
    {
        std::cout << "Gain set failed" << std::endl;
    }

    if (exposure >= 500)
    {
        nRet = MV_CC_SetFloatValue(this->handle, "ExposureTime", exposure);
        if (nRet != MV_OK)
        {
            std::cout << "ExposureTime set failed" << std::endl;
        }
        else
        {
            std::cout << "ExposureTime set successed:" << exposure << std::endl;
            this->exposureTime = exposure;
        }
    }
}

//显示相机当前图像
void Camera::displayCamera()
{
    Window w;
    Display *dpy = NULL;
    memset(&w, 0, sizeof(Window));
    dpy = XOpenDisplay(NIL);
    if (NULL == dpy)
    {
        printf("please run with screan environment\n");
    }
    else
    {
        std::cout << "dpy ok" << std::endl;
    }

    int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));
    w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 752, 480, 0, 0xffff00ff, 0xff00ffff);
    // 获取改变窗口大小事件
    XSelectInput(dpy, w, StructureNotifyMask);
    // 使窗口可见
    XMapWindow(dpy, w);
    // 创建图像上下文给出绘图函数的属性
    GC gc = XCreateGC(dpy, w, 0, NULL);
    // 告诉GC使用白色
    XSetForeground(dpy, gc, whiteColor);
    // 等待事件的到来
    for (;;)
    {
        XEvent e;
        XNextEvent(dpy, &e);
        if (e.type == MapNotify)
        {
            break;
        }
    }

    int nRet = MV_OK;

    do
    {
        // 开始取流
        nRet = MV_CC_StartGrabbing(this->handle);
        if (MV_OK != nRet)
        {
            printf("MV_CC_StartGrabbing fail! nRet [%x]\n", nRet);
            break;
        }
        nRet = MV_CC_Display(this->handle, (void *)w);
        if (MV_OK != nRet)
        {
            printf("MV_CC_Display fail! nRet [%x]\n", nRet);
            break;
        }
        printf("Display succeed\n");

        PressEnterToExit();

    } while (0);
}

//通过主动取图方式获取相机图像（无释放图像缓存）
void Camera::grabImage()
{
    int nRet = MV_OK;

    //关闭触发模式
    nRet = MV_CC_SetEnumValue(this->handle, "TriggerMode", 0);
    if (MV_OK != nRet)
    {
        printf("Set Trigger Mode fail! nRet [0x%x]\n", nRet);
    }

    //开始取流
    nRet = MV_CC_StartGrabbing(this->handle);
    if (MV_OK != nRet)
    {
        printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
    }
    else
    {
        std::cout << "start grabbing successed" << std::endl;
        state = GRABBING;
    }

    while (1)
    {
        nRet = MV_CC_GetOneFrameTimeout(this->handle, this->pData, g_nPayloadSize, &this->stImageInfo, 1000);

        fps++;
        now_t = time(0);
        if (now_t - org_t >= 1)
            if (now_t - org_t >= 1)
            {
                org_t = time(0);
                std::cout << "current fps:" << fps << std::endl; //输出当前帧率
                fps = 0;
            }

        cv::Mat current_frame(this->height, this->width, CV_8UC1, this->pData);
        cv::imshow("current", current_frame);
        if (cv::waitKey(1) == 'q')
        {
            cv::destroyAllWindows();
            break;
        }
    }
}
bool Camera::getImage(Mat &frame)
{
    int nRet = MV_OK;

    //关闭触发模式
    nRet = MV_CC_SetEnumValue(this->handle, "TriggerMode", 0);
    if (MV_OK != nRet)
    {
        printf("Set Trigger Mode fail! nRet [0x%x]\n", nRet);
    }

    //开始取流
    if (state != GRABBING)
    {
        nRet = MV_CC_StartGrabbing(this->handle);
        if (MV_OK != nRet)
        {
            printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
            return false;
        }
        else
        {
            std::cout << "start grabbing successed" << std::endl;
            state = GRABBING;
        }
    }
    nRet = MV_CC_GetOneFrameTimeout(this->handle, this->pData, g_nPayloadSize, &this->stImageInfo, 1000);
    if (MV_OK != nRet)
    {
        int count = 100;
        while (cout)
        {
            nRet = MV_CC_GetOneFrameTimeout(this->handle, this->pData, g_nPayloadSize, &this->stImageInfo, 1000);
            count--;
            /* code */
            if (nRet == MV_OK)
                break;
        }
        if (nRet != MV_OK)
        {
            printf("Getting image failed! nRet [0x%x]\n", nRet);
            return false;
        }
    }
    cv::Mat raw(this->height, this->width, CV_8UC1, this->pData);
    cv::cvtColor(raw, frame, COLOR_BayerBG2BGR);
    if (frame.empty())
        return false;
    return true;
}
bool Camera::getImage(unsigned char *pData2)
{
    int nRet = MV_OK;

    //关闭触发模式
    nRet = MV_CC_SetEnumValue(this->handle, "TriggerMode", 0);
    if (MV_OK != nRet)
    {
        printf("Set Trigger Mode fail! nRet [0x%x]\n", nRet);
    }

    //开始取流
    if (state != GRABBING)
    {
        nRet = MV_CC_StartGrabbing(this->handle);
        if (MV_OK != nRet)
        {
            printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
            return false;
        }
        else
        {
            std::cout << "start grabbing successed" << std::endl;
            state = GRABBING;
        }
    }
    nRet = MV_CC_GetOneFrameTimeout(this->handle, pData2, g_nPayloadSize, &this->stImageInfo, 1000);
    if (MV_OK != nRet)
    {
        int count = 100;
        while (cout)
        {
            nRet = MV_CC_GetOneFrameTimeout(this->handle, pData2, g_nPayloadSize, &this->stImageInfo, 1000);
            count--;
            /* code */
            if (nRet == MV_OK)
                break;
        }
        if (nRet != MV_OK)
        {
            printf("Getting image failed! nRet [0x%x]\n", nRet);
            return false;
        }
    }
    return true;
}
void Camera::get_height()
{
    int nRet = MV_OK;

    MVCC_INTVALUE stHeight = {0};
    nRet = MV_CC_GetIntValue(this->handle, "Height", &stHeight);
    if (MV_OK == nRet)
    {
        printf("height current value:%d\n", stHeight.nCurValue);
        printf("height max value:%d\n", stHeight.nMax);
        printf("height min value:%d\n", stHeight.nMin);
        printf("height increment value:%d\n\n", stHeight.nInc);
    }
    else
    {
        printf("get height failed! nRet [%x]\n\n", nRet);
    }
}

void Camera::get_width()
{
    int nRet = MV_OK;

    MVCC_INTVALUE stWidth = {0};
    nRet = MV_CC_GetIntValue(this->handle, "Width", &stWidth);
    if (MV_OK == nRet)
    {
        printf("width current value:%d\n", stWidth.nCurValue);
        printf("width max value:%d\n", stWidth.nMax);
        printf("width min value:%d\n", stWidth.nMin);
        printf("width increment value:%d\n\n", stWidth.nInc);
    }
    else
    {
        printf("get width failed! nRet [%x]\n\n", nRet);
    }
}

void PressEnterToExit()
{
    int c;
    fprintf(stderr, "\nPress enter to exit.\n");
    while (getchar() != '\n')
        ;
    sleep(1);
}

Point Camera::camera_to_pixel(Point3f point)
{
    Mat xyz = (Mat_<double>(3, 1) << point.x, point.y, point.z);
    Mat pixel = camera_matrix * xyz / xyz.at<double>(2, 0);
    return Point(pixel.at<double>(0, 0), pixel.at<double>(1, 0));
}
Point Camera::world_to_pixel(Point3f point)
{
    Mat world = (Mat_<double>(3, 1) << point.x, point.y, point.z);
    Mat xyz = R * world + T;
    Point3f camera = Point3f(xyz.at<double>(0, 0), xyz.at<double>(1, 0), xyz.at<double>(2, 0));
    return camera_to_pixel(camera);
}
