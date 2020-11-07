/**
 *相机参数标定, 参考: https://blog.csdn.net/y459541195/article/details/104544956
 **/

#include <stdio.h>
#include <math.h>

#include <vector>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

const int DOWN_SAMPLE_IMAGE_SIZE = 800 * 600;
const char app_path[64] = "./test_calibration";

char img_path[128];

int main() {
  vector<Mat> originImages;

  for (int i = 1; i <= 11; i ++) {
    sprintf(img_path, "%s/%d.jpg", app_path, i);
    Mat tmp = imread(img_path);

    // 放缩
    double imageSize = tmp.rows * tmp.cols;
    if (imageSize > DOWN_SAMPLE_IMAGE_SIZE) {
      double scale = sqrt(DOWN_SAMPLE_IMAGE_SIZE / imageSize);
      resize(tmp, tmp, Size(), scale, scale);
    }
    originImages.emplace_back(tmp);
  }
}
