/**
 * 使用stitcher合并图片
 **/

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <vector>

#define CYAN(format, ...) \
  printf("\033[1;36m" format "\33[0m\n", ## __VA_ARGS__)

using namespace cv;
using namespace std;

#define UBUNTU
#if defined(UBUNTU)

int main(int argc, char** argv)
{
  std::vector<Mat> imgs;
  char img_name[64];
  Mat img;
  for (int i = 1; i <= 3; i ++) {
    sprintf(img_name, "%d.jpg", i);
    img = imread(img_name);
    imgs.push_back(img);
  }

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

#else

extern "C" JNIEXPORT int JNICALL
Java_com_example_niswgsp_11_MainActivity_main_1test(
    JNIEnv* env,
    jobject thiz,
    jobjectArray imgPaths,
    jlong matBGR) {
  total_env = env;

  // 获取String数组长度
  jsize str_len = env->GetArrayLength(imgPaths);

  // 读取图片
  Mat img_read;
  std::vector<Mat> imgs;
  for (int i = 0; i < str_len; i ++) {
    jstring tmp = (jstring) env->GetObjectArrayElement(imgPaths, i);
    const char *img_path = env->GetStringUTFChars(tmp, 0);
    img_read = imread(img_path);
    imgs.push_back(img_read);
  }

  Mat pano;
  Ptr<Stitcher> stitcher = Stitcher::create();
  Stitcher::Status status = stitcher->stitch(imgs, pano);

  if (status != Stitcher::OK) {
    LOG("failed");
    return 0;
  }
  
  *(Mat *)matBGR = pano.clone();

  return 0;
}

#endif