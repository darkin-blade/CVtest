/**
  * 检测图像质量 
  */

#include <opencv2/imgcodecs.hpp>
#include <opencv2/quality/qualitybrisque.hpp>

using namespace cv;

int main() {
  Mat pano = imread("1.jpg");

  Ptr<quality::QualityBRISQUE> brisque = quality::QualityBRISQUE::create();
  Scalar score = brisque->compute(pano);

  printf("%lf\n", score[0]);
}


void show_img(const char *window_name, Mat img) {
  // namedWindow(window_name, WINDOW_AUTOSIZE);
  // imshow(window_name, img);
  // waitKey(0);

  // // 保存图片
  // char img_name[128];
  // int savable = 0;
  // for (int i = 0; i < 100; i ++) {
  //   sprintf(img_name, "./result_%d.png", i);
  //   if (fopen(img_name, "r") == NULL) {
  //     savable = 1;
  //     break;
  //   }
  // }
  // if (savable) {
  //   imwrite(img_name, img);
  // } else {
  //   CYAN("can't save img");
  // }
}