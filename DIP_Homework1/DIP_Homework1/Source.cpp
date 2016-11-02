#include <iostream>
#include <opencv\cv.h>
#include <opencv2\opencv.hpp>
#include <math.h>

//以結構型態處存影像計算後的結果
struct imgnum {
	float mean;           //平均值
	float std_dev;      //標準差
	float m;                  //標準誤差
	float p;                   //峰值信噪比
}typedef imgnum;

using namespace cv;
using namespace std;

float Mean(Mat img);             //計算平均值
float  Std_deviation(Mat img, float mean);              //計算標準差
float PSNRcount(float msenum);              //計算峰值信噪比計算
float MSEcount(Mat img_pure, Mat img_noisy);

int main(int argc, char** argv)
{
	Mat imgLenna, imgP[4], Lroi[4];
	imgnum roi[4], proi[4];

	//原始影像讀取
	imgLenna = imread("Lenna.bmp", 0);
	imgP[0] = imread("Pattern1.bmp", 0);
	imgP[1] = imread("Pattern2.bmp", 0);
	imgP[2] = imread("Pattern3.bmp", 0);
	imgP[3] = imread("Pattern4.bmp", 0);
	printf("%d", (int)(8.0 + 0.5));
	//檢查影像是否讀取
	for (int i = 0; i < 4; i++)
	{
		if (imgP[i].empty())
		{
			cout << "Pattern"<<i+1<< ".bmp Could not open or find the image" << endl;
			system("pause");
			return -1;
		}
	}
	if (imgLenna.empty())
	{
		cout << "Lenna.bmp Could not open or find the image" << endl;
		system("pause");
		return -1;
	}
	Mat roiPattern[4];

	//存取Lenna.bmp的ROI
	Lroi[0] = imgLenna(Rect(50,10 , 50, 50));
	Lroi[1] = imgLenna(Rect(70,330 , 50, 50));
	Lroi[2] = imgLenna(Rect(240, 250, 50, 50));
	Lroi[3] = imgLenna(Rect(300, 440, 50, 50));

	for (int i = 0; i<4; i++)
	{
		roiPattern[i] = imgP[i](Rect(100, 100, 50, 50));   //存取Pattern1~4.bmp的ROI
	}
	for (int i = 0; i<4; i++)
	{
		proi[i].mean = Mean(roiPattern[i]);   //計算每個pattern的roi的平均值
		proi[i].std_dev = Std_deviation(roiPattern[i], proi[i].mean);   //計算每個pattern的roi的標準差
	}
	for (int i = 0; i<4; i++)
	{
		roi[i].mean = Mean(Lroi[i]);   //計算Lenna每個ROI的平均值
		roi[i].std_dev = Std_deviation(Lroi[i], roi[i].mean);   //計算Lenne每個ROI的標準差
	}
	printf("Part1\nLenna\n");
	for (int i = 0; i<4; i++)
	{
		printf("roi_%d  mean:%.2f	std_dev:%.2lf\n", i + 1, roi[i].mean, roi[i].std_dev);
	}

	for (int i = 0; i<4; i++)
	{
		printf("Pattern%d\n", i + 1);
		printf("roi  mean:%.2f	std_dev:%.2f\n", proi[i].mean, proi[i].std_dev);
	}
	for (int i = 1; i<4; i++)
	{
		proi[i].m = MSEcount(imgP[0], imgP[i]);   //計算mse
		proi[i].p = PSNRcount(proi[i].m);   //計算psnr
	}
	printf("Part2\n");
	for (int i = 1; i<4; i++)
	{
		printf("Pattern_%d  mse:%.2f  psnr:%.2f\n", i + 1, proi[i].m, proi[i].p);
	}
	//將圖片輸出
	namedWindow("Image", CV_WINDOW_AUTOSIZE);
	namedWindow("ROI", CV_WINDOW_AUTOSIZE);
	namedWindow("ROI2", CV_WINDOW_AUTOSIZE);
	namedWindow("ROI3", CV_WINDOW_AUTOSIZE);
	namedWindow("ROI4", CV_WINDOW_AUTOSIZE);
	//視窗偏移之位移量
	int pox = 900;
	int poy = 50;

	cvMoveWindow("Image", pox, poy);
	cvMoveWindow("ROI", 50 + pox, 10 + poy);
	cvMoveWindow("ROI2", 70 + pox, 330 + poy);
	cvMoveWindow("ROI3", 240 + pox, 250 + poy);
	cvMoveWindow("ROI4", 300 + pox, 440 + poy);

	imshow("Image", imgLenna);
	imshow("ROI", Lroi[0]);
	imshow("ROI2", Lroi[1]);
	imshow("ROI3", Lroi[2]);
	imshow("ROI4", Lroi[3]);

	//影像存檔
	imwrite("LennaROI1.bmp", Lroi[0]);
	imwrite("LennaROI2.bmp", Lroi[1]);
	imwrite("LennaROI3.bmp", Lroi[2]);
	imwrite("LennaROI4.bmp", Lroi[3]);
	imwrite("Pattern1ROI.bmp", roiPattern[0]);
	imwrite("Pattern2ROI.bmp", roiPattern[1]);
	imwrite("Pattern3ROI.bmp", roiPattern[2]);
	imwrite("Pattern4ROI.bmp", roiPattern[3]);
	
	waitKey(0);
	return 0;
}

float Mean(Mat img)
{
	float total = 0;

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			total += img.at<uchar>(i, j);//依序存取每個像素值
		}
	}
	return total / img.total();//計算像素平均並回傳
}

float  Std_deviation(Mat img, float mean)
{
	float  std_dev = 0, tmp = 0;

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			tmp += pow((img.at<uchar>(i, j) - mean), 2);//計算離均差的平方
		}
	}
	return sqrt(tmp / (img.total() - 1));//回傳標準差
}

float MSEcount(Mat img_pure, Mat img_noisy)
{
	float mse = 0;
	for (int i = 0; i<img_pure.rows; i++)
	{
		for (int j = 0; j<img_pure.cols; j++)
		{
			mse += pow(img_noisy.at<uchar>(i, j) -img_pure.at<uchar>(i, j), 2);//計算每個像素差並平方
		}
	}
	return mse / img_pure.total();//回傳標準誤差值(MSE)
}

float PSNRcount(float msenum)
{
	return 10 * log10((255 * 255) / msenum);//回傳峰值信噪比(PSNR)
}


