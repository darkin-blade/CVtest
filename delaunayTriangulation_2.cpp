/*
  opencv的delaunay三角剖分
*/

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <iostream>

using namespace cv;
using namespace std;

//随机产生一个点集
vector<Point2f> generatePointSet(int n, Rect rect)
{
    vector<Point2f> pointSet;
    for (int i = 0; i < n; i++)
    {
        Point2f fp((float)(rand() % (rect.width - 2 * rect.x) + rect.x),
                   (float)(rand() % (rect.height - 2 * rect.y) + rect.y));
        pointSet.push_back(fp);
    }
    return pointSet;
}

//标记出点
static void drawPoint(Mat &img, Point2f fp, Scalar color)
{
    circle(img, fp, 3, color, CV_FILLED, 8, 0);
}

//画出点集
static void drawPointSet(Mat &img, vector<Point2f> pointSet, Scalar color)
{
    for (int i = 0; i < pointSet.size(); i++)
    {
        drawPoint(img, pointSet[i], color);
    }
}

//画出剖分
static void drawSubdiv(Mat &img, Subdiv2D &subdiv, Scalar delaunay_color)
{
    vector<Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);
    vector<Point> pt(3);

    for (size_t i = 0; i < triangleList.size(); i++)
    {
        Vec6f t = triangleList[i];
        pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
        pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
        pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
        line(img, pt[0], pt[1], delaunay_color, 1, CV_AA, 0);
        line(img, pt[1], pt[2], delaunay_color, 1, CV_AA, 0);
        line(img, pt[2], pt[0], delaunay_color, 1, CV_AA, 0);
    }
}

//画出Voronoi图
static void paintVoronoi(Mat &img, Subdiv2D &subdiv)
{
    vector<vector<Point2f>> facets;
    vector<Point2f> centers;
    subdiv.getVoronoiFacetList(vector<int>(), facets, centers);

    vector<Point> ifacet;
    vector<vector<Point>> ifacets(1);

    for (size_t i = 0; i < facets.size(); i++)
    {
        ifacet.resize(facets[i].size());
        for (size_t j = 0; j < facets[i].size(); j++)
            ifacet[j] = facets[i][j];

        Scalar color;
        color[0] = rand() & 255;
        color[1] = rand() & 255;
        color[2] = rand() & 255;
        fillConvexPoly(img, ifacet, color, 8, 0);

        ifacets[0] = ifacet;
        polylines(img, ifacets, true, Scalar(), 1, CV_AA, 0);
        circle(img, centers[i], 3, Scalar(), CV_FILLED, CV_AA, 0);
    }
}

int main()
{
    //创建点集
    Rect rect(100, 100, 400, 400);

    Mat img(rect.size(), CV_8UC3);
    img = Scalar::all(0);

    srand((unsigned)time(0));

    vector<Point2f> pointset = generatePointSet(20, rect);
    // Mat img_ptst = img.clone();
    // drawPointSet(img_ptst, pointset, Scalar(0, 255, 0));
    // imshow("Point set", img_ptst);
    // imwrite("pointSet.jpg", img_ptst);

    //创建Delaunay剖分
    Subdiv2D subdiv(rect);
    for (int i = 0; i < pointset.size(); i++)
    {
        subdiv.insert(pointset[i]);
    }

    //画出Delaunay剖分三角形
    Mat img_delaunay = img.clone();
    drawSubdiv(img_delaunay, subdiv, Scalar(255, 255, 255));
    imshow("Delaunay", img_delaunay);
    imwrite("delaunay.jpg", img_delaunay);

    // //画出Voronoi图
    // Mat img_voronoi = img.clone();
    // paintVoronoi(img_voronoi, subdiv);
    // imshow("Voronoi", img_voronoi);
    // imwrite("voronoi.jpg", img_voronoi);

    waitKey(0);
    return 0;
}
