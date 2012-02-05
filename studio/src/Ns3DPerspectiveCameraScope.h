// -----------------------------------------------------------------------------
//
// Ns3DPerspectiveCameraScope.h
//
// The interactive 3D Camera in the viewport.
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
//
// -----------------------------------------------------------------------------

#ifndef NS3D_PERSPECTIVECAMERASCOPE_H
#define NS3D_PERSPECTIVECAMERASCOPE_H

#include "Ns3DCameraScope.h"
#include "Ns3DBBox.h"
#include "NsQuery.h"
#include <NglViewport.h>
#include <NgBodyOp.h>
#include <NgInput.h>
#include <NbTimeBundle.h>
#include <cmath>

// -----------------------------------------------------------------------------

// Ns3DPerspectiveCameraScope
// --------------------------
//! Perspective camera scope.

class Ns3DPerspectiveCameraScope : public Ns3DCameraScope
{
public:

    //! CTOR.
    Ns3DPerspectiveCameraScope()
        : Ns3DCameraScope()
        , _hasBody(false)
        , _dragStartX(0)
        , _dragStartY(0)
        , _dragStartDist(1.f)
        , _dragStartPivot(0.f)
        , _modelview(1.f)  // Identity.
        , _projection(1.f)  // Identity.
    {}

    //! DTOR.
    virtual
    ~Ns3DPerspectiveCameraScope()
    {}

    virtual NtString
    typeName() const
    { return "Camera-Scope"; }

public:

    virtual const GLfloat*
    modelview() const
    { return &_modelview[0][0]; }

    virtual const GLfloat*
    projection() const
    { return &_projection[0][0]; }

    virtual const Ngl::mat44f&
    modelviewMat() const
    { return _modelview; }

    virtual const Ngl::mat44f&
    projectionMat() const
    { return _projection; }


public:     // Mouse events.

    virtual void
    initDrag(const int mx, const int my,
             const Ngl::Viewport &vp)
    {
        const Nb::TimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
        _dragStartX = mx;
        _dragStartY = my;
        _dragStartDist = orbitRadius(cvftb);
        _dragStartPivot = pivot(worldPos(), back(), _dragStartDist);
    }


    virtual void
    mouseLeftDrag(const int mx0, const int my0,
                  const int mx1, const int my1,
                  const Ngl::Viewport &vp)
    { 
        if (!_hasBody) {
            _rotate(mx0, my0, mx1, my1, vp);
        }
    }

    virtual void
    mouseMidDrag(const int mx0, const int my0,
                 const int mx1, const int my1,
                 const Ngl::Viewport &vp)
    { 
        if (!_hasBody) {
            _track(mx0, my0, mx1, my1, vp); 
        }
    }

    virtual void
    mouseRightDrag(const int mx0, const int my0,
                   const int mx1, const int my1,
                   const Ngl::Viewport &vp)
    { 
        if (!_hasBody) {
            _dolly(mx0, my0, mx1, my1, vp); 
        }
    }

    // Keyboard events.

    virtual void
    frame(/*const NtVec3f       &sceneMin,
          const NtVec3f       &sceneMax,*/
          const Ns3DBBox      &bbox,
          const Ngl::Viewport &vp)
    {
        if (!_hasBody) {
            _frame(/*sceneMin, sceneMax,*/ bbox, vp);
        }
    }


    virtual void 
    updateModelview();

    virtual void 
    updateProjection(const Ngl::Viewport &vp);

    virtual void 
    loadBody(const Ngl::Viewport &vp);

    //! [deprecated]
    virtual float
    angleOfView(const Nb::TimeBundle &tb) const
    { return getParam1f("Angle Of View", tb); }

private:

    void 
    _rotate(int mx0, int my0,
            int mx1, int my1,
            const Ngl::Viewport &vp);

    void 
    _track(int mx0, int my0,
           int mx1, int my1,
           const Ngl::Viewport &vp);

    void 
    _dolly(int mx0, int my0,
           int mx1, int my1,
           const Ngl::Viewport &vp);

    void 
    _frame(/*const NtVec3f       &sceneMin,
           const NtVec3f       &sceneMax,*/
           const Ns3DBBox      &bbox,
           const Ngl::Viewport &vp);

private:

    //! Transfer expressions between two values. Useful for grabbing camera
    //! settings from bodies.
    static void
    _transfer1f(const Nb::Value1f *src, Nb::Value1f *dst)
    {
        dst->setExpr(src->expr(0), 0);
    }

    //! Transfer expressions between two values. Useful for grabbing camera
    //! settings from bodies.
    static void
    _transfer3f(const Nb::Value3f *src, Nb::Value3f *dst)
    {
        dst->setExpr(src->expr(0), 0);
        dst->setExpr(src->expr(1), 1);
        dst->setExpr(src->expr(2), 2);
    }

    static GLfloat 
    _maxElementAbsDiff(const Ngl::mat44f &a, const Ngl::mat44f &b)
    {
        GLfloat maxDiff = -(std::numeric_limits<GLfloat>::max)();
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                maxDiff = std::max<float>(maxDiff, fabs(a[i][j] - b[i][j]));
            }
        }
        return maxDiff;
    }

    //! Clamp to [1..179].
    static double
    _angleOfView(const double aperture, const double focalLength)
    {
        return std::max<double>(
            Ngl::deg2rad(1),
            std::min<double>(2*atan(aperture/(2*focalLength)), 
                             Ngl::deg2rad(179)));
    }

    static double
    _focalLength(const double angleOfView)
    { return 1./tan(0.5*angleOfView); }

private:    // Member variables.

    bool _hasBody;
    int     _dragStartX;
    int     _dragStartY;
    float   _dragStartDist;
    NtVec3f _dragStartPivot;

    Ngl::mat44f   _modelview;   //!< Column-major 4x4 matrix.
    Ngl::mat44f   _projection;  //!< Column-major 4x4 matrix.
};

// -----------------------------------------------------------------------------

// updateModelview
// ---------------
//! Make sure the modelview matrix can be plugged straight into OpenGL.

void
Ns3DPerspectiveCameraScope::updateModelview()
{
    if (_hasBody) {
        return;
    }

    const Nb::TimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
    _modelview = 
        em::make_cam_view(
            translate(cvftb), 
            rotate(cvftb), 
            scale(cvftb));

#if 0

    const float *mv = &_modelview[0][0];
    qDebug() << mv[0] << mv[4] << mv[8] << mv[12];
    qDebug() << mv[1] << mv[5] << mv[9] << mv[13];
    qDebug() << mv[2] << mv[6] << mv[10] << mv[14];
    qDebug() << mv[3] << mv[7] << mv[11] << mv[15];


    NtVec3f ea(
        (180.f/M_PI)*(atan2(-xfr[2][1], xfr[2][2])),
        (180.f/M_PI)*(asin(xfr[2][0])),
        (180.f/M_PI)*(atan2(-xfr[1][0], xfr[0][0])));

    qDebug() << "Translate:" << translate(ctb)[0] << translate(ctb)[1] << translate(ctb)[2];
    qDebug() << "wsx      :" << worldPos()[0] << worldPos()[1] << worldPos()[2];
    qDebug() << "Rotate:" << rot[0] << rot[1] << rot[2];
    qDebug() << "EA    :" << -ea[0] << -ea[1] << -ea[2];
    qDebug() << "R:" << right()[0] << right()[1] << right()[2]
            << "(" << em::mag(right()) << ")";
    qDebug() << "U:" << up()[0]    << up()[1]    << up()[2]
            << "(" << em::mag(up()) << ")";
    qDebug() << "B:" << back()[0]  << back()[1]  << back()[2]
            << "(" << em::mag(back()) << ")";
    const NtVec3f piv(_pivot(worldPos(), back(), orbitRadius(ctb)));
    qDebug() << "Pivot:" << piv[0] << piv[1] << piv[2];
#endif
}


// updateProjection
// ----------------
//! Make sure the projection matrix can be plugged straight into OpenGL.

void
Ns3DPerspectiveCameraScope::updateProjection(const Ngl::Viewport& vp)
{
    if (_hasBody) {
        return;
    }

    // Grab parameter values. 
    // NB: Aperture values are in inches!

    const Nb::TimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
    const double focalLengthMM = getParam1f("Focal Length", cvftb); // [mm]
    const double horizontalAperture = getParam1f("Horizontal Aperture", cvftb);
    const double verticalAperture = getParam1f("Vertical Aperture", cvftb);    
    const double n = nearClip(cvftb);
    const double f = farClip(cvftb);

    const double hApertureMM = 25.4*horizontalAperture; // 1 inch = 25.4 mm.
    const double hAOV = _angleOfView(hApertureMM, focalLengthMM);
    const double vApertureMM = 25.4*verticalAperture;   // 1 inch = 25.4 mm.
    const double vAOV = _angleOfView(vApertureMM, focalLengthMM);
    const double e = _focalLength(hAOV); // NB: Horizontal!
    const double a = (static_cast<double>(vp.height())/vp.width());

    GLfloat p[16];
    p[0] = e; p[4] = 0;   p[8] = 0;                   p[12] = 0;
    p[1] = 0; p[5] = e/a; p[9] = 0;                   p[13] = 0;
    p[2] = 0; p[6] = 0;   p[10] = -((f + n)/(f - n)); p[14] = (-2*f*n)/(f - n);
    p[3] = 0; p[7] = 0;   p[11] = -1;                 p[15] = 0;
    _projection = em::glmat44f(p);

#if 0
    std::cerr << "Focal Length: " << focalLength << " [mm]\n";
    std::cerr << "Horizontal Angle of View: " << Ngl::rad2deg(hAOV) << " [deg]\n";
    std::cerr << "Vertical Angle of View: " << Ngl::rad2deg(vAOV) << " [deg]\n";
#endif

    // HZ intrinsic pin-hole camera params.
    //const double fx = 1/tan(0.5f*hAOV);
    //const double fy = 1/tan(0.5f*vAOV);
    //const double s = 0;    // Skew, not used.
    //const double cx = 0;   // Principal point x.
    //const double cy = 0;   // Principal point y.

    //std::cerr << "fx: " << fx << "\n";
    //std::cerr << "fy: " << fy << "\n";

    //const double x0 = vp.x();
    //const double y0 = vp.y();
    //const double w = vp.width();
    //const double h = vp.height();

    //GLfloat p[16];
    //p[0] = 2*fx/w; p[4] = -2*s/w;   p[8] = 0;/*(w - 2*cx + 2*x0)/w*/;  p[12] = 0; 
    //p[1] = 0;    p[5] = (2*fy/h)*a; p[9] = 0;/*(h - 2*cy + 2*y0)/h*/;  p[13] = 0; 
    //p[2] = 0;    p[6] = 0;       p[10] = -(f + n)/(f - n);    p[14] = (-2*f*n)/(f - n); 
    //p[3] = 0;    p[7] = 0;       p[11] = -1;                  p[15] = 0; 
    //_projection = em::glmat44f(p);
}


// loadBody
// --------
//! Set camera parameters from a camera body.

void
Ns3DPerspectiveCameraScope::loadBody(const Ngl::Viewport& vp)
{
    _hasBody = false;
    ssHud.str("");
    NsOpObject *op = NsOpStore::instance()->queryMutableOp(fromNbStr(name()));
    if (0 != op && "ACTIVE" == op->state()) {
        const Nb::TimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
        foreach (NsBodyInputPlugObject *bipo, op->mutableBodyInputs()) {
            if (0 != bipo->source()) {
                NsPlugObject *po =
                    NsOpStore::instance()->queryMutablePlug(
                        fromNbStr((bipo->source()->longname())));
                if (0 != po) {
                    QList<NsBodyObject*> bodies;
                    if (queryIsFrameLive(cvftb.frame)) {
                        // Current visible frame is the live frame. 

                        bodies = po->mutableLiveBodies();
                    }
                    else {
                        // Current visible frame is not the live frame. 

                        bodies = po->mutableOp()->mutableCachedBodies();
                    }

                    const NtString camBody =
                        param1s("Camera Body")->eval(cvftb);

                    foreach (NsBodyObject *body, bodies) {
                        if (admitBody(*body, *bipo, fromNbStr(camBody))) {
                            // Camera body with matching name detected.
                            // Set camera parameters provided in camera body.

                            Nb::Body &nbBody(body->nbBody());

                            // Get modelview parameters.

                            _transfer3f(nbBody.prop3f("Translate"),
                                        param3f("Translate"));
                            _transfer3f(nbBody.prop3f("Rotate"),
                                        param3f("Rotate"));
                            _transfer3f(nbBody.prop3f("Scale"),
                                        param3f("Scale"));
                            _modelview = nbBody.globalMatrix;
                            //updateModelview();

                            // Check that we compute modelview matrix 
                            // the same way that Maya does.

                            const GLfloat maxAbsDiff = 
                                _maxElementAbsDiff(
                                    _modelview, 
                                    Ngl::mat44f(nbBody.globalMatrix));
                            if (maxAbsDiff > 0.01f) {
                                NsMessageWidget::instance()->clientWarning(
                                    "Camera body global matrix does not "
                                    "match parameters.");
                            }

                            // Get projection parameters.

                            _transfer1f(nbBody.prop1f("Near Clip"),
                                        param1f("Near Clip"));
                            _transfer1f(nbBody.prop1f("Far Clip"),
                                        param1f("Far Clip"));
                            _transfer1f(nbBody.prop1f("Focal Length"),
                                        param1f("Focal Length"));
                            _transfer1f(nbBody.prop1f("Horizontal Aperture"),
                                        param1f("Horizontal Aperture"));
                            _transfer1f(nbBody.prop1f("Vertical Aperture"),
                                        param1f("Vertical Aperture"));
                            updateProjection(vp);

                            ssHud << "Body: '" << nbBody.name() << "'\n";
                            _hasBody = true; // Cam params were set from body.
                        }
                    }
                }    
            }
        }
    }
}


// rotate
// ------
//! Rotate the view.
//! [x0, y0] are the pixel coordinates at which the drag started.
//! [dx, dy] are the distances (in pixels) the mouse has moved since
//! last call.

void
Ns3DPerspectiveCameraScope::_rotate(const int mx0, const int my0,
                                    const int mx1, const int my1,
                                    const Ngl::Viewport &vp)
{
    const Nb::TimeBundle ctb = currentTime();
    const float ms = mouseSensitivity(ctb);
    const float ndx = (ms*(mx1 - mx0))/vp.width();
    const float ndy = (ms*(my1 - my0))/vp.height();
    const float mouseSpeed = std::min(180.f, 180.f*ms);
    const NtVec3f wsx0 = worldPos();
    const NtVec3f bdir = back();
    const float orb = orbitRadius(ctb);

    const NtVec3f piv(pivot(wsx0, bdir, orb));

    // Get spherical angles of camera position with respect to pivot.
    const NtVec3f sphDeg0(sphericalAngles(wsx0, piv));

    const NtVec3f sphDeg1(
        sphDeg0[0] + mouseSpeed*ndy,
        sphDeg0[1] + mouseSpeed*ndx,
        sphDeg0[2]);

    const NtVec3f wsx1(
        sphericalPosition(
            em::clamp(sphDeg1[0], -89.f, 89.f), // Phi.
            sphDeg1[1],  // theta
            orb,
            piv));

    // Look at the pivot from the new camera position and extract
    // resulting Euler angles.

    const em::glmat44f mv(
        em::make_cam_view_lookat(wsx1, piv));

    const NtVec3f ea(em::euler_angles_xyz(mv));

    // Update camera world position and rotation.

    setParam3f("Rotate", -ea[0], -ea[1], -ea[2]);
    setParam3f("Translate", wsx1[0], wsx1[1], wsx1[2]);

    updateModelview();
}


// track
// ------
//! Move camera in view plane.
//! [x, y]   are the current pixel coordinates of the mouse.
//! [dx, dy] are the distances (in pixels) the mouse has moved since
//! last call.

void
Ns3DPerspectiveCameraScope::_track(const int mx0, const int my0,
                                   const int mx1, const int my1,
                                   const Ngl::Viewport &vp)
{
    const Nb::TimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
    const float rad = orbitRadius(cvftb);
    const float n = nearClip(cvftb);
    const float f = farClip(cvftb);
    const float ms = mouseSensitivity(cvftb);
    float td0 = 0;
    float td1 = 0;
    if (trackDistance(Ngl::vec3f(0, 0, -rad),   // Pivot plane origin.
                      Ngl::vec3f(0, 0, 1),      // Pivot plane normal.
                      em::glmat44f(1.f),        // Identify.
                      _projection,
                      vp,
                      mx0, my0,
                      mx1, my1,
                      n, f,
                      td0, td1)) {
        const float wdx = ms*td0;
        const float wdy = ms*td1;
        const NtVec3f t1 = 
            getParam3f("Translate", cvftb) + wdx*right() + wdy*up(); // New pos.
        setParam3f("Translate", t1[0], t1[1], t1[2]);
        updateModelview();
    }
}


// dolly
// ------
//! Move camera in view direction.
//! [x0, y0] are the pixel coordinates at which the drag started.
//! [dx, dy] are the distances (in pixels) the mouse has moved since
//! last call.

void
Ns3DPerspectiveCameraScope::_dolly(const int mx0, const int my0,
                                   const int mx1, const int my1,
                                   const Ngl::Viewport &vp)
{
    const Nb::TimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
    const float near = nearClip(cvftb);
    const float ms = mouseSensitivity(cvftb);
    const float ndx = (ms*(_dragStartX - mx1))/vp.width();
    const float ndy = (ms*(_dragStartY - my1))/vp.height();
    const float vx = exp(ndx) - 1;
    const float vy = exp(ndy) - 1;
    const float orb1 = std::max(2.f*near, _dragStartDist*(1 + (vx - .5f*vy)));
    const NtVec3f t1 = _dragStartPivot + orb1*back();  // New world space pos.
    setParam3f("Translate", t1[0], t1[1], t1[2]);
    setParam1f("Orbit Radius", orb1);
    updateModelview();
}


// _frame
// --------
//! Use the provided bbox to compute a new camera position. Currently this
//! function does not take perspective projection into account, it should do so
//! in the future. The position and orbit radius of the camera are set so that
//! the pivot is placed at the center of the provided bounding box. The
//! camera is positioned outside the bounding box.
//! TODO: Use projection to do framing in viewport coords. This is why we
//!       still pass in the viewport, even though it is not used at the moment.

void
Ns3DPerspectiveCameraScope::_frame(/*const NtVec3f       &sceneMin,
                                   const NtVec3f       &sceneMax,*/
                                   const Ns3DBBox      &bbox,
                                   const Ngl::Viewport &vp)
{
    const NtVec3f mid(bbox.midX(), bbox.midY(), bbox.midZ());
    const NtVec3f dims(
        std::max<float>(1.f, bbox.dimX()), 
        std::max<float>(1.f, bbox.dimY()), 
        std::max<float>(1.f, bbox.dimZ()));
    const float orb1 = 5.f*em::mag(0.5f*dims);  // New orbit radius.
    const NtVec3f wsx1 = mid + orb1*back();     // New position.
    setParam3f("Translate", wsx1[0], wsx1[1], wsx1[2]);
    setParam1f("Orbit Radius", orb1);
    updateModelview();
    //updateProjection(vp);
}

#endif // NS3D_PERSPECTIVECAMERASCOPE_H
