#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;


#define LOG(format, ...) \
  printf("\033[1;36m" format "\33[0m\n", ## __VA_ARGS__)

char img_path[128];
char img_name[64];

void show_img(const char *window_name, Mat img);

int main() {
  for (int i = 1; i <= 99; i ++) {
    // sprintf(img_path, "../%d.jpg", i);
    scanf("%s", img_name);
    sprintf(img_path, "../%s", img_name);
    Mat img = imread(img_path);
    float origin_size = img.rows * img.cols;
    float scale = sqrt(600 * 800 / origin_size);
    resize(img, img, Size(), scale, scale);
    if (img.channels() == 3) {
      cvtColor(img, img, COLOR_RGB2RGBA);
    }
    show_img("img", img);
  }
}

void show_img(const char *window_name, Mat img) {
  if (img.rows * img.cols <= 0) {
    LOG("invalid img %s", window_name);
    return;
  }

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
