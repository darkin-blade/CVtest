#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

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

  vl_sift_pix *sift_img1 = new vl_sift_pix[img1.cols * img1.rows];

  VlSiftFilt *SiftFilt = NULL;
  SiftFilt = vl_sift_new(img1.cols, img1.rows,
      4,// noctaves
      2,// nlevels
      0);// o_min
  if (vl_sift_process_first_octave(SiftFilt, sift_img1) != VL_ERR_EOF) {
    do {
      vl_sift_detect(SiftFilt);
      GREEN("total key points %d", SiftFilt->nkeys);
    } while (vl_sift_process_next_octave(SiftFilt) != VL_ERR_EOF);
  }
  vl_sift_delete(SiftFilt);
  namedWindow("vl sift", WINDOW_NORMAL);
  imshow("vl sift", img1);
  waitKey(0);
  GREEN("finished");
}
