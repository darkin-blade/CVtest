/***
  * Mat类型转换
  **/

#include <vector>
#include <stdio.h>
#include <math.h>

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/stitching.hpp>


#define LOG(format, ...) \
  printf("\033[1;36m" format "\33[0m\n", ## __VA_ARGS__)

using namespace cv;
using namespace cv::detail;
using namespace std;

int main() {
  Mat a = Mat(1, 1, CV_8UC1, Scalar::all(255));
  LOG("%d", a.at<uchar>(1, 1));
  a.convertTo(a, CV_32F);
  LOG("%f", a.at<float>(0, 0));
  LOG("%d %d", a.rows, a.cols);
}