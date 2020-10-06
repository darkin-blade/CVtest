/**
  * 绘制接缝线
  */

#include <opencv2/opencv.hpp>
#include <vector>
#include <stdio.h>

#define CYAN(format, ...) \
  printf("\033[1;36m" format "\33[0m\n", ## __VA_ARGS__)

using namespace cv;
using namespace cv::detail;
using namespace std;

void show_img(const char *window_name, Mat img);
int main(int argc, char** argv) 
{
  char tmp_name[32];// 临时文件名
  const int img_num = 2;
  const char *img_name[img_num] = {"1.jpg", "2.jpg"};

  Ptr<Feature2D> finder;
  if (true) {
    finder = ORB::create();
  } else if (false) {
    finder = AKAZE::create();
  } else {
    assert(0);
  }

  Mat img;// 临时存储
  vector<Mat> imgs;// 存储所有图片
  vector<ImageFeatures> features(img_num);

  // 寻找特征点
  for (int i = 0; i < img_num; i ++) {
    // 读取图片
    Mat full_img = imread(img_name[i]);

    // 放缩图片, cols 是宽
    int width = 720;
    int height = full_img.rows * 720 / full_img.cols;
    img = Mat(height, width, full_img.type());
    resize(full_img, img, img.size(), 0, 0, INTER_LINEAR);
    imgs.push_back(img);// 保存图片到向量

    // 查找特征点
    computeImageFeatures(finder, img, features[i]);

    full_img.release();
  }

  img.release();

  vector<MatchesInfo> pairwise_matches;// 特征匹配
  Ptr<FeaturesMatcher> matcher = makePtr<BestOf2NearestMatcher>(false, 0.3f, 6, 6);// TODO 参数
  (*matcher)(features, pairwise_matches);

  // 相机参数评估
  Ptr<Estimator> estimator;
  if (true) {
    estimator = makePtr<HomographyBasedEstimator>();
  } else if (false) {
    estimator = makePtr<AffineBasedEstimator>();
  } else {
    assert(0);
  }

  vector<CameraParams> cameras;
  (*estimator)(features, pairwise_matches, cameras);
  for (size_t i = 0; i < cameras.size(); ++i)
  {
    Mat R;
    cameras[i].R.convertTo(R, CV_32F);
    cameras[i].R = R;
  }

  Ptr<detail::BundleAdjusterBase> adjuster;// 光束平差法,精确化相机参数
  if (false) adjuster = makePtr<detail::BundleAdjusterReproj>();
  else if (true) adjuster = makePtr<detail::BundleAdjusterRay>();
  else if (false) adjuster = makePtr<detail::BundleAdjusterAffinePartial>();
  adjuster->setConfThresh(1);// TODO conf_thresh
  // TODO setRefinementMask
  (*adjuster)(features, pairwise_matches, cameras);

  // 波形矫正
  vector<Mat> rmats;
  for (size_t i = 0; i < cameras.size(); i ++) {
    rmats.push_back(cameras[i].R.clone());
  }
  if (false) {// 水平校正
    waveCorrect(rmats, WAVE_CORRECT_HORIZ);
  } else if (true) {// 垂直校正
    waveCorrect(rmats, WAVE_CORRECT_VERT);
  } else {
    assert(0);
  }
  for (size_t i = 0; i < cameras.size(); i ++) {
    cameras[i].R = rmats[i];
  }

  // 图像映射变换
  vector<Point> corners(img_num);
  vector<UMat> masks_warped(img_num);
  vector<UMat> images_warped(img_num);
  vector<Mat> masks(img_num);

  for (int i = 0; i < img_num; i ++)
  {
    masks[i].create(imgs[i].size(), CV_8U);
    masks[i].setTo(Scalar::all(255));
  }

  Ptr<WarperCreator> warper_creator;
  // 选择投影类型
  if (true) {
    warper_creator = makePtr<cv::PlaneWarper>();
  } else if (false) {
    warper_creator = makePtr<cv::AffineWarper>();
  } else if (false) {
    warper_creator = makePtr<cv::CylindricalWarper>();
  } else if (false) {
    warper_creator = makePtr<cv::SphericalWarper>();
  } else {
    assert(0);
  }

  Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(cameras[0].focal));
  for (int i = 0; i < img_num; i ++)
  {
    Mat_<float> K;
    cameras[i].K().convertTo(K, CV_32F); 
    corners[i] = warper->warp(imgs[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);
    warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]);
  }

  // TODO
  UMat tmp;
  imgs[0].copyTo(tmp);
  cout << images_warped[0].type() << " " << images_warped[0].type() << " " << tmp.type() << endl;
  cout << images_warped[0].channels() << " " << images_warped[0].channels() << " " << tmp.channels() << endl;
  // for (int i = 0; i < img_num; i ++) {
  //   images_warped[i].getMat(ACCESS_RW).copyTo(images_warped[i]);
  // }

  // 曝光补偿
  Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(ExposureCompensator::GAIN);// TODO 参数
  compensator->feed(corners, images_warped, masks_warped);

  for (int i = 0; i < img_num; i ++) {
    compensator->apply(i, corners[i], images_warped[i], masks_warped[i]);
    sprintf(tmp_name, "img%d", i);
    show_img(tmp_name, images_warped[i].getMat(ACCESS_RW));
    // sprintf(tmp_name, "mask%d", i);
    // show_img(tmp_name, masks_warped[i].getMat(ACCESS_RW));
  }

  if (0) {
    Ptr<SeamFinder> seam_finder;// 接缝线寻找器
    if (true) {
      seam_finder = makePtr<detail::VoronoiSeamFinder>();// 逐点法
    } else if (false) {
      seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR);// 默认: 图割法
    } else if (false) {
      seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR_GRAD);
    } else if (false) {
      seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR);// 动态规划法
    } else if (false) {
      seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR_GRAD);
    } else {
      assert(0);
    }

    vector<UMat> images_warped_f(img_num);// TODO
    for (int i = 0; i < img_num; i ++) {// 图像数据类型转换
      images_warped[i].convertTo(images_warped_f[i], CV_32F);
    }

    // 得到接缝线的掩码图像 masks_warped
    seam_finder->find(images_warped_f, corners, masks_warped); 

    // 参考: https://blog.csdn.net/zhaocj/article/details/78944867
    // 通过canny边缘检测，得到掩码边界，其中有一条边界就是接缝线
    for (int k = 0; k < img_num; k ++) {
      Canny(masks_warped[k], masks_warped[k], 3, 9, 3); // TODO 参数
    }

    // 为了使接缝线看得更清楚，这里使用了膨胀运算来加粗边界线
    vector<Mat> dilate_img(img_num);  
    Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));// 定义结构元素,粗细

    for (int i = 0; i < img_num; i ++)// 遍历所有图像
    {
      dilate(masks_warped[i], dilate_img[i],element);// 膨胀运算
      // 在映射变换图上画出接缝线,在这里只是为了呈现出的一种效果,所以并没有区分接缝线和其他掩码边界
      for (int y = 0; y < images_warped[i].rows; y ++) {
        for (int x = 0; x < images_warped[i].cols; x ++) {
          if (dilate_img[i].at<uchar>(y, x) == 255) {
            // 掩码边界
            // 将UMat转成Mat
            // 颜色
            images_warped[i].getMat(ACCESS_READ).at<Vec3b>(y, x)[0] = 0;// B
            images_warped[i].getMat(ACCESS_READ).at<Vec3b>(y, x)[1] = 0;// G
            images_warped[i].getMat(ACCESS_READ).at<Vec3b>(y, x)[2] = 255;// R
          }				
        }
      }

      if (1) {// 显示/保存图片
        sprintf(tmp_name, "seam%d.jpg", i);
        show_img(tmp_name, images_warped[i].getMat(ACCESS_RW));
      }
    }
  }

  return 0;
}

void show_img(const char *window_name, Mat img) {
  namedWindow(window_name, WINDOW_NORMAL);
  resizeWindow(window_name, 1280, 720);
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
    CYAN("can't save img");
  }
}