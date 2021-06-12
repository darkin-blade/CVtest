/***
  * 图像融合
  **/

#include <iostream>
#include <fstream>
#include <string>
#include "opencv2/opencv_modules.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching/detail/autocalib.hpp"
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/timelapsers.hpp"
#include "opencv2/stitching/detail/camera.hpp"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/stitching/detail/matchers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/detail/seam_finders.hpp"
#include "opencv2/stitching/detail/warpers.hpp"
#include "opencv2/stitching/warpers.hpp"

using namespace std;

using namespace cv;
using namespace cv::detail;

#define LOG(format, ...) \
  printf("\033[1;36m" format "\33[0m\n", ## __VA_ARGS__)

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

void blend1();
void blend2();
void blend3();

class LaplacianBlending {
private:
  Mat leftImg;
  Mat rightImg;
  Mat blendMask;

  //Laplacian Pyramids
  vector<Mat> leftLapPyr, rightLapPyr, resultLapPyr;
  Mat leftHighestLevel, rightHighestLevel, resultHighestLevel;
  //mask为三通道方便矩阵相乘
  vector<Mat> maskGaussianPyramid;

  int levels;

  void buildPyramids()
  {
    buildLaplacianPyramid(leftImg, leftLapPyr, leftHighestLevel);
    buildLaplacianPyramid(rightImg, rightLapPyr, rightHighestLevel);
    buildGaussianPyramid();
  }

  void buildGaussianPyramid()
  {
    //金字塔内容为每一层的掩模
    assert(leftLapPyr.size()>0);

    maskGaussianPyramid.clear();
    Mat currentImg;
    cvtColor(blendMask, currentImg, COLOR_GRAY2BGR);
    //保存mask金字塔的每一层图像
    maskGaussianPyramid.push_back(currentImg); //0-level

    currentImg = blendMask;
    for (int l = 1; l<levels + 1; l++) {
      Mat _down;
      if (leftLapPyr.size() > l)
        pyrDown(currentImg, _down, leftLapPyr[l].size());
      else
        pyrDown(currentImg, _down, leftHighestLevel.size()); //lowest level

      Mat down;
      cvtColor(_down, down, COLOR_GRAY2BGR);
      //add color blend mask into mask Pyramid
      maskGaussianPyramid.push_back(down);
      string winName = to_string(l);
      imshow(winName,down);
      currentImg = _down;
    }
  }

  void buildLaplacianPyramid(const Mat& img, vector<Mat>& lapPyr, Mat& HighestLevel)
  {
    lapPyr.clear();
    Mat currentImg = img;
    for (int l = 0; l<levels; l++) {
      Mat down, up;
      pyrDown(currentImg, down);
      pyrUp(down, up, currentImg.size());
      Mat lap = currentImg - up;
      lapPyr.push_back(lap);
      currentImg = down;
    }
    currentImg.copyTo(HighestLevel);
  }

  Mat reconstructImgFromLapPyramid()
  {
    //将左右laplacian图像拼成的resultLapPyr金字塔中每一层
    //从上到下插值放大并与残差相加，即得blend图像结果
    Mat currentImg = resultHighestLevel;
    for (int l = levels - 1; l >= 0; l--)
    {
      Mat up;
      pyrUp(currentImg, up, resultLapPyr[l].size());
      currentImg = up + resultLapPyr[l];
    }
    return currentImg;
  }

  void blendLapPyrs()
  {
    //获得每层金字塔中直接用左右两图Laplacian变换拼成的图像resultLapPyr
    resultHighestLevel = leftHighestLevel.mul(maskGaussianPyramid.back()) +
      rightHighestLevel.mul(Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid.back());
    for (int l = 0; l<levels; l++)
    {
      Mat A = leftLapPyr[l].mul(maskGaussianPyramid[l]);
      Mat antiMask = Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid[l];
      Mat B = rightLapPyr[l].mul(antiMask);
      Mat blendedLevel = A + B;

      resultLapPyr.push_back(blendedLevel);
    }
  }

public:
  LaplacianBlending(const Mat& _left, const Mat& _right, const Mat& _blendMask, int _levels) ://construct function, used in LaplacianBlending lb(l,r,m,4);
    leftImg(_left), rightImg(_right), blendMask(_blendMask), levels(_levels)
  {
    assert(_left.size() == _right.size());
    assert(_left.size() == _blendMask.size());
    //创建拉普拉斯金字塔和高斯金字塔
    buildPyramids();
    //每层金字塔图像合并为一个
    blendLapPyrs();
  };

  Mat blend()
  {
    //重建拉普拉斯金字塔
    return reconstructImgFromLapPyramid();
  }
};

Mat LaplacianBlend(const Mat &left, const Mat &right, const Mat &mask)
{
  LaplacianBlending laplaceBlend(left, right, mask, 10);
  return laplaceBlend.blend();
}

int main() {
  blend1();
}

void blend3() {
  Mat leftImg = imread("../1.jpg");
  Mat rightImg = imread("../2.jpg");

  int hight = leftImg.rows;
  int width = leftImg.cols;

  Mat leftImg32f, rightImg32f;
  leftImg.convertTo(leftImg32f, CV_32F);
  rightImg.convertTo(rightImg32f, CV_32F);

  //创建用于混合的掩膜，这里在中间进行混合
  Mat mask = Mat::zeros(hight, width, CV_32FC1);
  mask(Range::all(), Range(0, mask.cols * 0.5)) = 1.0;

  Mat blendImg = LaplacianBlend(leftImg32f, rightImg32f, mask);

  blendImg.convertTo(blendImg, CV_8UC3);

  show_img("blended", blendImg);
}

void blend2() {
  vector<Mat> pano_images;
  vector<Mat> pano_masks;
  char img_path[128];
  char app_path[32] = "..";

  for (int i = 1; i <= 2; i ++) {
    sprintf(img_path, "%s/mask%d.png", app_path, i);
    Mat tmp_mask = imread(img_path);
    if (tmp_mask.channels() != 1) {
      cvtColor(tmp_mask, tmp_mask, COLOR_RGB2GRAY);
    }
    tmp_mask.convertTo(tmp_mask, CV_8UC1);
    pano_masks.emplace_back(tmp_mask);

    sprintf(img_path, "%s/img%d.png", app_path, i);
    Mat tmp_image = imread(img_path);
    pano_images.emplace_back(tmp_image);
  }

  Mat left_f, right_f;
  pano_images[0].convertTo(left_f, CV_32F);
  pano_images[1].convertTo(right_f, CV_32F);
  Mat left_mask, right_mask;
  pano_masks[0].convertTo(left_mask, CV_32F);
  pano_masks[1].convertTo(right_mask, CV_32F);

  Mat result = LaplacianBlend(left_f, right_f, left_mask);
  result.convertTo(result, CV_8UC3);
  show_img("result", result);
}

void blend1() {
  vector<Mat> pano_images;
  vector<Mat> pano_masks;
  char img_path[128];
  char app_path[32] = "..";

  for (int i = 1; i <= 2; i ++) {
    sprintf(img_path, "%s/mask%d.png", app_path, i);
    Mat tmp_mask = imread(img_path);
    if (tmp_mask.channels() != 1) {
      cvtColor(tmp_mask, tmp_mask, COLOR_RGB2GRAY);
    }
    tmp_mask.convertTo(tmp_mask, CV_8UC1);
    pano_masks.emplace_back(tmp_mask);

    sprintf(img_path, "%s/img%d.png", app_path, i);
    Mat tmp_image = imread(img_path);
    pano_images.emplace_back(tmp_image);
  }

  vector<Point2i> corners;
  for (int i = 0; i < 2; i ++) {
    corners.emplace_back(0, 0);
  }

  // 为结果生成区域
  vector<Size> sizes;
  for (int i = 0; i < 2; i ++) {
    sizes.emplace_back(pano_images[i].size());
  }
  Size dst_sz = resultRoi(corners, sizes).size();
  float blend_width = 10;
  Ptr<Blender> blender;
  if (true) {
    // 多频带融合
    blender = Blender::createDefault(Blender::MULTI_BAND, true);// try_cuda = false
    MultiBandBlender *mb = dynamic_cast<MultiBandBlender*>(blender.get());
    mb->setNumBands(5);
  } else {
    // 羽化融合
    blender = Blender::createDefault(Blender::FEATHER);
    FeatherBlender* fb = dynamic_cast<FeatherBlender*>(blender.get());
    fb->setSharpness(1.f/blend_width);
  }
  blender->prepare(corners, sizes);

  // 纹理映射
  for (int i = 0; i < 2; i ++) {
    // 膨胀运算
    // Mat dilated_mask, seam_mask, mask_warped;
    // pano_images[i].copyTo(mask_warped);
    // dilate(pano_masks[i], dilated_mask, Mat());
    // resize(dilated_mask, seam_mask, mask_warped.size(), 0, 0, INTER_LINEAR_EXACT);
    // mask_warped = seam_mask & dilated_mask;
    // 转换图像格式
    Mat images_warped_s;
    pano_images[i].convertTo(images_warped_s, CV_16S);
    // blender->feed(images_warped_s, mask_warped, corners[i]);
    blender->feed(images_warped_s, pano_masks[i], corners[i]);
  }
  Mat blend_result, blend_mask;
  blender->blend(blend_result, blend_mask);
  blend_result.convertTo(blend_result, CV_8UC3);
  cvtColor(blend_result, blend_result, COLOR_RGB2RGBA);

  show_img("result", blend_result);
}
