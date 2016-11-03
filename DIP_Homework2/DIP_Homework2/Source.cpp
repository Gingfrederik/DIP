#include <iostream>
#include <opencv\cv.h>
#include <opencv2\opencv.hpp>
#include <cmath>

using namespace cv;
using namespace std;


//twirl effect
void twirl(Mat &img, int K, Mat &map_x, Mat &map_y)
{
	//xc,yc為中心坐標點
	double xc = img.rows / 2;
	double yc = img.cols / 2;
	double pp;
	double r, a;
	double dy, dx;

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			dx = i - xc;
			dy = j - yc;

			r = sqrt(dx*dx + dy*dy);
			//以atan2取代atan 解決角度問題
			a = atan2(dy, dx);
			if(K==0)
			    pp = a;
			else
			    pp = a + r / K;

			map_x.at<float>(i, j) = r*cos(pp) + xc;
			map_y.at<float>(i, j) = r*sin(pp) + yc;
		}
	}
}

//fisheye effect
void fisheyexxx(Mat &img, Mat &map_x, Mat &map_y)
{
	//xc,yc為中心坐標點
	double xc = img.rows /2;
	double yc = img.cols /2;
	double maxr;
	double rp, r, a;
	double  dy, dx;
	maxr = sqrt(xc*xc + yc*yc);

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			dx = i - xc;
			dy = j - yc;

			r = sqrt(dx*dx + dy*dy);
			//以atan2取代atan 解決角度問題
			if (dx == 0)
				a = atan2(dy, 1);
			else
				a = atan2(dy, dx);

			rp = (r*r) / maxr;

			map_x.at<float>(i, j) = rp*cos(a) + xc;
			map_y.at<float>(i, j) = rp*sin(a) + yc;
		}
	}
}

//mapping using bilinear interpolation
void mapping_bilinear(Mat &map_x, Mat &map_y, Mat &img, Mat &out)
{
	int width = img.cols;
	int height = img.rows;

	//Gray-level
	if (img.channels() == 1)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				float new_x = map_x.at<float>(i, j);
				float new_y = map_y.at<float>(i, j);

				//四捨五入
				new_x += 0.5;
				new_y += 0.5;
				
				//邊界偵測
				if (new_x<0)         new_x = 0;
				if (new_x > height - 1)  new_x = height - 2;
				if (new_y<0)         new_y = 0;
				if (new_y > width - 1) new_y = width - 2;

				//取整數
				int x1 = (int)(new_x);
				int y1 = (int)(new_y);

				float u, t;
				t = new_x - x1;
				u = new_y - y1;

				out.at<uchar>(i, j) = (1 - u)*(1 - t)*img.at<uchar>(x1, y1) +
					(u)*(1 - t)*img.at<uchar>(x1, y1 + 1) +
					(1 - u)*(t)*img.at<uchar>(x1 + 1, y1) +
					(u)*(t)*img.at<uchar>(x1 + 1, y1 + 1);
			}
		}

	}
	//for rgb
	else
	{
		for (int j = 0; j < img.cols; j++)
		{
			for (int i = 0; i < img.rows; i++)
			{
				float new_x = map_x.at<float>(i, j);
				float new_y = map_y.at<float>(i, j);

				//四捨五入
				new_x += 0.5;
				new_y += 0.5;
				
				//邊界偵測
				if (new_x<0)         new_x = 0;
				if (new_x > height - 1)  new_x = height - 2;
				if (new_y<0)         new_y = 0;
				if (new_y > width - 1) new_y = width - 2;

				//取整數
				int x1 = (int)(new_x);
				int y1 = (int)(new_y);

				float u, t;
				t = new_x - x1;
				u = new_y - y1;

				for (int k = 0; k<3; k++)
				{
					out.at<Vec3b>(i, j)[k] = (1 - u)*(1 - t)*img.at<Vec3b>(x1, y1)[k] +
						(u)*(1 - t)*img.at<Vec3b>(x1, y1 + 1)[k] +
						(1 - u)*(t)*img.at<Vec3b>(x1 + 1, y1)[k] +
						(u)*(t)*img.at<Vec3b>(x1 + 1, y1 + 1)[k];
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	Mat img, dst,dst2;
	Mat map_x, map_y;
	Mat map_x2, map_y2;
	int k;
	img = imread(argv[1], -1);
	printf("Input twirl amount k : ");
	scanf_s("%d", &k);


	if (img.empty())
	{
		cout << "Could not open or find the image" << endl;
		return -1;
	}

	//dst, dst2 儲存變更後的照片
	dst.create(img.size(), img.type());
	dst2.create(img.size(), img.type());
	//map_x, map_y 記錄變換後的坐標點
	map_x.create(img.size(), CV_32FC1);
	map_y.create(img.size(), CV_32FC1);

	fisheyexxx(img,map_x,map_y);
	mapping_bilinear(map_x, map_y, img, dst);
	imwrite("fisheye_out.bmp", dst);

	twirl(img, k, map_x, map_y);
	mapping_bilinear(map_x, map_y, img, dst2);
	imwrite("twirl_out.bmp", dst2);

	namedWindow("orignal", CV_WINDOW_AUTOSIZE);
	imshow("orignal", img);
	namedWindow("twirl", CV_WINDOW_AUTOSIZE);
	imshow("twirl", dst2);
	namedWindow("fisheye", CV_WINDOW_AUTOSIZE);
	imshow("fisheye", dst);

	waitKey(0);
	return 0;
}

