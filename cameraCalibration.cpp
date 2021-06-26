/**
 相机参数标定, 参考: https://blog.csdn.net/y459541195/article/details/104544956
 畸变校正
 **/

#include <math.h>

#include <iostream>
#include <vector>

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types_c.h>

using namespace cv;
using namespace std;

#define LOG(format, ...) \
  printf("\033[1;32m[%s, %d]" format "\33[0m\n", __func__, __LINE__, ## __VA_ARGS__)

void show_img(const char *window_name, Mat img) {
  namedWindow(window_name, WINDOW_NORMAL);
  resizeWindow(window_name, 1280, 720);
  imshow(window_name, img);
  waitKey(0);

  // 保存图片
  char img_name[128];
  int savable = 0;
  for (int i = 0; i < 100; i ++) {
    sprintf(img_name, "../result_%d.png", i);
    if (fopen(img_name, "r") == NULL) {
      savable = 1;
      break;
    }
  }
  if (savable) {
    imwrite(img_name, img);
  } else {
    LOG("can't save img");
  }
}

const int DOWN_SAMPLE_IMAGE_SIZE = 800 * 600;
const char app_path[64] = "../test_calibration";

char img_path[128];

int main() {
  vector<Mat> imageRGB;
  vector<Mat> imageGray;
  int imageNum = 11;

  for (int i = 1; i <= imageNum; i ++) {
    sprintf(img_path, "%s/%d.jpg", app_path, i);
    Mat tmp = imread(img_path);

    // 放缩
    if (false) {
      double imageSize = tmp.rows * tmp.cols;
      if (imageSize > DOWN_SAMPLE_IMAGE_SIZE) {
        double scale = sqrt(DOWN_SAMPLE_IMAGE_SIZE / imageSize);
        resize(tmp, tmp, Size(), scale, scale);
      }
    }
    imageRGB.emplace_back(tmp);
    cvtColor(tmp, tmp, COLOR_BGR2GRAY);
    imageGray.emplace_back(tmp);
  }

  int cornerSize[2]{8, 15};
  vector<Point3f> objectCorners;// 临时变量
  vector<Point2f> imageCorners;// 临时变量

  // 保存棋盘的世界坐标系坐标
  for (int i = 0; i < cornerSize[1]; i ++) {
    for (int j = 0; j < cornerSize[0]; j ++) {
      objectCorners.emplace_back(Point3f(j, i, 0));
    }
  }
  
  vector<vector<Point3f> > objectPoints;
  vector<vector<Point2f> > imagePoints;

  for (int i = 0; i < imageNum; i ++) {
    // 角点检测
    int result = findChessboardCorners(imageGray[i], Size(cornerSize[0], cornerSize[1]), imageCorners);

    // 提取亚像素角点
    TermCriteria criteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.001);
    cornerSubPix(imageGray[i], imageCorners, Size(11, 11), Size(-1, -1), criteria);
    // 保存每幅图像的角点
    objectPoints.emplace_back(objectCorners);
    imagePoints.emplace_back(imageCorners);
  }

  // 计算内参和畸变系数
  Mat cameraMatrix, distCoeffs, R, T;
  calibrateCamera(objectPoints, imagePoints, imageGray[0].size(), cameraMatrix, distCoeffs, R, T);
  cout << cameraMatrix << endl;
  cout << distCoeffs << endl;
  // for (int i = 0; i < cameraMatrix.rows; i ++) {
  //   for (int j = 0; j < cameraMatrix.cols; j ++) {
  //     printf("%lf %c", cameraMatrix.at<double>(i, j), j == (cameraMatrix.rows - 1) ? '\n' : ' ' );
  //   }
  // }

  for (int i = 0; i < 4; i ++) {
    Mat srcImg, dstImg;
    sprintf(img_path, "../%d.jpg", i + 1);
    srcImg = imread(img_path);
    cout << srcImg.size << endl;
    undistort(srcImg, dstImg, cameraMatrix, distCoeffs);
    show_img("result", dstImg);
  }
}
