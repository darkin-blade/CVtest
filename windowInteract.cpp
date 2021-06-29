/*
  用户window交互测试
*/

#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

using namespace cv;
using namespace std;

Mat img;
Point prevPt(-1, -1);
void onMouse(int event, int x, int y, int flags, void *)
{
  if (x < 0 || x >= img.cols || y < 0 || y >= img.rows) {
    return;
  }
  if (event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON)) {
    /* 左键抬起 || 没有操作 */
    // TODO: BUG: 没有操作
    prevPt = Point(-1, -1);
  } else if (event == EVENT_LBUTTONDOWN) {
    /* 按下鼠标左键 */
    cout << x << " " << y << endl;
    prevPt = Point(x, y);
  } else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON)) {
    /* 按下左键后鼠标移动 */
    Point pt(x, y);
    if (prevPt.x < 0) {
      prevPt = pt;
    }
    line(img, prevPt, pt, Scalar(255, 0, 0, 255), 5, 8, 0);
    prevPt = pt;
    imshow("draw", img);
  }
}

int main() {
  img = imread("../1.jpg");
  namedWindow("draw", WINDOW_NORMAL);
  imshow("draw", img);
  resizeWindow("draw", 1280, 720);
  setMouseCallback("draw", onMouse, 0);
  while (1) {
    /* 监听按键 */
    char c = (char)waitKey(0);
    cout << c << endl;
  }
}