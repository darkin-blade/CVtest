/*
  条件限制delaunay三角剖分, 输出索引
  https://doc.cgal.org/latest/Mesh_2/index.html
*/

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_conformer_2.h>
#include <iostream>
typedef CGAL::Exact_predicates_inexact_constructions_kernel
  K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K>
  CDT;
typedef CDT::Point
  Point;
typedef CDT::Vertex_handle
  Vertex_handle;

using namespace std;

int main()
{
  CDT cdt;
  // construct a constrained triangulation
  Vertex_handle
    va = cdt.insert(Point( 5., 5.)),
    vb = cdt.insert(Point(-5., 5.)),
    vc = cdt.insert(Point( 4., 3.)),
    vd = cdt.insert(Point( 5.,-5.)),
    ve = cdt.insert(Point( 6., 6.)),
    vf = cdt.insert(Point(-6., 6.)),
    vg = cdt.insert(Point(-6.,-6.)),
    vh = cdt.insert(Point( 6.,-6.));
  cdt.insert_constraint(va,vb);
  cdt.insert_constraint(vb,vc);
  cdt.insert_constraint(vc,vd);
  cdt.insert_constraint(vd,va);
  cdt.insert_constraint(ve,vf);
  cdt.insert_constraint(vf,vg);
  cdt.insert_constraint(vg,vh);
  cdt.insert_constraint(vh,ve);
  for (CDT::Finite_faces_iterator fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); fit ++) {
    ;
  }

  // cout << "Number of vertices before: "
  //           << cdt.number_of_vertices() << endl;
  // // make it conforming Delaunay
  // CGAL::make_conforming_Delaunay_2(cdt);
  // cout << "Number of vertices after make_conforming_Delaunay_2: "
  //           << cdt.number_of_vertices() << endl;
  // // then make it conforming Gabriel
  // CGAL::make_conforming_Gabriel_2(cdt);
  // cout << "Number of vertices after make_conforming_Gabriel_2: "
  //           << cdt.number_of_vertices() << endl;
}