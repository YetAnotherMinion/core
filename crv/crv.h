/*
 * Copyright 2015 Scientific Computation Research Center
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#ifndef CRV_H
#define CRV_H

#include "apfMesh2.h"
#include "apfShape.h"
#include <ma.h>
#include <stdio.h>

/** \file crv.h
  * \brief main file for curved element support */

/** \namespace crv
  * \brief All CRV functions are contained in this namespace */
namespace crv {

/** \brief actually 1 greater than max order */
static unsigned const MAX_ORDER = 20;
/** \brief sets the blending order, if shape blending is used */
void setBlendingOrder(const int type, const int b);
/** \brief gets the blending order */
int getBlendingOrder(const int type);

/** \brief computes min det Jacobian / max det Jacobian */
double getQuality(apf::Mesh* m,apf::MeshEntity* e);

/** \brief Base Mesh curving object
  \details P is the order, S is the space dimension,
  different from the mesh dimension, used to distinguish between planar 2D
  meshes and surface meshes. */

/** \brief converts interpolating points to control points */
void convertInterpolationPoints(int n, int ne,
    apf::NewArray<apf::Vector3>& nodes,
    apf::NewArray<double>& c,
    apf::NewArray<apf::Vector3>& newNodes);

void convertInterpolationPoints(apf::Mesh2* m, apf::MeshEntity* e,
    int n, int ne, apf::NewArray<double>& c);

/** \brief a per entity version of above */
void snapToInterpolate(apf::Mesh2* m, apf::MeshEntity* e);

class MeshCurver
{
  public:
    MeshCurver(apf::Mesh2* m, int P, int S) : m_mesh(m), m_order(P),
    m_spaceDim(S)
    {
      if (S == 0)
        m_spaceDim = m->getDimension();
    };
    virtual ~MeshCurver() {};
    virtual bool run() = 0;

    /** \brief snaps points to interpolating locations */
    void snapToInterpolate(int dim);

    /** \brief wrapper around synchronizeFieldData */
    void synchronize();

  protected:
    apf::Mesh2* m_mesh;
    int m_order;
    int m_spaceDim;
};

/** \brief curves an already changed mesh
 * \details this is a bit of a hack, meant to work with
 * all interpolating shape functions
 */
class InterpolatingCurver : public MeshCurver
{
  public:
    InterpolatingCurver(apf::Mesh2* m, int P, int S = 0) : MeshCurver(m,P,S) {};
    virtual ~InterpolatingCurver() {};
    virtual bool run();

};

/** \brief this curves a mesh with Bezier shapes
 * \details converts the mesh and snaps boundary entities to geometry
 * P is the order, B is the blending order (set to 0 to use full shapes)
 */
class BezierCurver : public MeshCurver
{
  public:
    BezierCurver(apf::Mesh2* m, int P, int B, int S = 0) : MeshCurver(m,P,S)
    {
      if(B > 0){
        for(int type = 0; type < apf::Mesh::TYPES; ++type)
        setBlendingOrder(type,B);
      }
    };

    /** \brief curves a mesh using bezier curves of chosen order
      \details finds interpolating points, then converts to control points
      see crvBezier.cc */
    virtual bool run();
};

/** \brief this curves a mesh with 4th order G1 Patches
 * \details approach is in crvBezier.cc
 */
class GregoryCurver : public BezierCurver
{
  public:
    GregoryCurver(apf::Mesh2* m, int P, int B, int S = 0)
    : BezierCurver(m,P,B,S) {};
    /** \brief curves a mesh using G1 gregory surfaces, see crvBezier.cc */
    virtual bool run();
    /** \brief sets cubic edge points using normals */
    void setCubicEdgePointsUsingNormals();
    /** \brief sets internal points using neighbors (See Notes)
      \details NOT CURRENTLY FULLY IMPLEMENTED */
//    void setInternalPointsUsingNeighbors();
    /** \brief sets internal points locally */
    void setInternalPointsLocally();
};

/** \brief Elevate a bezier curve to a higher order
 \details This elevates from nth order to n+rth order
 requires the curve be order n+r in memory already, and
 that the first n points correspond to the lower order curve */
void elevateBezierCurve(apf::Mesh2* m, apf::MeshEntity* edge, int n, int r);

/** \brief Get the Bezier Curve or Shape of some order
 \details goes from first to sixth order */
apf::FieldShape* getBezier(int order);
/** \brief Get the 4th order Gregory Surface*/
apf::FieldShape* getGregory();

/** \brief get coefficients for interpolating points to control points
 \details works only for prescribed optimal point locations */
void getBezierTransformationCoefficients(int P, int type,
    apf::NewArray<double>& c);
void getInternalBezierTransformationCoefficients(apf::Mesh* m, int P, int blend,
    int type, apf::NewArray<double>& c);
void getGregoryTransformationCoefficients(int type, apf::NewArray<double>& c);
void getGregoryBlendedTransformationCoefficients(int blend, int type,
    apf::NewArray<double>& c);

void getHigherOrderBezierTransform(apf::Mesh* m, int P, int type,
    apf::NewArray<double> & c);
void getHigherOrderInternalBezierTransform(apf::Mesh* m, int P, int blend,
    int type, apf::NewArray<double> & c);

/** \brief computes interpolation error of a curved entity on a mesh
  \details this computes the Hausdorff distance by sampling
   n points per dimension of the entity through uniform
   sampling locations in parameter space */
double interpolationError(apf::Mesh* m, apf::MeshEntity* e, int n);

/** \brief Visualization, writes file for specified type */
void writeCurvedVtuFiles(apf::Mesh* m, int type, int n, const char* prefix);

/** \brief Visualization, writes file of control nodes for each entity */
void writeControlPointVtuFiles(apf::Mesh* m, const char* prefix);
/** \brief Visualization, writes file of shapes evaluated at node xi
 *  for each entity */
void writeInterpolationPointVtuFiles(apf::Mesh* m, const char* prefix);
/** \brief publically accessible functions */
int getTriNodeIndex(int P, int i, int j);
int getTetNodeIndex(int P, int i, int j, int k);

/** \brief binomial function n!/(i!(n-i)!) */
int binomial(int n, int i);
/** \brief trinomial function n!/(i!j!(n-i-j)!) */
int trinomial(int n, int i, int j);
/** \brief "quadnomial" function n!/(i!j!k!(n-i-j-k)!) */
int quadnomial(int n, int i, int j, int k);

/** \brief check the validity (det(Jacobian) > eps) of an element
 * \details entities is a container of invalid downward entities
 * algorithm is an integer corresponding to what method to use
 * 0 - subdivision
 * 1 - elevation
 * 2 - subdivision, without first check
 * 3 - elevation, without first check
 * 4 - subdivision, using matrices
 * methods 2 and 3 exist because the first check tends to catch everything
 * without actually using subdivision and elevation, and giving this option
 * is easier for debugging and verifying the efficacy of those procedures
 * */

int checkTriValidity(apf::Mesh* m, apf::MeshEntity* e,
    apf::MeshEntity* entities[6], int algorithm);

int checkTetValidity(apf::Mesh* m, apf::MeshEntity* e,
    apf::MeshEntity* entities[14], int algorithm);

/** \brief crv fail function */
void fail(const char* why) __attribute__((noreturn));

}

#endif
