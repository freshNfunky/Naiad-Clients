// -----------------------------------------------------------------------------
//
// em_glmat44_algo.h
//
// Various useful matrix algorithms.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
//
// This material contains the confidential and proprietary information of
// Exotic Matter AB and may not be disclosed, copied or duplicated in
// any form, electronic or hardcopy, in whole or in part, without the
// express prior written consent of Exotic Matter AB. This copyright notice
// does not imply publication.
//
// -----------------------------------------------------------------------------

#ifndef EM_GLMAT44_ALGO
#define EM_GLMAT44_ALGO

#ifdef WIN32
#undef near
#undef far
#endif

// TODO: change to "" when placed in proper em folder.
#include "em_glmat33.h"
#include "em_glmat44.h"

#include <em_mat44_algo.h>
#include <em_assert.h>

#include <iostream>

namespace em {

// -----------------------------------------------------------------------------

template <typename T>
glmat44<T>
make_rotation_x(const T angle_deg)
{
    static const T deg2rad(static_cast<T>(M_PI)/180);
    const T s(sin(deg2rad*angle_deg));
    const T c(cos(deg2rad*angle_deg));

    return glmat44<T>(
        1, 0,  0, 0,
        0, c, -s, 0,
        0, s,  c, 0,
        0, 0,  0, 1);
}

// -----------------------------------------------------------------------------

template <typename T>
glmat44<T>
make_rotation_y(const T angle_deg)
{
    static const T deg2rad(static_cast<T>(M_PI)/180);
    const T s(sin(deg2rad*angle_deg));
    const T c(cos(deg2rad*angle_deg));

    return glmat44<T>(
         c, 0, s, 0,
         0, 1, 0, 0,
        -s, 0, c, 0,
         0, 0, 0, 1);
}

// -----------------------------------------------------------------------------

template <typename T>
glmat44<T>
make_rotation_z(const T angle_deg)
{
    static const T deg2rad(static_cast<T>(M_PI)/180);
    const T s(sin(deg2rad*angle_deg));
    const T c(cos(deg2rad*angle_deg));

    return glmat44<T>(
        c, -s, 0, 0,
        s,  c, 0, 0,
        0,  0, 1, 0,
        0,  0, 0, 1);
}

// -----------------------------------------------------------------------------

//! Create XYZ rotation matrix from Euler angles, given in degrees.
template <typename T>
glmat44<T>
make_rotation_xyz(const vec<3,T>& rotate_deg)
{
    static const T deg2rad(static_cast<T>(M_PI)/180);

    const T s0(sin(deg2rad*rotate_deg[0]));
    const T s1(sin(deg2rad*rotate_deg[1]));
    const T s2(sin(deg2rad*rotate_deg[2]));

    const T c0(cos(deg2rad*rotate_deg[0]));
    const T c1(cos(deg2rad*rotate_deg[1]));
    const T c2(cos(deg2rad*rotate_deg[2]));

    return glmat44<T>(
        c1*c2,           -c1*s2,             s1,    0,
        s0*s1*c2 + c0*s2, c0*c2 - s0*s1*s2, -s0*c1, 0,
        s0*s2 - c0*s1*c2, c0*s1*s2 + s0*c2,  c0*c1, 0,
        0,                0,                 0,     1);
}

// TODO: make_rotation_zyx
//       make_rotation_yxz
//       etc.

// -----------------------------------------------------------------------------

template <typename T>
glmat44<T>
make_translation(const vec<3,T>& translate)
{
#if 0
    const glmat44<T> m(
        1, 0, 0, translate[0],
        0, 1, 0, translate[1],
        0, 0, 1, translate[2],
        0, 0, 0, 1);
    const T *ptr(&m[0][0]);
    using std::cerr;
    cerr << "translate\n";
    cerr << ptr[0] << " " << ptr[4] << " " << ptr[8]  << " " << ptr[12] << "\n";
    cerr << ptr[1] << " " << ptr[5] << " " << ptr[9]  << " " << ptr[13] << "\n";
    cerr << ptr[2] << " " << ptr[6] << " " << ptr[10] << " " << ptr[14] << "\n";
    cerr << ptr[3] << " " << ptr[7] << " " << ptr[11] << " " << ptr[15] << "\n";
#endif

    return glmat44<T>(
        1, 0, 0, translate[0],
        0, 1, 0, translate[1],
        0, 0, 1, translate[2],
        0, 0, 0, 1);
}

// -----------------------------------------------------------------------------

template <typename T>
glmat44<T>
make_scale(const vec<3,T>& scale)
{
#if 0
    const glmat44<T> s(
        scale[0], 0,        0,        0,
        0,        scale[1], 0,        0,
        0,        0,        scale[2], 0,
        0,        0,        0,        1);

    const T *ptr(&s[0][0]);
    using std::cerr;
    cerr << "scale\n";
    cerr << ptr[0] << " " << ptr[4] << " " << ptr[8]  << " " << ptr[12] << "\n";
    cerr << ptr[1] << " " << ptr[5] << " " << ptr[9]  << " " << ptr[13] << "\n";
    cerr << ptr[2] << " " << ptr[6] << " " << ptr[10] << " " << ptr[14] << "\n";
    cerr << ptr[3] << " " << ptr[7] << " " << ptr[11] << " " << ptr[15] << "\n";
#endif

    return glmat44<T>(
        scale[0], 0,        0,        0,
        0,        scale[1], 0,        0,
        0,        0,        scale[2], 0,
        0,        0,        0,        1);
}

// -----------------------------------------------------------------------------

template<class T>
glmat44<T>
make_transform(const vec<3,T>& translate,
               const vec<3,T>& rotate,
               const vec<3,T>& scale)
{
#if 0
    glmat44<T> t(make_translation(translate));
    T *ptr = &t[0][0];
    using std::cerr;
    cerr << "translate\n";
    cerr << ptr[0] << " " << ptr[4] << " " << ptr[8]  << " " << ptr[12] << "\n";
    cerr << ptr[1] << " " << ptr[5] << " " << ptr[9]  << " " << ptr[13] << "\n";
    cerr << ptr[2] << " " << ptr[6] << " " << ptr[10] << " " << ptr[14] << "\n";
    cerr << ptr[3] << " " << ptr[7] << " " << ptr[11] << " " << ptr[15] << "\n";

    glmat44<T> r(make_rotation_xyz(rotate));
    ptr = &r[0][0];
    cerr << "rotate\n";
    cerr << ptr[0] << " " << ptr[4] << " " << ptr[8]  << " " << ptr[12] << "\n";
    cerr << ptr[1] << " " << ptr[5] << " " << ptr[9]  << " " << ptr[13] << "\n";
    cerr << ptr[2] << " " << ptr[6] << " " << ptr[10] << " " << ptr[14] << "\n";
    cerr << ptr[3] << " " << ptr[7] << " " << ptr[11] << " " << ptr[15] << "\n";

    glmat44<T> s(make_scale(scale));
    ptr = &s[0][0];
    cerr << "scale\n";
    cerr << ptr[0] << " " << ptr[4] << " " << ptr[8]  << " " << ptr[12] << "\n";
    cerr << ptr[1] << " " << ptr[5] << " " << ptr[9]  << " " << ptr[13] << "\n";
    cerr << ptr[2] << " " << ptr[6] << " " << ptr[10] << " " << ptr[14] << "\n";
    cerr << ptr[3] << " " << ptr[7] << " " << ptr[11] << " " << ptr[15] << "\n";


#endif

    return (make_translation(translate)*
            make_rotation_xyz(rotate)*
            make_scale(scale));
}

// -----------------------------------------------------------------------------

template<class T>
void
invert(const glmat44<T>& a, glmat44<T>& b)
{
    invert(a.to_mat44(), b.to_mat44());
}

// -----------------------------------------------------------------------------

//! TODO: deprecate!
template <typename T>
bool
project(const vec<3,T>&   obj,
        const vec4i&      vp,
        const glmat44<T>& mv,
        const glmat44<T>& p,
        vec<4,T>&         eye,
        vec<4,T>&         clip,
        vec<3,T>&         ndc,
        vec<3,T>&         win,
        const T           depth_range_near = 0, // As set with glDepthRange.
        const T           depth_range_far  = 1)
{
    // Apply modelview transformation, hard-coded matrix multiplication.
    // Assumes obj[3] = 1.

    eye[0] = mv[0][0]*obj[0] + mv[1][0]*obj[1] + mv[2][0]*obj[2] + mv[3][0];
    eye[1] = mv[0][1]*obj[0] + mv[1][1]*obj[1] + mv[2][1]*obj[2] + mv[3][1];
    eye[2] = mv[0][2]*obj[0] + mv[1][2]*obj[1] + mv[2][2]*obj[2] + mv[3][2];
    eye[3] = mv[0][3]*obj[0] + mv[1][3]*obj[1] + mv[2][3]*obj[2] + mv[3][3];

    // Use projection transformation to bring eye coords
    // into clip coords.

    clip[0] = p[0][0]*eye[0] + p[1][0]*eye[1] + p[2][0]*eye[2] + p[3][0]*eye[3];
    clip[1] = p[0][1]*eye[0] + p[1][1]*eye[1] + p[2][1]*eye[2] + p[3][1]*eye[3];
    clip[2] = p[0][2]*eye[0] + p[1][2]*eye[1] + p[2][2]*eye[2] + p[3][2]*eye[3];
    clip[3] = p[0][3]*eye[0] + p[1][3]*eye[1] + p[2][3]*eye[2] + p[3][3]*eye[3];

    // TODO: More robust check?!

    if (0 == clip[3]) {
        return false;
    }

    // Normalized device coordinates.

    const T inv_ndc_w(1/clip[3]);
    ndc[0] = clip[0]*inv_ndc_w;
    ndc[1] = clip[1]*inv_ndc_w;
    ndc[2] = clip[2]*inv_ndc_w;

    // Window (viewport) coordinates.

    const T diff(depth_range_far - depth_range_near);

    win[0] = vp[0] + 0.5*vp[2]*(ndc[0] + 1);
    win[1] = vp[1] + 0.5*vp[3]*(ndc[1] + 1);
    win[2] = depth_range_near + 0.5*depth_range_far*(ndc[2] + diff);

    return true;
}

// -----------------------------------------------------------------------------

//! xf: model transform.
//! cv: view transform
template <typename T>
bool
project(const vec<3,T>&   obj,
        const glmat44<T>& xf,
        const glmat44<T>& cv,
        const glmat44<T>& proj,
        const vec4i&      vp,
        vec<3,T>&         wsx,
        vec<3,T>&         eye,
        vec<4,T>&         clip,
        vec<3,T>&         ndc,
        vec<3,T>&         win,
        const T           depth_range_near = 0, // As set with glDepthRange.
        const T           depth_range_far  = 1)
{
    wsx = xf*obj;                                       // World space.
    eye = cv*wsx;                                       // Eye space.
    clip = proj*vec<4,T>(eye[0], eye[1], eye[2], 1);    // Clip space.

    if (0 == clip[3]) {
        return false;
    }

    // Normalized device coordinates.

    const T inv_ndc_w(1/clip[3]);
    ndc[0] = clip[0]*inv_ndc_w;
    ndc[1] = clip[1]*inv_ndc_w;
    ndc[2] = clip[2]*inv_ndc_w;

    // Window (viewport) coordinates.

    const T diff(depth_range_far - depth_range_near);

    win[0] = vp[0] + 0.5*vp[2]*(ndc[0] + 1);
    win[1] = vp[1] + 0.5*vp[3]*(ndc[1] + 1);
    win[2] = depth_range_near + 0.5*depth_range_far*(ndc[2] + diff);

    return true;
}

// -----------------------------------------------------------------------------

//! TODO: deprecate!
template <typename T>
void
projectEye(const glmat44<T>& mv,
           const vec<3,T>&   obj,
           vec<4,T>&         eye)
{
    eye = mv*vec<4,T>(obj[0], obj[1], obj[2], 1);
}

// -----------------------------------------------------------------------------

//! xf: model transform.
//! cv: view transform
template <typename T>
void
projectEye(const vec<3,T>&   obj,
           const glmat44<T>& xf,
           const glmat44<T>& cv,
           vec<3,T>&         wsx,
           vec<3,T>&         eye)
{
    wsx = xf*obj;
    eye = cv*wsx;
}

// -----------------------------------------------------------------------------

template <typename T>
void
projectEye(const glmat44<T>& mv,
           const vec<3,T>*   obj,
           vec<4,T>*         eye,
           const int         n,
           vec<4,T>&         eye_min,
           vec<4,T>&         eye_max)
{
    EM_ASSERT(obj != 0);
    EM_ASSERT(eye != 0);
    EM_ASSERT(n > 0);

    // Transform first object-coord into eye-space.

    projectEye(mv, obj[0], eye[0]);

    // Initialize bounds.

    eye_min = eye[0];
    eye_max = eye[0];

    for(int v(1); v < n; ++v) {
        projectEye(mv, obj[v], eye[v]);

        eye_min[0] = std::min(eye_min[0], eye[v][0]);
        eye_min[1] = std::min(eye_min[1], eye[v][1]);
        eye_min[2] = std::min(eye_min[2], eye[v][2]);
        eye_min[3] = std::min(eye_min[3], eye[v][3]);
        eye_max[0] = std::max(eye_max[0], eye[v][0]);
        eye_max[1] = std::max(eye_max[1], eye[v][1]);
        eye_max[2] = std::max(eye_max[2], eye[v][2]);
        eye_max[3] = std::max(eye_max[3], eye[v][3]);
    }
}

// -----------------------------------------------------------------------------

//! Compute object coords and inverse(projection*modelview)
template <typename T>
bool
unproject(const vec<3,T>&   win,
          const vec4i&      vp,
          const glmat44<T>& mv,
          const glmat44<T>& proj,
          vec<3,T>&         obj,
          glmat44<T>&       inv_pmv,
          const T           depth_range_near = 0, // As set with glDepthRange.
          const T           depth_range_far  = 1)
{
    const glmat44<T> pmv(proj*mv);
    invert(pmv, inv_pmv);

    return unproject(win, vp, inv_pmv, obj, depth_range_near, depth_range_far);
}

// -----------------------------------------------------------------------------

//! Uses inverse(projection*modelview) directly
template <typename T>
bool
unproject(const vec<3,T>&   win,
          const vec4i&      vp,
          const glmat44<T>& inv_pmv,
          vec<3,T>&         obj,
          const T           depth_range_near = 0, // As set with glDepthRange.
          const T           depth_range_far  = 1)
{
    EM_ASSERT(depth_range_far != 0);

    // Transformation to NDC, between -1 and 1.

    const T diff(depth_range_far - depth_range_near);
    const vec<4,T> ndc(
        (win[0] - vp[0])/vp[2]*2 - 1,
        (win[1] - vp[1])/vp[3]*2 - 1,
        (2*(win[2] - depth_range_near))/depth_range_far - diff,
        1);

    // Object coordinates.

    const vec<4,T> objw(inv_pmv*ndc);

    if (0 == objw[3]) {
        return false;
    }

    const T inv_w(1/objw[3]);

    obj[0] = objw[0]*inv_w;
    obj[1] = objw[1]*inv_w;
    obj[2] = objw[2]*inv_w;

    return true;
}

// -----------------------------------------------------------------------------

template <typename T>
void
unprojectEye(const glmat44<T>& inv_mv,
             const vec<4,T>&   eye,
             vec<3,T>&         obj)
{
    const vec<4,T> obj4(inv_mv*eye);
    obj = vec<3,T>(obj4[0], obj4[1], obj4[2]);
}

// -----------------------------------------------------------------------------

//! Compute frustum corners in world space.
template <typename T>
void
frustum(const glmat44<T>& mv,
        const glmat44<T>& proj,
        vec<3,T>          wsx[8])
{
    EM_ASSERT(wsx != 0);    // Check non-null output array.

    // Frustum vertices in NDC.

    static const vec<4,T> ndc[] =
    {
        vec<4,T>(-1, -1, -1, 1),  // Near
        vec<4,T>( 1, -1, -1, 1),
        vec<4,T>( 1,  1, -1, 1),
        vec<4,T>(-1,  1, -1, 1),
        vec<4,T>(-1, -1,  1, 1),  // Far
        vec<4,T>( 1, -1,  1, 1),
        vec<4,T>( 1,  1,  1, 1),
        vec<4,T>(-1,  1,  1, 1)
    };

    glmat44<T> inv_pmv(1);  // Identity.
    invert(proj*mv, inv_pmv);

    // Transform frustum vertices from NDC to world-space.

    for (int i(0); i< 8; ++i) {
        const vec<4,T> wsx4(inv_pmv*ndc[i]);
//        std::cerr << wsx4 << "\n";
//        EM_ASSERT(abs(wsx4[3]) > 0.);
        const T inv_w(1/wsx4[3]);
        wsx[i][0] = wsx4[0]*inv_w;
        wsx[i][1] = wsx4[1]*inv_w;
        wsx[i][2] = wsx4[2]*inv_w;
    }
}

// -----------------------------------------------------------------------------

//! Compute Euler angles from a rotation matrix, assumes
//! XYZ rotation order. Angles are given in degrees.
template <typename T>
vec<3,T>
euler_angles_xyz(const glmat44<T> &rot)
{
    static const T rad2deg(180/static_cast<T>(M_PI));

    return vec<3,T>(rad2deg*(atan2(-rot[2][1], rot[2][2])),
                    rad2deg*(asin(rot[2][0])),
                    rad2deg*(atan2(-rot[1][0], rot[0][0])));
}

// -----------------------------------------------------------------------------

//template <typename T>
//glmat44<T>
//make_cam_view_trans_rot_scale(const glmat44<T>& trans,
//                              const glmat44<T>& rot,
//                              const glmat44<T>& scale)
//{
//    return (scale*rot*trans);
//}

// TODO: make_cam_view_srt
//       make_cam_view_str
//       etc.

// -----------------------------------------------------------------------------

//! Create camera view matrix from translation, rotation, and scale. Assumes
//! XYZ rotation order. Also assumes the order in which transformations are
//! multiplied.
template <typename T>
glmat44<T>
make_cam_view(const vec<3,T> &translate,
              const vec<3,T> &rotate_deg,
              const vec<3,T> &scale)
{
    return (make_scale(scale)*
            make_rotation_xyz(-rotate_deg)*
            make_translation(-translate));
}

// -----------------------------------------------------------------------------

//! Create camera view matrix from right, up, back, and world space position.
template <typename T>
glmat44<T>
make_cam_view(const vec<3,T>& r,
              const vec<3,T>& u,
              const vec<3,T>& b,
              const vec<3,T>& wsx)
{
    return glmat44<T>(
        r[0], r[1], r[2], -(wsx[0]*r[0] + wsx[1]*r[1] + wsx[2]*r[2]),
        u[0], u[1], u[2], -(wsx[0]*u[0] + wsx[1]*u[1] + wsx[2]*u[2]),
        b[0], b[1], b[2], -(wsx[0]*b[0] + wsx[1]*b[1] + wsx[2]*b[2]),
        0,    0,    0,     1);
}

// -----------------------------------------------------------------------------

//! Construct camera view from arguments similar to those used in gluLookAt.
template <typename T>
glmat44<T>
make_cam_view_lookat(const vec<3,T>& wsx,
                     const vec<3,T>& ctr,
                     const vec<3,T>& wup = vec<3,T>(0, 1, 0))
{
    EM_ASSERT(em::mag2(wsx - ctr) > 0);
    const vec<3,T> back(em::normalized(wsx - ctr));
    EM_ASSERT(em::mag2(em::cross(wup, back)) > 0);
    const vec<3,T> right(em::normalized(em::cross(wup, back)));
    const vec<3,T> up(em::cross(back, right));

    return make_cam_view(right, up, back, wsx);
}

// -----------------------------------------------------------------------------

//! Compute the matrix used to transform normals given a modelview
//! matrix. The normal matrix is the transpose of the inverse of the
//! top left 3x3 of the modelview matrix.
template <typename T>
glmat33<T>
make_cam_normal_matrix(const glmat44<T>& mv)
{
    glmat33<T> nm(1);

    // NB: Transpose in assign.

    nm[0][0] =  (mv[1][1]*mv[2][2] - mv[1][2]*mv[2][1]);
    nm[0][1] = -(mv[1][0]*mv[2][2] - mv[2][0]*mv[1][2]);
    nm[0][2] =  (mv[1][0]*mv[2][1] - mv[2][0]*mv[1][1]);
    nm[1][0] = -(mv[0][1]*mv[2][2] - mv[2][1]*mv[0][2]);
    nm[1][1] =  (mv[0][0]*mv[2][2] - mv[2][0]*mv[0][2]);
    nm[1][2] = -(mv[0][0]*mv[2][1] - mv[0][1]*mv[2][0]);
    nm[2][0] =  (mv[0][1]*mv[1][2] - mv[0][2]*mv[1][1]);
    nm[2][1] = -(mv[0][0]*mv[1][2] - mv[0][2]*mv[1][0]);
    nm[2][2] =  (mv[0][0]*mv[1][1] - mv[0][1]*mv[1][0]);

    return nm;

//    glmat44<T> inv_mv(1); // Identity.
//    invert(mv, inv_mv);

//    // Transpose in construction.

//    return glmat33<T>(
//        inv_mv[0][0], inv_mv[0][1], inv_mv[0][2],
//        inv_mv[1][0], inv_mv[1][1], inv_mv[1][2],
//        inv_mv[2][0], inv_mv[2][1], inv_mv[2][2]);
}

// -----------------------------------------------------------------------------

//! Compute the matrix used to transform normals given model and view
//! matrices. The normal matrix is the transpose of the inverse of the
//! top left 3x3 of the modelview matrix.
template <typename T>
glmat33<T>
make_cam_normal_matrix(const glmat44<T>& xf,
                       const glmat44<T>& cv)
{
    return make_cam_normal_matrix(cv*xf);
}

// -----------------------------------------------------------------------------

template <typename T>
glmat44<T>
make_persp_projection(const T left,
                      const T right,
                      const T bottom,
                      const T top,
                      const T near,
                      const T far)
{
    EM_ASSERT(right > left);
    EM_ASSERT(top > bottom);
    EM_ASSERT(far > near);

    const T inv_rl(1/(right - left));
    const T inv_tb(1/(top - bottom));
    const T inv_fn(1/(far - near));

    glmat44<T> p;

    p[0][0] = (2*near)*inv_rl;          // Column 1.
    p[0][1] = 0;
    p[0][2] = 0;
    p[0][3] = 0;

    p[1][0] = 0;                        // Column 2.
    p[1][1] = (2*near)*inv_tb;
    p[1][2] = 0;
    p[1][3] = 0;

    p[2][0] = (right + left)*inv_rl;   // Column 3.
    p[2][1] = (top + bottom)*inv_tb;
    p[2][2] = -(far + near)*inv_fn;
    p[2][3] = -1;

    p[3][0] = 0;                        // Column 4.
    p[3][1] = 0;
    p[3][2] = -(2*far*near)*inv_fn;
    p[3][3] = 0;

    return p;
}

// -----------------------------------------------------------------------------

template <typename T>
glmat44<T>
make_ortho_projection(const T left,
                      const T right,
                      const T bottom,
                      const T top,
                      const T near,
                      const T far)
{
    EM_ASSERT(right > left);
    EM_ASSERT(top > bottom);
    EM_ASSERT(far > near);

    const T inv_rl(1/(right - left));
    const T inv_tb(1/(top - bottom));
    const T inv_fn(1/(far - near));

    glmat44<T> p;

    p[0][0] = 2*inv_rl;               // Column 1.
    p[0][1] = 0;
    p[0][2] = 0;
    p[0][3] = 0;

    p[1][0] = 0;                      // Column 2.
    p[1][1] = 2*inv_tb;
    p[1][2] = 0;
    p[1][3] = 0;

    p[2][0] = 0;                      // Column 3.
    p[2][1] = 0;
    p[2][2] = -2*inv_fn;
    p[2][3] = 0;

    p[3][0] = -(right + left)*inv_rl;   // Column 4.
    p[3][1] = -(top + bottom)*inv_tb;
    p[3][2] = -(far + near)*inv_fn;
    p[3][3] = 1;

    return p;
}

// -----------------------------------------------------------------------------

}  // namespace em

#endif  // EM_GLMAT44_ALGO
