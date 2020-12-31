
/**
  * 最大流算法
  */

#include <iostream>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/detail/gcgraph.hpp>
#include <opencv2/imgproc/types_c.h>

#define LOG(format, ...) \
  printf("\033[1;36m" format "\33[0m\n", ## __VA_ARGS__)

using namespace std;

using namespace cv;
using namespace cv::detail;

int main() {
  GCGraph<double> graph(4, 5);
  graph.addVtx();
  graph.addVtx();
  graph.addVtx();
  graph.addVtx();
  graph.addTermWeights(0, 10000, 0);
  graph.addTermWeights(3, 0, 10000);
  graph.addEdges(0, 1, 40, 40);
  graph.addEdges(0, 3, 20, 20);
  graph.addEdges(1, 3, 20, 20);
  graph.addEdges(1, 2, 30, 30);
  graph.addEdges(2, 3, 10, 10);
  LOG("max %lf", graph.maxFlow());
}