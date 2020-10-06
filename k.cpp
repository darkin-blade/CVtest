/**
  * 曝光补偿
  */

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/stitching/detail/exposure_compensate.hpp>

#include <iostream>
#include <vector>

#define GREEN(format, ...) \
  printf("\033[1;32m[%s, %d]" format "\33[0m\n", __func__, __LINE__, ## __VA_ARGS__)

using namespace cv;
using namespace cv::detail;
using namespace std;

void show_img(const char *window_name, Mat img);
int main() {
  char window_name[32];

  Mat img1 = imread("1.jpg");
  Mat img2 = imread("2.jpg");

  vector<Mat> images;
  vector<UMat> images_warped, masks_warped;
  vector<Point2i> corners;
  images.emplace_back(img1);
  images.emplace_back(img2);
  for (int i = 0; i < 2; i ++) {
    UMat tmp_img, tmp_mask;
    images[i].copyTo(tmp_img);
    images_warped.emplace_back(tmp_img);
    sprintf(window_name, "img%d", i);
    tmp_mask = UMat::zeros(tmp_img.size(), CV_8UC1);
    // tmp_mask = tmp_img.clone();
    tmp_mask.setTo(255);
    masks_warped.emplace_back(tmp_mask);
    cout << tmp_img.channels() << endl;
    cvtColor(tmp_img, tmp_img, COLOR_RGB2RGBA);
    cout << tmp_img.channels() << endl;
    show_img(window_name, tmp_img.getMat(ACCESS_RW));
  }
  // 曝光补偿
  Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(ExposureCompensator::GAIN);// 使用分块增益补偿
  // corners.emplace_back((0, 20));
  // corners.emplace_back((145, 0));
  corners.emplace_back((-15, 0));
  corners.emplace_back((130, -20));
  compensator->feed(corners, images_warped, masks_warped);
  for (int i = 0; i < 2; i ++) {
    // sprintf(window_name, "img%d", i);
    compensator->apply(i, corners[i], images_warped[i], masks_warped[i]);
    images_warped[i].copyTo(images[i]);
    show_img(window_name, images[i]);
  }
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