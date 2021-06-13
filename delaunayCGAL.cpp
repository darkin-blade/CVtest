/*
  cgal三角化, 输出索引
  https://stackoverflow.com/questions/19850354/cgal-retrieve-vertex-index-after-delaunay-triangulation
*/

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <vector>

// 简化名称
typedef CGAL::Exact_predicates_inexact_constructions_kernel
  Kernel;
typedef CGAL::Triangulation_vertex_base_with_info_2<unsigned int, Kernel>
  Vb;
typedef CGAL::Triangulation_data_structure_2<Vb>
  Tds;
typedef CGAL::Delaunay_triangulation_2<Kernel, Tds>
  Delaunay;
typedef Kernel::Point_2
  Point;

int main() {

  std::vector< std::pair<Point,unsigned> > points;
  points.push_back( std::make_pair( Point(1,1), 0 ) );
  points.push_back( std::make_pair( Point(1,2), 1 ) );
  points.push_back( std::make_pair( Point(1,3), 2 ) );
  points.push_back( std::make_pair( Point(2,1), 3 ) );
  points.push_back( std::make_pair( Point(2,2), 4 ) );
  points.push_back( std::make_pair( Point(2,3), 5 ) );

  Delaunay triangulation;
  triangulation.insert(points.begin(),points.end());

  for(Delaunay::Finite_faces_iterator fit = triangulation.finite_faces_begin();
      fit != triangulation.finite_faces_end(); ++fit) {

    Delaunay::Face_handle face = fit;
    std::cout << "Triangle:\t" << triangulation.triangle(face) << std::endl;
    std::cout << "Vertex 0:\t" << triangulation.triangle(face)[0] << std::endl;
    std::cout << "Vertex 0:\t" << face->vertex(0)->info() << " " << face->vertex(1)->info() << " " << face->vertex(2)->info() << std::endl;
  }
}