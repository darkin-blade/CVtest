/**
 * 对接两张图片的特征点, 参考: https://blog.csdn.net/holybin/article/details/44734011
 **/

#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;
 
int main(int argc, char** argv)
{
   Mat img = imread("img_1.png");
   
   vector<KeyPoint> keyPoints;
   FAST(img, keyPoints, 20);

   // 绘制特征点
   Mat imgKeyPoints;
   drawKeypoints(img, keyPoints, imgKeyPoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

   imshow("key points", imgKeyPoints);

   waitKey(0);
 
   return 0;
}
