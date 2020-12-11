#include <iostream>

#include <opencv2/videoio.hpp>

#define LOG(format, ...) \
  printf("\033[1;32m" format "\33[0m\n", ## __VA_ARGS__)

using namespace std;
using namespace cv;

string videoFile = "/home/lynx/study/stitch/orb_slam2/TestData/1.mp4";

int main() {
  VideoCapture cap;
  // cap.open(0);
  cap.open(videoFile);
  if (!cap.isOpened()) {
    LOG("can't load video");
    return 1;
  } else {
    LOG("success");
  }
}