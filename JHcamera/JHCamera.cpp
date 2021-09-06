#include "JHCamera.h"
#include "JHCap.h"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/imgproc/imgproc_c.h"

Camera::Camera(int index)
{
    this->index = index;
	if (CameraInit(this->index) == API_OK)//判断相机是否成功开启
	{
		std::cout << "Success to open Camera_" << this->index << std::endl;
	}
	else
	{
		std::cout << "Fail to open Camera_" << this->index << std::endl;
		return;
	}
    this->cameraSet();
}

Camera::~Camera()
{
    CameraFree(this->index);
    delete this->imgbuf;
}

void Camera::cameraSet()
{
	CameraGetImageBufferSize(this->index, &this->length, CAMERA_IMAGE_RGB24);//获取图像大小信息
	this->imgbuf = new unsigned char[this->length];//创建一个变量空间存放图片
	CameraSetResolution(this->index, 0, &this->parameter.width, &this->parameter.height);//分辨率
    CameraSetHighspeed(this->index, this->parameter.Speed);//图像获取速度模式
	CameraSetEnhancement(this->index,this->parameter.Enhancement);//开启颜色增强
	CameraGetExposureUnit(this->index,&this->parameter.unit);//获取单位曝光时间
	std::cout<<"unit="<<this->parameter.unit<<std::endl;
	// CameraSetFrameRate(this->index,false,this->parameter.FrameRate);//设置限制帧率，使帧率为最大值以下的某个值

	CameraSetExposure(this->index,this->parameter.Exposure);//设置曝光值
	CameraSetGain(this->index, this->parameter.Gain);//设置增益
	// CameraSetWBGain(this->index,this->parameter.rg,this->parameter.gg,this->parameter.bg);//设置白平衡RGB增益值
	// CameraSetAdvancedGain(this->index,this->parameter.AdvancedGain);//设置高级增益
	// CameraSetDenoise(this->index,this->parameter.Denoise);//设置自动软件降噪
	
	// CameraSetAEC(this->index,this->parameter.AEC);//设置自动曝光
	//若启动自动曝光，则启动防闪烁模式
	// if (this->parameter.AEC)
	// {
	// 	this->parameter.AntiFlicker=true;
	// 	CameraSetAntiFlicker(this->index,this->parameter.AntiFlicker);
	// }
	// CameraSetAGC(this->index,this->parameter.AGC);//设置自动增益
	// CameraSetAWB(this->index,this->parameter.AWB);//设置自动白平衡

	// CameraSetGamma(this->index,this->parameter.Gamma);//设置伽马值
	// CameraSetContrast(this->index,this->parameter.ConTrast);//设置对比度
	// CameraSetSaturation(this->index,this->parameter.Saturation);//设置饱和度
	// CameraSetBlackLevel(this->index,this->parameter.BlackLevel);//设置黑电平
	// CameraSetLineHeight(this->index,this->parameter.LineHeight);//设置线扫模式
	// CameraSetLineTrigger(this->index,this->parameter.LineTrigger);//设置行触发方式

	this->setBrightness();
}

void Camera::setBrightness()
{
	int i = 0;
	
	while(true)
	{
		if(this->openVideo() == API_OK)//获取测试图像
		{
			cv::Mat frame_test(this->parameter.height,this->parameter.width,CV_8UC3,this->imgbuf);//复制图像
			cv::cvtColor(frame_test,frame_test,CV_BGR2GRAY);//转为灰度图

			cv::Scalar mean_test = cv::mean(frame_test);//计算图像灰度均值
			float mean = mean_test[0] - this->parameter.refer;//得到灰度图平均灰度值的偏移均值

			/*计算偏移均值的平均偏差*/
			int rows = frame_test.rows;
			int cols = frame_test.cols;
			int sum_temp = 0;//偏移总量
			int valid_pixel = 0;//有效像素点数

			for(int i = ((int)rows * 0.33); i < rows; i++)
			{
				uchar *p = frame_test.ptr<uchar>(i);//获取该图像每行的首元素指针
				for(int j = 0;j < cols; j++)
				{
					int diff_temp = p[j] - this->parameter.refer;//计算像素点与参考值的偏差
					int abs_temp = abs(diff_temp - mean);//计算| x - x平均 |
					sum_temp += abs_temp;//累计偏差
					valid_pixel++;//更新有效像素点数
				}
			}

			float meanDev = (float)sum_temp / (float)valid_pixel;//计算简单平均偏差	

			if(meanDev < abs(mean))
			{
				if(mean > 0)//过亮
				{
					this->parameter.Gain--;
					CameraSetGain(this->index,this->parameter.Gain);
				}
				else if(mean < 0)//过暗
				{
					this->parameter.Gain++;
					CameraSetGain(this->index,this->parameter.Gain);
				}
				else
				{
					break;
				}
				
			}
			else
			{
				break;
			}
		}
		if(i++ >= 20)
		{
			break;
		}
	}

}

void Camera::cameraInformation()
{
    int count;
	CameraGetResolutionCount(this->index, &count);
	std::cout << this->index << "分辨率：" << count << std::endl;
	int width;
	int height;
	CameraGetResolutionMax(this->index, &width, &height);
	std::cout << this->index << "尺寸：" << width << "*" << height << std::endl;
	CameraGetGain(this->index, &count);
	std::cout << this->index << "相机增益：" << count << std::endl;
	CameraGetExposure(this->index, &count);
	std::cout << this->index << "曝光值：" << count << std::endl;
	double exposure_time;
	CameraGetExposureUnit(this->index, &exposure_time);
	std::cout << this->index << "每单位曝光时间（ms)：" << exposure_time << std::endl;
	std::cout << this->index << "曝光时间(ms)：" << exposure_time * count << std::endl;
	CameraGetAETarget(this->index, &count);
	std::cout << this->index << "自动曝光目标值：" << count << std::endl;
	CameraGetSaturation(this->index, &exposure_time);
	std::cout << this->index << "颜色饱和度：" << count << std::endl;
	system("pause");
	system("cls");
}

int Camera::openVideo()
{
    return CameraQueryImage(this->index, this->imgbuf, &this->length, CAMERA_IMAGE_BMP);
}

void Camera::debugInformation()
{
	std::cout<< "Camera index: " << this->index << std::endl;
}