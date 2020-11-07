/**
 * 显示一张图片
 **/

#include <opencv2/opencv.hpp>
using namespace cv;

// 显示一张图片
int main() {
  Mat src = imread("./img_1.png");
  imshow("test", src);
  waitKey(0);
  return 0;
}
