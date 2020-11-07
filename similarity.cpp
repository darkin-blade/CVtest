/**
 * 计算图像相似度
 */

#include <assert.h>
#include <stdio.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

using namespace std;
using namespace cv;

int main() 
{
  char path_1[128], path_2[128];
  scanf("%s %s", path_1, path_2);
  Mat img_1 = imread(path_1);
  Mat img_2 = imread(path_2);

  if (img_1.channels() == 1) {
    int histSize = 256;
    float range[] = { 0, 256 };
    const float * histRange = { range };

    Mat hist1, hist2;

    calcHist(&img_1, 1, 0, Mat(), hist1, 1, &histSize, &histRange, true, false);
    normalize(hist1, hist1, 0, 1, NORM_MINMAX, -1, Mat());
    calcHist(&img_2, 1, 0, Mat(), hist2, 1, &histSize, &histRange, true, false);
    normalize(hist2, hist2, 0, 1, NORM_MINMAX, -1, Mat());

    double dSimilarity = compareHist(hist1, hist2, CV_COMP_CORREL);
    printf("1: %lf\n", dSimilarity);
  } else {
    cvtColor(img_1, img_1, COLOR_BGR2HSV);
    cvtColor(img_2, img_2, COLOR_BGR2HSV);

    int h_bins = 50;
    int s_bins = 60;
    int histSize[] = { h_bins, s_bins };
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    const float * ranges[] = { h_ranges, s_ranges };
    int channels[] = { 0, 1 };

    MatND hist1, hist2;
    calcHist(&img_1, 1, channels, Mat(), hist1, 2, histSize, ranges, true, false);
    normalize(hist1, hist1, 0, 1, NORM_MINMAX, -1, Mat());
    calcHist(&img_2, 1, channels, Mat(), hist2, 2, histSize, ranges, true, false);
    normalize(hist2, hist2, 0, 1, NORM_MINMAX, -1, Mat());

    double dSimilarity = compareHist(hist1, hist2, CV_COMP_CORREL);
    printf("3: %lf\n", dSimilarity);
  }
}
