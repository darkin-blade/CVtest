/**
  * 线段匹配, 用不了
  */

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <opencv2/line_descriptor/descriptor.hpp>
#include <opencv2/ximgproc/fast_line_detector.hpp>

#include <iostream>
#include <vector>

using namespace std;
using namespace cv;
using namespace cv::detail;
using namespace cv::line_descriptor;
using namespace cv::ximgproc;

#define LOG(format, ...) \
  printf("\033[1;32m[%s, %d]" format "\33[0m\n", __func__, __LINE__, ## __VA_ARGS__)

const int MATCHES_DIST_THRESHOLD = 25;

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
    LOG("can't save img");
  }
}

int main() {
  Mat img1 = imread("../1.jpg");
  Mat img2 = imread("../2.jpg");
  Mat mask1 = Mat::ones(img1.size(), CV_8UC1);
  Mat mask2 = Mat::ones(img2.size(), CV_8UC1);

  Ptr<BinaryDescriptor> bd = BinaryDescriptor::createBinaryDescriptor();

  /***********************************/

  Ptr<FastLineDetector> fld = createFastLineDetector(10, 1.41421356, 50.0, 50.0, 3, false);

  vector<KeyLine> keylines1, keylines2;
  Mat descriptor1, descriptor2;

  fld->detect(img1, keylines1);
  fld->detect(img2, keylines2);

  bd->compute(img1, keylines1, descriptor1);
  bd->compute(img2, keylines2, descriptor2);

  vector<KeyLine> octave1, octave2;
  Mat left, right;
  for (int i = 0; i < keylines1.size(); i ++) {
    if (keylines1[i].octave == 0) {
      octave1.emplace_back(keylines1[i]);
      left.push_back(descriptor1.row(i));
    }
  }
  for (int i = 0; i < keylines2.size(); i ++) {
    if (keylines2[i].octave == 0) {
      octave2.emplace_back(keylines2[i]);
      right.push_back(descriptor2.row(i));
    }
  }

  Ptr<BinaryDescriptorMatcher> bdm = BinaryDescriptorMatcher::createBinaryDescriptorMatcher();

  vector<DMatch> matches;
  bdm->match(left, right, matches);

  vector<DMatch> good_matches;
  for (int i = 0; i < matches.size(); i ++) {
    if (matches[i].distance < MATCHES_DIST_THRESHOLD) {
      good_matches.emplace_back(matches[i]);
    }
  }

  Mat result;
  drawLineMatches(img1, octave1, img2, octave2, good_matches, result);

  show_img("result", result);
}