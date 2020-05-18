#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>

#include <math.h>
#include <stdio.h>

using namespace std;
using namespace cv;

void show_img(const char *window_name, Mat img);

const char* source_window = "Source image";;
const char* rotate_window = "Rotate+Resize";

int main( )
{
  Mat rot_mat( 2, 3, CV_32FC1 );
  Mat src, rotate_dst;

  /// 载入图像
  src = imread("2.jpg", IMREAD_UNCHANGED);
  if (src.channels() == 3) {
    cvtColor(src, src, CV_BGR2BGRA);
  }

  /** 图像旋转 */

  Point center = Point( src.cols/2, src.rows/2 );
  double angle = 35.0;
  double scale = 0.8;

  /// 根据以上参数得到旋转矩阵
  rot_mat = getRotationMatrix2D( center, angle, scale );

  //计算旋转后的画布大小，并将旋转中心平移到新的旋转中心
  Rect bbox = RotatedRect(center, Size(src.cols*scale, src.rows*scale), angle).boundingRect();
  rot_mat.at<double>(0, 2) += bbox.width / 2.0 - center.x;
  rot_mat.at<double>(1, 2) += bbox.height / 2.0 - center.y;

  /// 旋转图像
  warpAffine( src, rotate_dst, rot_mat, bbox.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(255, 0, 0, 0) );

  show_img( source_window, src );

  show_img( rotate_window, rotate_dst );
}

void show_img(const char *window_name, Mat img) {
  namedWindow(window_name, WINDOW_AUTOSIZE);
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
    assert(0);
  }
}
