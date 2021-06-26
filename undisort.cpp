/* 
畸变校正, 参考: https://blog.csdn.net/billbliss/article/details/52527182
*/

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

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

int main() {
  Mat K = Mat::zeros(3, 3, CV_64FC1);
  K.at<double>(0, 0) = 3166;// fx
  K.at<double>(0, 1) = 0; 
  K.at<double>(0, 2) = 1521;// cx
  K.at<double>(1, 0) = 0; 
  K.at<double>(1, 1) = 3151;// fy
  K.at<double>(1, 2) = 2000;// cy
  K.at<double>(2, 0) = 0; 
  K.at<double>(2, 1) = 0; 
  K.at<double>(2, 2) = 1; 
  Mat src, dst;
  src = imread("../4.jpg");
  vector<double> distCoeffs;

  distCoeffs.emplace_back(0.05465694372885115);
  distCoeffs.emplace_back(0.198208668072882);
  distCoeffs.emplace_back(0.002225091498174759);
  distCoeffs.emplace_back(0.004193335821950941);
  distCoeffs.emplace_back(-0.7520806463166811);

  /* 默认 */
  // undistort(src, dst, K, distCoeffs);

  /* 最大图 */
  // TODO

  /* 全图 */
  Mat map1, map2;
  initUndistortRectifyMap(K, distCoeffs, Mat(), Mat(), Size(2976, 3968), CV_16SC2, map1, map2);
  remap(src, dst, map1, map2, INTER_LINEAR);
  show_img("dst", dst);
}
