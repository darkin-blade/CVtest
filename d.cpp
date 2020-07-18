/**
 * 特征点匹配
 **/

#include <opencv2/opencv.hpp>
#include <vector>
#include <stdio.h>

#define CYAN(format, ...) \
  printf("\033[1;36m" format "\33[0m\n", ## __VA_ARGS__)

using namespace cv;
using namespace cv::detail;
using namespace std;

int main(int argc, char** argv)
{
  char tmp_name[32];// 临时文件名
  const int img_num = 2;
  const char *img_name[img_num] = {"mac1.jpg", "mac2.jpg"};
  // const int img_num = 1;
  // const char *img_name[img_num] = {"boat1.jpg"};

  Ptr<Feature2D> finder;
  if (true) {
    finder = ORB::create();
  } else {
    finder = AKAZE::create();
  }

  Mat img;
  vector<Mat> imgs;// 存储所有图片
  vector<ImageFeatures> features(img_num);

  // 寻找特征点
  for (int i = 0; i < img_num; i ++) {
    // 读取图片
    Mat full_img = imread(img_name[i]);

    // 放缩图片, cols 是宽
    int width = 480;
    int height = full_img.rows * 480 / full_img.cols;
    img = Mat(height, width, full_img.type());
    resize(full_img, img, img.size(), 0, 0, INTER_LINEAR);
    imgs.push_back(img);// 保存图片到向量

    // 查找特征点
    computeImageFeatures(finder, img, features[i]);

    // 绘制特征点
    if (false) {
      Mat imgKeyPoints;
      drawKeypoints(img, features[i].keypoints, imgKeyPoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
      imshow(img_name[i], imgKeyPoints);

      sprintf(tmp_name, "tmp%d.jpg", i);
      imwrite(tmp_name, imgKeyPoints);
    }

    full_img.release();
  }

  img.release();

  // 匹配特征点
  vector<MatchesInfo> pairwise_matches;
  Ptr<FeaturesMatcher> matcher;
  if (false) {// 定义特征匹配器
    matcher = makePtr<BestOf2NearestMatcher>(false, 0.3);// TODO 参数
  } else if (true) {
    matcher = makePtr<AffineBestOf2NearestMatcher>(false, false, 0.3);// TODO 参数
  } else {
    assert(0);
  }

  (*matcher)(features, pairwise_matches);// 进行特征匹配

  if (false) {// 第一种匹配方法, 参考 https://blog.csdn.net/zhaocj/article/details/78799194
    Mat dispimg;// 两个图像和匹配线
    Mat imgROI;

    // 将两幅图像合并显示 TODO 看不懂
    dispimg.create(Size(imgs[0].cols * 2, imgs[0].rows), CV_8UC3);// TODO 最后一个参数
    imgROI = dispimg(Rect(0, 0, imgs[0].cols, imgs[0].rows));
    resize(imgs[0], imgROI, Size((imgs[0].cols), (imgs[0].rows)));  
    imgROI = dispimg(Rect((imgs[0].cols), 0, (imgs[1].cols), (imgs[1].rows)));  
    resize(imgs[1], imgROI, Size((imgs[1].cols), (imgs[1].rows)));

    // 绘制匹配点对
    Point2f p1, p2;// 分别表示两幅图像内的匹配点对
    int j = 0;
    for (size_t i = 0; i < pairwise_matches[1].matches.size(); i ++)// 遍历匹配点对
    {
      if (!pairwise_matches[1].inliers_mask[i]) { // 不是内点，则继续下一次循环
        continue;
      }

      const DMatch& m = pairwise_matches[1].matches[i];// 得到内点的匹配点对
      p1 = features[0].keypoints[m.queryIdx].pt; 
      p2 = features[1].keypoints[m.trainIdx].pt;
      p2.x += features[0].img_size.width;// p2: 在合并图像上的坐标

      line(dispimg, p1, p2, Scalar(0, 0, 255), 1, 16);// 用直线进行匹配, CV_AA 未定义, 直接写16

      if(j ++ == 20)// 控制内点数量
        break;
    }

    // 调整窗口大小
    namedWindow("pair", 0);// TODO 最后一个参数
    resizeWindow("pair", 1280, 720);
    imshow("pair", dispimg);
  } else if (false) {// 第二种匹配方法, 参考: https://www.cnblogs.com/skyfsm/p/7401523.html      
    FlannBasedMatcher flannBasedMatcher;
    Feature2D finder;// TODO 命名
    vector<Mat> imgMatches(img_num);// 记录特征点信息
    vector<DMatch> matchPoints;
    Mat img_match;// 最终结果

    // 特征点匹配
    finder.compute(imgs[0], features[0].keypoints, imgMatches[0]);// 没有实现
    finder.compute(imgs[1], features[1].keypoints, imgMatches[1]);
    flannBasedMatcher.match(imgMatches[0], imgMatches[1], matchPoints, Mat());// TODO 最后一个参数
    drawMatches(imgs[0], features[0].keypoints, imgs[1], features[1].keypoints, matchPoints, img_match);

    // 调整窗口大小
    namedWindow("pair", 0);// TODO 最后一个参数
    resizeWindow("pair", 1280, 720);
    imshow("pair", img_match);
  } else if (true) {// 第三种匹配方法, 参考: https://www.jianshu.com/p/420f8211d1cb
    Ptr<Feature2D> descriptor = ORB::create();// 原文 typedef DescriptorExtractor
    vector<Mat> imgMatches(img_num);// 记录特征点信息
    Ptr<DescriptorMatcher> descriptor_matcher = DescriptorMatcher::create("BruteForce-Hamming");// 使用汉明距离进行匹配

    descriptor->compute(imgs[0], features[0].keypoints, imgMatches[0]);
    descriptor->compute(imgs[1], features[1].keypoints, imgMatches[1]);

    // 特征点匹配
    vector<DMatch> bad_matches, good_matches;
    descriptor_matcher->match(imgMatches[0], imgMatches[1], bad_matches);

    // 绘制匹配点
    Mat bad_match, good_match;
    drawMatches(imgs[0], features[0].keypoints, imgs[1], features[1].keypoints, bad_matches, bad_match);

    // 调整窗口大小
    namedWindow("match", 0);// TODO 最后一个参数
    resizeWindow("match", 1280, 720);
    imshow("match", bad_match);

    imwrite("result.jpg", bad_match);
  }

  waitKey(0);

  return 0;
}
