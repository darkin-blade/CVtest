/*
  条件限制delaunay三角剖分, 输出索引
  https://stackoverflow.com/questions/17680321/retrieve-vertices-from-cgals-delaunay-constrained-triangulation
*/
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Spatial_sort_traits_adapter_2.h>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_2<unsigned, K> Vb;
typedef CGAL::Constrained_triangulation_face_base_2<K>           Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>              TDS;
typedef CGAL::Exact_predicates_tag                               Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag> CDT;

typedef CDT::Point          Point;
typedef CGAL::Spatial_sort_traits_adapter_2<K,Point*> Search_traits;

using namespace std;

template <class InputIterator>
void insert_with_info(CDT& cdt, InputIterator first, InputIterator last)
{
  vector<ptrdiff_t> indices;
  vector<Point> points;
  ptrdiff_t index = 0;

  for (InputIterator it = first; it != last; it ++){
    cout << index << " " << *it << endl;
    points.push_back( *it);
    indices.push_back(index ++);
  }

  // CGAL::spatial_sort(
  //   indices.begin(),
  //   indices.end(),
  //   Search_traits(&(points[0]),
  //   cdt.geom_traits()));

  // for (int i = 0; i < points.size(); i ++) {
  //   cout << points[i] << endl;
  // }

  CDT::Vertex_handle v_hint;
  // CDT::Face_handle hint;
  // for (typename vector<ptrdiff_t>::const_iterator it = indices.begin(), end = indices.end();
  //   it != end; ++it){
  //   v_hint = cdt.insert(points[*it], hint);
  //   if (v_hint != CDT::Vertex_handle()){
  //     v_hint->info() = *it;
  //     hint = v_hint->face();
  //   }
  // }
  for (int i = 0; i < indices.size(); i ++) {
    // v_hint = cdt.insert(points[i], hint);
    v_hint = cdt.insert(points[i]);
    if (v_hint != CDT::Vertex_handle()) {
      v_hint->info() = i;
      // hint = v_hint->face();
    } else {
      cout << "error" << endl;
    }
  }
}

int main()
{
  vector< Point > points;
  points.push_back( Point(0,0) );
  points.push_back( Point(1,0) );
  points.push_back( Point(0,1) );
  points.push_back( Point(14,4) );
  points.push_back( Point(2,2) );
  points.push_back( Point(-4,0) );


  CDT cdt;
  insert_with_info(cdt, points.begin(), points.end());

  CGAL_assertion( cdt.number_of_vertices() == 6 );

  // check that the info was correctly set.
  CDT::Finite_vertices_iterator vit;
  for (vit = cdt.finite_vertices_begin(); vit != cdt.finite_vertices_end(); ++vit)
    if( points[ vit->info() ] != vit->point() ){
      cerr << "Error different info" << endl;
      exit(EXIT_FAILURE);
    }


  for (CDT::Finite_faces_iterator fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); fit ++) {
    cout << (fit->vertex(0))->info() << "-" << (fit->vertex(1))->info() << "-" << (fit->vertex(2))->info() << endl;
  }
  cout << "OK" << endl;
}