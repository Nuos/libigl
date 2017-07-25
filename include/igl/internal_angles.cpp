// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2015 Alec Jacobson <alecjacobson@gmail.com>
// Copyright (C) 2015 Daniele Panozzo <daniele.panozzo@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#include "internal_angles.h"
#include "squared_edge_lengths.h"
#include "parallel_for.h"
#include "get_seconds.h"

template <typename DerivedV, typename DerivedF, typename DerivedK>
IGL_INLINE void igl::internal_angles(
  const Eigen::MatrixBase<DerivedV>& V,
  const Eigen::MatrixBase<DerivedF>& F,
  Eigen::PlainObjectBase<DerivedK> & K)
{
  using namespace Eigen;
  using namespace std;
  typedef typename DerivedV::Scalar Scalar;
  if(F.cols() == 3)
  {
    // Edge lengths
    Matrix<
      Scalar,
      DerivedF::RowsAtCompileTime,
      DerivedF::ColsAtCompileTime> L_sq;
      igl::squared_edge_lengths(V,F,L_sq);

    assert(F.cols() == 3 && "F should contain triangles");
    igl::internal_angles_using_squared_edge_lengths(L_sq,K);
  }else
  {
    assert(V.cols() == 3 && "If F contains non-triangle facets, V must be 3D");
    K.resizeLike(F);
    auto corner = [](
      const typename DerivedV::ConstRowXpr & x, 
      const typename DerivedV::ConstRowXpr & y, 
      const typename DerivedV::ConstRowXpr & z)
    {
      typedef Eigen::Matrix<Scalar,1,3> RowVector3S;
      RowVector3S v1 = (x-y).normalized();
      RowVector3S v2 = (z-y).normalized();
      // http://stackoverflow.com/questions/10133957/signed-angle-between-two-vectors-without-a-reference-plane
      Scalar s = v1.cross(v2).norm();
      Scalar c = v1.dot(v2);
      return atan2(s, c);
    };
    for(unsigned i=0; i<F.rows(); ++i)
    {
      for(unsigned j=0; j<F.cols(); ++j)
      {
        K(i,j) = corner(
            V.row(F(i,int(j-1+F.cols())%F.cols())),
            V.row(F(i,j)),
            V.row(F(i,(j+1+F.cols())%F.cols()))
            );
      }
    }
  }
}

template <typename DerivedL, typename DerivedK>
IGL_INLINE void igl::internal_angles_using_squared_edge_lengths(
  const Eigen::MatrixBase<DerivedL>& L_sq,
  Eigen::PlainObjectBase<DerivedK> & K)
{
  typedef typename DerivedL::Index Index;
  assert(L_sq.cols() == 3 && "Edge-lengths should come from triangles");
  const Index m = L_sq.rows();
  K.resize(m,3);
  parallel_for(
    m,
    [&L_sq,&K](const Index f)
    {
      for(size_t d = 0;d<3;d++)
      {
        const auto & s1 = L_sq(f,d);
        const auto & s2 = L_sq(f,(d+1)%3);
        const auto & s3 = L_sq(f,(d+2)%3);
        K(f,d) = acos((s3 + s2 - s1)/(2.*sqrt(s3*s2)));
      }
    },
    1000l);
}

template <typename DerivedL, typename DerivedK>
IGL_INLINE void igl::internal_angles_using_edge_lengths(
  const Eigen::MatrixBase<DerivedL>& L,
  Eigen::PlainObjectBase<DerivedK> & K)
{
  // Note:
  //   Usage of internal_angles_using_squared_edge_lengths() is preferred to internal_angles_using_squared_edge_lengths()
  //   This function is deprecated and probably will be removed in future versions
  typedef typename DerivedL::Index Index;
  assert(L.cols() == 3 && "Edge-lengths should come from triangles");
  const Index m = L.rows();
  K.resize(m,3);
  parallel_for(
    m,
    [&L,&K](const Index f)
    {
      for(size_t d = 0;d<3;d++)
      {
        const auto & s1 = L(f,d);
        const auto & s2 = L(f,(d+1)%3);
        const auto & s3 = L(f,(d+2)%3);
        K(f,d) = acos((s3*s3 + s2*s2 - s1*s1)/(2.*s3*s2));
      }
    },
    1000l);
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
// generated by autoexplicit.sh
template void igl::internal_angles<Eigen::Matrix<float, -1, 3, 1, -1, 3>, Eigen::Matrix<int, -1, 3, 1, -1, 3>, Eigen::Matrix<float, -1, 3, 0, -1, 3> >(Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 1, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, 3, 1, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, 3, 0, -1, 3> >&);
// generated by autoexplicit.sh
template void igl::internal_angles<Eigen::Matrix<float, -1, 3, 1, -1, 3>, Eigen::Matrix<unsigned int, -1, 3, 1, -1, 3>, Eigen::Matrix<float, -1, 3, 0, -1, 3> >(Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 1, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<unsigned int, -1, 3, 1, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, 3, 0, -1, 3> >&);
template void igl::internal_angles<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::internal_angles<Eigen::Matrix<double, -1, 3, 1, -1, 3>, Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 1, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::internal_angles<Eigen::Matrix<double, -1, 3, 0, -1, 3>, Eigen::Matrix<int, -1, 3, 0, -1, 3>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, 3, 0, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::internal_angles<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, 3, 0, -1, 3> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> >&);
template void igl::internal_angles<Eigen::Matrix<double, -1, 3, 0, -1, 3>, Eigen::Matrix<int, -1, 3, 0, -1, 3>, Eigen::Matrix<double, -1, 3, 0, -1, 3> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, 3, 0, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> >&);
template void igl::internal_angles<Eigen::Matrix<double, -1, 3, 1, -1, 3>, Eigen::Matrix<int, -1, 3, 1, -1, 3>, Eigen::Matrix<double, -1, 3, 0, -1, 3> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 1, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, 3, 1, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> >&);
#endif
