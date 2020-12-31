/**
  * 单纯的寻找接缝线
  */

#include <vector>
#include <stdio.h>
#include <math.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/stitching/detail/seam_finders.hpp>

#define LOG(format, ...) \
  printf("\033[1;36m" format "\33[0m\n", ## __VA_ARGS__)

using namespace cv;
using namespace cv::detail;
using namespace std;

void show_img(const char *window_name, Mat img);
void show_img(const char *window_name, UMat img);
Size2f normalizeVertices(vector<vector<Point2f> > & vertices);

double my_min(double a, double b) {
  if (a < b) return a;
  return b;
}
double my_max(double a, double b) {
  if (a > b) return a;
  return b;
}

int main() {
  Ptr<SeamFinder> seam_finder;// 接缝线寻找器
  if (false) {
    seam_finder = makePtr<detail::VoronoiSeamFinder>();// 逐点法
  } else if (true) {
    seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR);// 默认: 图割法
  } else if (false) {
    seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR_GRAD);
  } else if (false) {
    seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR);// 动态规划法
  } else if (false) {
    seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR_GRAD);
  } else {
    assert(0);
  }

  int img_num = 2;

  vector<Point2i> origins(img_num);
  vector<vector<Point2f> > corners(img_num);
  vector<Mat> images(img_num);
  vector<UMat> masks(img_num);
  vector<UMat> images_f(img_num);

  images[0] = imread("../1.jpg");
  images[1] = imread("../2.jpg");
  origins[0] = Point2f(0, 0);
  origins[1] = Point2f(95, 107);

  for (int i = 0; i < img_num; i ++) {// 图像数据类型转换
    images[i].convertTo(images_f[i], CV_32F);
    masks[i] = UMat(images[i].size(), CV_8UC1, Scalar::all(255));

    corners[i].emplace_back(Point2i(0, 0) + origins[i]);
    corners[i].emplace_back(Point2i(images[i].cols, 0) + origins[i]);
    corners[i].emplace_back(Point2i(images[i].cols, images[i].rows) + origins[i]);
    corners[i].emplace_back(Point2i(0, images[i].rows) + origins[i]);
  }

  // 得到接缝线的掩码图像
  seam_finder->find(images_f, origins, masks); 
  for (int i = 0; i < img_num; i ++) {
    // show_img("mask", masks[i]);
  }

  Size2f pano_size = normalizeVertices(corners);
  Mat result = Mat::zeros(pano_size, CV_8UC3);

  Mat dst;
  dst = Mat(result, Rect(corners[0][0].x, corners[0][0].y, images[0].cols, images[0].rows));
  images[0].copyTo(dst, masks[0]);
  dst = Mat(result, Rect(corners[1][0].x, corners[1][0].y, images[1].cols, images[1].rows));
  images[1].copyTo(dst, masks[1]);

  show_img("result", result);
}

void show_img(const char *window_name, UMat img) {
  Mat tmp;
  img.copyTo(tmp);
  show_img(window_name, tmp);
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

Size2f normalizeVertices(vector<vector<Point2f> > & vertices) {
  double min_x = std::numeric_limits<double>::max(), max_x = -std::numeric_limits<double>::max();
  double min_y = std::numeric_limits<double>::max(), max_y = -std::numeric_limits<double>::max();
  for(int i = 0; i < vertices.size(); ++i) {
    for(int j = 0; j < vertices[i].size(); ++j) {
      min_x = my_min(min_x, vertices[i][j].x);
      min_y = my_min(min_y, vertices[i][j].y);
      max_x = my_max(max_x, vertices[i][j].x);
      max_y = my_max(max_y, vertices[i][j].y);
    }
  }
  for(int i = 0; i < vertices.size(); ++i) {
    for(int j = 0; j < vertices[i].size(); ++j) {
      vertices[i][j].x = (vertices[i][j].x - min_x);
      vertices[i][j].y = (vertices[i][j].y - min_y);
    }
  }
  return Size2f(max_x - min_x, max_y - min_y);
}