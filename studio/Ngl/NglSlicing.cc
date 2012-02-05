// -----------------------------------------------------------------------------
//
// NglSlicing.cc
//
// Methods for slicing.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved. 
//
// This file is part of Open Naiad Studio..
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of Exotic Matter AB nor its contributors may be used to
// endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// -----------------------------------------------------------------------------

#include "NglSlicing.h"
#include "NglViewport.h"
#include "NglUtils.h"
#include <em_glmat44.h>
#include <em_glmat44_algo.h>



namespace Ngl
{
// -----------------------------------------------------------------------------

//// projectEye
//// ----------
////! Takes an array of object coordinates and projects them into eye-space using
////! the provided modelview matrix.
////! Assumptions:
////! -----
////! (1) Object coords have W = 1.
////! (2) Fourth row of XF is [0 0 0 1]
////! (3) World-space coords have W = 1.
////! (4) Fourth row of MV matrix is [0 0 0 1]
////! -----
////! Returns bounding volume in eye-space.

//void
//projectEye(const em::glmat44f& xf,
//           const em::glmat44f& mv,
//           const em::vec3f*    obj,
//           const int           n,
//           em::vec3f*          eyeMin,
//           em::vec3f*          eyeMax)
//{
//    if (0 == obj || n <= 0) {
//        return;
//    }

//    // Compute transformation from object to eye-coords.

//    const em::glmat44f oe(mv*xf);

//    // Transform first object-coord into eye-space.

//    em::vec<3,GLfloat> eye(oe*obj[0]);

//    // Initialize bounds.

//    *eyeMin = eye;
//    *eyeMax = eye;

//    for(int v(1); v < n; ++v) {
//        eye = oe*obj[v];

//        (*eyeMin)[0] = std::min((*eyeMin)[0], eye[0]);
//        (*eyeMin)[1] = std::min((*eyeMin)[1], eye[1]);
//        (*eyeMin)[2] = std::min((*eyeMin)[2], eye[2]);
//        (*eyeMax)[0] = std::max((*eyeMax)[0], eye[0]);
//        (*eyeMax)[1] = std::max((*eyeMax)[1], eye[1]);
//        (*eyeMax)[2] = std::max((*eyeMax)[2], eye[2]);
//    }
//}


//// unprojectEye
//// ------------
////! Transform eye-coords into world-space given inverse modelview matrix.
////! Ignores fourth row of matrix, which is assumed to be [0 0 0 1]. The
////! Returned world-space coord is assumed to have W = 1.

//void
//unprojectEye(const em::vec<3,GLfloat>& eye,
//             const em::glmat44f&       imv,
//             em::vec<3,GLfloat>*       wsx)
//{
//    *wsx = imv*eye;

////    (*wsx)[0] = imv[0]*eye[0] + imv[4]*eye[1] + imv[ 8]*eye[2] + imv[12];
////    (*wsx)[1] = imv[1]*eye[0] + imv[5]*eye[1] + imv[ 9]*eye[2] + imv[13];
////    (*wsx)[2] = imv[2]*eye[0] + imv[6]*eye[1] + imv[10]*eye[2] + imv[14];
//}


//// projectWin
//// ----------
////! Project world-space coords to window-coords.
//
//void
//projectWin(const GLfloat*            mv,
//           const GLfloat*            p,
//           const Ngl::Viewport&      vp,
//           const em::vec<3,GLfloat>* wsx,
//           GLint                     n)
//{
//    if (0 == obj || n <= 0) {
//        return;
//    }
//
//    GLfloat pmv[16];
//    multMat44f(p, mv, pmv);
//
//    em::vec<3,GLfloat> clip;
//
//
//}


// slice
// -----
//! Generate view-aligned quads from eye-space bounding box.

int
slice(const em::glmat44f&     mv,
      const em::vec4f&        eyeMin,
      const em::vec4f&        eyeMax,
      const int               count,
      std::vector<em::vec3f>* quadWsx,
      float*                  spacing)
{
    if (count <= 0 || 0 == quadWsx /*|| eyeMin[2] > 0.f*/) {
        // If min eye-space Z-bound is positive everything is behind
        // the camera.

        return 0;
    }

    // Vector quad vertices, 4 vertices per quad.

    quadWsx->resize(4*count);    // May throw.

    const em::vec4f quadEye0[] = {
        em::vec4f(eyeMin[0], eyeMin[1], eyeMax[2], 1),
        em::vec4f(eyeMax[0], eyeMin[1], eyeMax[2], 1),
        em::vec4f(eyeMax[0], eyeMax[1], eyeMax[2], 1),
        em::vec4f(eyeMin[0], eyeMax[1], eyeMax[2], 1)
    };

    // Compute transformation for unprojection.

    em::glmat44f invMv;
    em::invert(mv, invMv);

    // Unproject first quad, the closest to the camera.

    for(int v(0); v < 4; ++v) {
        em::unprojectEye(invMv, quadEye0[v], (*quadWsx)[v]);
        //eyeWsx0[v] = imv*eye0[v];
        //unprojectEye(eye0[v], &imv[0][0], &eyeWsx0[v]);
        //(*quadWsx)[v] = eyeWsx0[v];
    }

    if (0 != spacing) {
        *spacing = 0.f;
    }

    if (count > 1) {
        // If more than one quad compute the rest in parallel.

        const float eyeSliceSpacing((eyeMax[2] - eyeMin[2])/(count - 1));

        if (0 != spacing) {
            *spacing = eyeSliceSpacing;
        }

        #pragma omp parallel
        {
            //em::vec3f eyeWsx[4];

            int s;

            #pragma omp for
            for (s = 1; s < count; ++s) {
                const float eyeZ(eyeMax[2] - s*eyeSliceSpacing);
                const em::vec4f quadEye[] = {
                    em::vec4f(eyeMin[0], eyeMin[1], eyeZ, 1),
                    em::vec4f(eyeMax[0], eyeMin[1], eyeZ, 1),
                    em::vec4f(eyeMax[0], eyeMax[1], eyeZ, 1),
                    em::vec4f(eyeMin[0], eyeMax[1], eyeZ, 1)
                };

                for (int v(0); v < 4; ++v) {
                    em::unprojectEye(invMv, quadEye[v], (*quadWsx)[4*s + v]);

                    //eyeWsx[v] = imv*eye[v];
                    //unprojectEye(eye[v], &imv[0][0], &eyeWsx[v]);
                    //(*quadWsx)[4*s + v] = eyeWsx[v];
                }
            }
        }
    }

    return count;
}



//em::vec<3,GLfloat> wsx;     // World-space.
//em::vec<3,GLfloat> eye;     // Eye-coords.
////    em::vec<4,GLfloat> clip;    // Clip-space.
////    em::vec<3,GLfloat> ndc;     // Normalized device coords.
////    em::vec<3,GLfloat> win;     // Window coords.
//
//(*eyeMin)[0] =  (std::numeric_limits<GLfloat>::max)();
//(*eyeMin)[1] =  (std::numeric_limits<GLfloat>::max)();
//(*eyeMin)[2] =  (std::numeric_limits<GLfloat>::max)();
////(*eyeMin)[3] =  (std::numeric_limits<GLfloat>::max)();
//(*eyeMax)[0] = -(std::numeric_limits<GLfloat>::max)();
//(*eyeMax)[1] = -(std::numeric_limits<GLfloat>::max)();
//(*eyeMax)[2] = -(std::numeric_limits<GLfloat>::max)();
////(*eyeMax)[3] = -(std::numeric_limits<GLfloat>::max)();
//
////    (*winMin)[0] =  (std::numeric_limits<GLfloat>::max)();
////    (*winMin)[1] =  (std::numeric_limits<GLfloat>::max)();
////    (*winMin)[2] =  (std::numeric_limits<GLfloat>::max)();
////    (*winMax)[0] = -(std::numeric_limits<GLfloat>::max)();
////    (*winMax)[1] = -(std::numeric_limits<GLfloat>::max)();
////    (*winMax)[2] = -(std::numeric_limits<GLfloat>::max)();
//
//for(int v(0); v < n; ++v) {
//
//    wsx[0] = xf[0]*obj[v][0] + xf[4]*obj[v][1] + xf[ 8]*obj[v][2] + xf[12];
//    wsx[1] = xf[1]*obj[v][0] + xf[5]*obj[v][1] + xf[ 9]*obj[v][2] + xf[13];
//    wsx[2] = xf[2]*obj[v][0] + xf[6]*obj[v][1] + xf[10]*obj[v][2] + xf[14];
//    //wsx[3] = xf[3]*objv[0] + xf[7]*objv[1] + xf[11]*objv[2] + xf[15];
//
//    eye[0] = mv[0]*wsx[0] + mv[4]*wsx[1] + mv[ 8]*wsx[2] + mv[12];
//    eye[1] = mv[1]*wsx[0] + mv[5]*wsx[1] + mv[ 9]*wsx[2] + mv[13];
//    eye[2] = mv[2]*wsx[0] + mv[6]*wsx[1] + mv[10]*wsx[2] + mv[14];
//    //eye[3]=mv[3]*wsx[0]+mv[7]*wsx[1]+mv[11]*wsx[2]+mv[15];
//
//    (*eyeMin)[0] = std::min((*eyeMin)[0], eye[0]);
//    (*eyeMin)[1] = std::min((*eyeMin)[1], eye[1]);
//    (*eyeMin)[2] = std::min((*eyeMin)[2], eye[2]);
//    //(*eyeMin)[3] = std::min((*eyeMin)[3], eye[3]);
//    (*eyeMax)[0] = std::max((*eyeMax)[0], eye[0]);
//    (*eyeMax)[1] = std::max((*eyeMax)[1], eye[1]);
//    (*eyeMax)[2] = std::max((*eyeMax)[2], eye[2]);
//    //(*eyeMax)[3] = std::max((*eyeMax)[3], eye[3]);
//
////        clip[0]=p[0]*eye[0]+p[4]*eye[1]+p[ 8]*eye[2]+p[12]*eye[3];
////        clip[1]=p[1]*eye[0]+p[5]*eye[1]+p[ 9]*eye[2]+p[13]*eye[3];
////        clip[2]=p[2]*eye[0]+p[6]*eye[1]+p[10]*eye[2]+p[14]*eye[3];
////        clip[3]=p[3]*eye[0]+p[7]*eye[1]+p[11]*eye[2]+p[15]*eye[3];
//
////        (*eyeMin)[0] = std::min((*eyeMin)[0], clip[0]);
////        (*eyeMin)[1] = std::min((*eyeMin)[1], clip[1]);
////        (*eyeMin)[2] = std::min((*eyeMin)[2], clip[2]);
////        (*eyeMin)[3] = std::min((*eyeMin)[3], clip[3]);
////        (*eyeMax)[0] = std::max((*eyeMax)[0], clip[0]);
////        (*eyeMax)[1] = std::max((*eyeMax)[1], clip[1]);
////        (*eyeMax)[2] = std::max((*eyeMax)[2], clip[2]);
////        (*eyeMax)[3] = std::max((*eyeMax)[3], clip[3]);
//
//
////        const GLfloat invClipW(1.f/clip[3]);
////
////        ndc[0]=clip[0]*invClipW;
////        ndc[1]=clip[1]*invClipW;
////        ndc[2]=clip[2]*invClipW;
////
////        win[0]=vp.x() + (0.5f*vp.width())*(ndc[0] + 1.f);
////        win[1]=vp.y() + (0.5f*vp.height())*(ndc[1] + 1.f);
////        win[2]=0.5f*(ndc[2] + 1.f);
////
////        (*winMin)[0] = std::min((*winMin)[0], win[0]);
////        (*winMin)[1] = std::min((*winMin)[1], win[1]);
////        (*winMin)[2] = std::min((*winMin)[2], win[2]);
////        (*winMax)[0] = std::max((*winMax)[0], win[0]);
////        (*winMax)[1] = std::max((*winMax)[1], win[1]);
////        (*winMax)[2] = std::max((*winMax)[2], win[2]);



//
//
//bool
//unproject(const vec3f    &win,
//          const Viewport &vp,
//          const GLfloat  *invPMV,
//          vec3f          *obj)
//{
//    // Transformation to normalized coordinates between -1 and 1
//
//    const vec4f ndc(
//        (win[0] - vp.x())/vp.width()*2.f - 1.f,
//        (win[1] - vp.y())/vp.height()*2.f - 1.f,
//        2.f*win[2] - 1.f,
//        1.f);
//
//    // Object coordinates
//
//    vec4f objW;
//    multMat44fVec4f(invPMV, ndc, &objW);
//    if (0.f == objW[3])
//       return false;
//
//    objW[3] = 1.f/objW[3];
//
//    (*obj)[0] = objW[0]*objW[3];
//    (*obj)[1] = objW[1]*objW[3];
//    (*obj)[2] = objW[2]*objW[3];
//
//    return true;
//}




// -----------------------------------------------------------------------------
}   // Namespace: Ngl.
