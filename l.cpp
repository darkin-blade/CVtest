/**
  * 图像分割
  */

#include <iostream>

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

#define GREEN(format, ...) \
  printf("\033[1;32m[%s, %d]" format "\33[0m\n", __func__, __LINE__, ## __VA_ARGS__)

using namespace cv;
using namespace cv::detail;
using namespace std;

void show_img(const char *window_name, Mat img);

Vec3b RandomColor(int value);  //生成随机颜色函数
 
int main( int argc, char* argv[] )
{
  // 读取图像
	Mat image = imread("1.jpg");
 
	// 图像灰度化
	Mat imageGray;
	cvtColor(image, imageGray, CV_RGB2GRAY);
  // TODO 高斯滤波
	GaussianBlur(imageGray, imageGray, Size(5, 5), 2);
	imshow("Gray Image", imageGray); 
  // 边缘检测
	Canny(imageGray, imageGray, 80, 150);  
	imshow("Canny Image", imageGray);
 
	// 轮廓检测
	vector<vector<Point> > contours;  
	vector<Vec4i> hierarchy;  
	findContours(imageGray, contours, hierarchy, RETR_TREE,CHAIN_APPROX_SIMPLE, Point());  
	Mat marks(image.size(),CV_32S);// 分水岭算法第二个矩阵参数
	marks = Scalar::all(0);
  // 轮廓标记
	for(int index = 0, compCount = 0; index >= 0; compCount ++ ) 
	{
		// 设置注水点, 有多少轮廓, 就有多少注水点
		drawContours(marks, contours, index, Scalar::all(compCount + 1), 1, 8, hierarchy);
    index = hierarchy[index][0];
	}
 
	// 查看分水岭算法的矩阵参数
	Mat marksShows;
	convertScaleAbs(marks, marksShows);
	imshow("marksShow", marksShows);

 
	// 分水岭算法之后的矩阵marks
	watershed(image, marks);
	Mat afterWatershed;
	convertScaleAbs(marks, afterWatershed);
	imshow("After Watershed", afterWatershed);
 
	//对每一个区域进行颜色填充
	Mat PerspectiveImage = Mat::zeros(image.size(), CV_8UC3);
	for(int i = 0; i < marks.rows; i ++) {
		for(int j = 0; j < marks.cols; j ++) {
			int index = marks.at<int>(i,j);
			if (marks.at<int>(i,j) == -1) {
        // TODO
				PerspectiveImage.at<Vec3b>(i,j) = Vec3b(255,255,255);
			} else {
				PerspectiveImage.at<Vec3b>(i,j) = RandomColor(index);
			}
		}
	}
	show_img("After ColorFill", PerspectiveImage);
 
	// 分割并将填充颜色的结果跟原始图像融合
	// Mat wshed;
	// addWeighted(image, 0.4, PerspectiveImage, 0.6, 0, wshed);
	// imshow("AddWeighted Image", wshed);
}
 
Vec3b RandomColor(int value) {
	value = value % 255;
	RNG rng;// 随机化种子
	int aa = rng.uniform(0, value);
	int bb = rng.uniform(0, value);
	int cc = rng.uniform(0, value);
	return Vec3b(aa, bb, cc);
}

void show_img(const char *window_name, Mat img) {
  namedWindow(window_name, WINDOW_NORMAL);
  resizeWindow(window_name, 1280, 720);
  imshow(window_name, img);
  waitKey(0);

  // 保存图片
  char img_name[128];
  int savable = 0;
  for (int i = 0; i < 100; i ++) {
    sprintf(img_name, "./result_%d.png", i);
    if (fopen(img_name, "r") == NULL) {
      savable = 1;
      break;
    }
  }
  if (savable) {
    imwrite(img_name, img);
  } else {
    GREEN("can't save img");
  }
}