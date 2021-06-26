#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

#define LOG(format, ...) \
  printf("\033[1;32m[%s, %d]" format "\33[0m\n", __func__, __LINE__, ## __VA_ARGS__)

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

int main(int argc, char** argv)
{
    Mat src, dst, color_dst;
    const char* default_file = "../5.jpg";
    const char* filename = argc >=2 ? argv[1] : default_file;
    // Loads an image
    src = imread( filename, IMREAD_GRAYSCALE );
    GaussianBlur(src, src, Size(25, 25), 5);
    show_img( "Source", src );

    /* 缩放 */
    float origin_size = src.rows * src.cols;
    int DOWN_SAMPLE_IMAGE_SIZE = 600 * 800;
    if (origin_size > DOWN_SAMPLE_IMAGE_SIZE) {
      float scale = sqrt(DOWN_SAMPLE_IMAGE_SIZE / origin_size);
      resize(src, src, Size(), scale, scale);
    }

    Canny( 
      src, 
      dst, 
      50, 
      50, 
      3);
    cvtColor( dst, color_dst, COLOR_GRAY2BGR );
    vector<Vec4i> lines;
    HoughLinesP( 
      dst,
      lines, 
      1, // 距离精度: 1
      CV_PI/180, 
      200, // 阈值: 80
      50, // 长度: 30
      50);// 间隙: 10
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( color_dst, Point(lines[i][0], lines[i][1]),
        Point( lines[i][2], lines[i][3]), Scalar(0,0,255), 2, 8 );
    }
    show_img( "Detected Lines", color_dst );
    return 0;
}