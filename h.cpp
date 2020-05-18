#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
}

#define GREEN(format, ...) \
  printf("\033[1;32m[%s, %d]" format "\33[0m\n", __func__, __LINE__, ## __VA_ARGS__)

using namespace cv;
using namespace std;

int main() {
  Mat img1 = imread("1.jpg");
  Mat img2 = imread("2.jpg");

  // 保存图像数据
  vl_sift_pix *sift_img1 = new vl_sift_pix[img1.cols * img1.rows];
  for (int i = 0, k = 0; i < img1.rows; i ++) {
    uint8_t *p = img1.ptr<uint8_t>(i);
    for (int j = 0; j < img1.cols; j ++) {
      sift_img1[k + j] = p[j];
    }
    k += img1.cols;
  }

  VlSiftFilt *SiftFilt = NULL;
  SiftFilt = vl_sift_new(img1.cols, img1.rows,
      4,// noctaves
      2,// nlevels
      0);// o_min
  if (vl_sift_process_first_octave(SiftFilt, sift_img1) != VL_ERR_EOF) {
    do {
      vl_sift_detect(SiftFilt);
      GREEN("cur key points %d", SiftFilt->nkeys);
      
      // 描绘特征点
      VlSiftKeypoint *key_point = SiftFilt->keys;
      for (int i = 0; i < SiftFilt->nkeys; i ++) {
        VlSiftKeypoint cur_point = *key_point;
        key_point ++;
        Scalar color(0, 0, 255);
        circle(img1, Point2f(cur_point.x, cur_point.y), 3, color, -1);
      }
    } while (vl_sift_process_next_octave(SiftFilt) != VL_ERR_EOF);
  }
  vl_sift_delete(SiftFilt);
  namedWindow("vl sift", WINDOW_AUTOSIZE);
  imshow("vl sift", img1);
  waitKey(0);
  GREEN("finished");
}
