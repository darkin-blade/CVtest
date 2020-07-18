/**
 * 合并两张图片
 **/

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <vector>

#define CYAN(format, ...) \
  printf("\033[1;36m" format "\33[0m\n", ## __VA_ARGS__)

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
  std::vector<Mat> imgs;
  Mat img;
  img = imread("img1.png");
  imgs.push_back(img);
  img = imread("img2.png");
  imgs.push_back(img);

  Mat pano;
  Ptr<Stitcher> stitcher = Stitcher::create();
  Stitcher::Status status = stitcher->stitch(imgs, pano);

  if (status != Stitcher::OK) {
    CYAN("failed");
    return 0;
  }

  imshow("result", pano);
  waitKey(0);

  return 0;
}
