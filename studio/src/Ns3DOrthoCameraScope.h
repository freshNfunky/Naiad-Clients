// -----------------------------------------------------------------------------
//
// Ns3DOrthoCameraScope.h
//
// The interactive 3D ortho camera in the viewport.
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

#ifndef NS_3D_ORTHO_CAMERA_SCOPE_H
#define NS_3D_ORTHO_CAMERA_SCOPE_H

#include "NsStringUtils.h"
#include "Ns3DCameraScope.h"
#include "Ns3DBBox.h"
#include "NsCmdSetParam.h"
#include "NsCmdSetParam3.h"

#include <NgBodyOp.h>
#include <NgInput.h>

#include <NbTimeBundle.h>

#include "Ns3DBodyScope.h"

#include <QHash>
#include <QString>

// -----------------------------------------------------------------------------

// Ns3DOrthoCameraScope
// --------------------
//! Ortho camera scope.

class Ns3DOrthoCameraScope : public Ns3DCameraScope
{
public:

    //! CTOR.
    Ns3DOrthoCameraScope()
        : Ns3DCameraScope()
        , _viewName("Front")
        , _dragStartX(0)
        , _dragStartY(0)
        , _dragStartZoom(1.f)
        , _modelview(1.f)   // Identity.
        , _projection(1.f)  // Identity.
    {
        //updateModelview();
        //updateProjection();
    }


    //! DTOR.
    virtual 
    ~Ns3DOrthoCameraScope() 
    {}

    virtual NtString
    typeName() const
    { return "Ortho-Camera-Scope"; }

    virtual const GLfloat *
    modelview() const
    { return &_modelview[0][0]; }

    virtual const GLfloat *
    projection() const
    { return &_projection[0][0]; }

    virtual const Ngl::mat44f &
    modelviewMat() const
    { return _modelview; }

    virtual const Ngl::mat44f &
    projectionMat() const
    { return _projection; }

    virtual void
    hudInfo(std::stringstream *ss)
    {
        *ss << typeName() << ": '" << name() << "'\n";
    }

public:     // Mouse events.

    virtual void
    initDrag(const int mx, const int my, 
             const Ngl::Viewport &vp)
    {
        const Nb::TimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
        _dragStartX = mx;
        _dragStartY = my;
        _dragStartZoom = zoomFactor(cvftb);
    }

    virtual void
    mouseLeftDrag(const int mx0, const int my0,
                  const int mx1, const int my1,
                  const Ngl::Viewport &vp)
    {}  // _dolly(ndx, ndy, vp, bbox);

    virtual void
    mouseMidDrag(const int mx0, const int my0,
                 const int mx1, const int my1,
                 const Ngl::Viewport &vp)
    { _track(mx0, my0, mx1, my1, vp); }

    virtual void
    mouseRightDrag(const int mx0, const int my0,
                   const int mx1, const int my1,
                   const Ngl::Viewport &vp)
    { _zoom(mx0, my0, mx1, my1, vp); }


    // Keyboard events.

    virtual void
    frame(/*const NtVec3f       &sceneMin,
          const NtVec3f       &sceneMax,*/
          const Ns3DBBox      &bbox,
          const Ngl::Viewport &vp)
    { _frame(/*sceneMin, sceneMax,*/ bbox, vp); }


    virtual void 
    updateModelview();

    virtual void 
    updateProjection(const Ngl::Viewport& vp);

    virtual void 
    updateParam(const QString &opName,
                const QString &parameterName,
                const QString &newValue,
                NtComponent    component);

    virtual float
    zoomFactor(const Nb::TimeBundle &tb) const
    { return getParam1f("Zoom Factor", tb); }

protected:

    //! Set the given parameter using a command. Also set parameter specific to
    //! the current view.
    virtual void
    setParam1f(const QString &paramName, const float value, const int prec = 6)
    {
        Ns3DCameraScope::setParam1f(
            paramName, value, prec);
        Ns3DCameraScope::setParam1f(
            viewParamName(paramName), value, prec);
    }

    //! Set the given vector parameter using a command. Also set parameter
    //! specific to the current view.
    virtual void
    setParam3f(const QString &paramName,
               const float    value0,
               const float    value1,
               const float    value2,
               const int      prec = 6)
    {
        Ns3DCameraScope::setParam3f(
            paramName, value0, value1, value2, prec);
        Ns3DCameraScope::setParam3f(
            viewParamName(paramName), value0, value1, value2, prec);
    }

    //! Return name of parameter specific to the view.
    QString
    viewParamName(const QString &paramName)
    { return (paramName + QString(" ") + _viewName); }

private:

    void 
    _zoom(const int mx0, const int my0,
          const int mx1, const int my1,
          const Ngl::Viewport &vp);

    void 
    _track(const int mx0, const int my0,
           const int mx1, const int my1,
           const Ngl::Viewport &vp);

    //void 
    //_dolly(float ndx, float ndy, const Ngl::Viewport &vp, const Ns3DBBox &bbox);

    void 
    _frame(/*const NtVec3f       &sceneMin,
           const NtVec3f       &sceneMax,*/
           const Ns3DBBox      &bbox,
           const Ngl::Viewport &vp);

private:    // Member variables.

    QString _viewName;
    int   _dragStartX;
    int   _dragStartY;
    float _dragStartZoom;

    Ngl::mat44f _modelview;   //!< Column-major 4x4 matrix.
    Ngl::mat44f _projection;  //!< Column-major 4x4 matrix.
};

// -----------------------------------------------------------------------------

// updateModelview
// ---------------
//! Make sure the modelview matrix can be plugged straight into OpenGL.

void
Ns3DOrthoCameraScope::updateModelview()
{
    const Nb::TimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
    _modelview = 
        em::make_cam_view(translate(cvftb), rotate(cvftb), scale(cvftb));
}


// updateProjection
// ----------------
//! Make sure the projection matrix can be plugged straight into OpenGL.

void
Ns3DOrthoCameraScope::updateProjection(const Ngl::Viewport &vp)
{
    const Nb::TimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
    const GLfloat aspect = static_cast<GLfloat>(vp.height())/vp.width();
    const GLfloat width = zoomFactor(cvftb);
    const GLfloat height = width*aspect;
    _projection = em::make_ortho_projection(-width, width,
                                            -height, height,
                                            nearClip(cvftb), farClip(cvftb));
}


// updateParam
// -----------
//! Make sure that view specific parameters get updated and that
//! view specific values are loaded when view changes.

void
Ns3DOrthoCameraScope::updateParam(const QString     &opName,
                                  const QString     &parameterName,
                                  const QString     &newValue,
                                  const NtComponent  component)
{
    if (fromNbStr(name()) != opName) {
        return;
    }

    if ("View Direction" == parameterName &&
        _viewName        != newValue) {

        _viewName = newValue;

        // Update GUI to reflect the current settings.

        const QString opName(fromNbStr(name()));

        Nb::String vpn = fromQStr(viewParamName("Translate"));
        NsCmdSetParam3::exec(
            NsCmdSetParam3::ArgsList() <<
                NsCmdSetParam3::Args(
                    opName,
                    "Translate",
                    fromNbStr(param3f(vpn)->expr(NI_X)),
                    fromNbStr(param3f(vpn)->expr(NI_Y)),
                    fromNbStr(param3f(vpn)->expr(NI_Z))));

        vpn = fromQStr(viewParamName("Rotate"));
        NsCmdSetParam3::exec(
            NsCmdSetParam3::ArgsList() <<
                NsCmdSetParam3::Args(
                    opName,
                    "Rotate",
                    fromNbStr(param3f(vpn)->expr(NI_X)),
                    fromNbStr(param3f(vpn)->expr(NI_Y)),
                    fromNbStr(param3f(vpn)->expr(NI_Z))));

        vpn = fromQStr(viewParamName("Scale"));
        NsCmdSetParam3::exec(
            NsCmdSetParam3::ArgsList() <<
                NsCmdSetParam3::Args(
                    opName,
                    "Scale",
                    fromNbStr(param3f(vpn)->expr(NI_X)),
                    fromNbStr(param3f(vpn)->expr(NI_Y)),
                    fromNbStr(param3f(vpn)->expr(NI_Z))));

        vpn = fromQStr(viewParamName("Orbit Radius"));
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    opName,
                    "Orbit Radius",
                    fromNbStr(param1f(vpn)->expr(0)),
                    0));

        vpn = fromQStr(viewParamName("Show Pivot"));
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    opName,
                    "Show Pivot",
                    fromNbStr(param1e(vpn)->expr(0)),
                    0));

        vpn = fromQStr(viewParamName("Pivot Size"));
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    opName,
                    "Pivot Size",
                    fromNbStr(param1f(vpn)->expr(0)),
                    0));

        vpn = fromQStr(viewParamName("Near Clip"));
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    opName,
                    "Near Clip",
                    fromNbStr(param1f(vpn)->expr(0)),
                    0));

        vpn = fromQStr(viewParamName("Far Clip"));
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    opName,
                    "Far Clip",
                    fromNbStr(param1f(vpn)->expr(0)),
                    0));

        vpn = fromQStr(viewParamName("Zoom Factor"));
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    opName,
                    "Zoom Factor",
                    fromNbStr(param1f(vpn)->expr(0)),
                    0));

        vpn = fromQStr(viewParamName("Mouse Sensitivity"));
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    opName,
                    "Mouse Sensitivity",
                    fromNbStr(param1f(vpn)->expr(0)),
                    0));
    }
    else if ("Translate" == parameterName) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    fromNbStr(name()),
                    viewParamName(parameterName),
                    newValue,
                    component));
    }
    else if ("Rotate" == parameterName) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    fromNbStr(name().str()),
                    viewParamName(parameterName),
                    newValue,
                    component));
    }
    else if ("Scale" == parameterName) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    fromNbStr(name()),
                    viewParamName(parameterName),
                    newValue,
                    component));
    }
    else if ("Orbit Radius" == parameterName) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    fromNbStr(name()),
                    viewParamName(parameterName),
                    newValue,
                    component));
    }
    else if ("Show Pivot" == parameterName) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    fromNbStr(name()),
                    viewParamName(parameterName),
                    newValue,
                    component));
    }
    else if ("Pivot Size" == parameterName) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    fromNbStr(name()),
                    viewParamName(parameterName),
                    newValue,
                    component));
    }
    else if ("Near Clip" == parameterName) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    fromNbStr(name()),
                    viewParamName(parameterName),
                    newValue,
                    component));
    }
    else if ("Far Clip" == parameterName) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    fromNbStr(name()),
                    viewParamName(parameterName),
                    newValue,
                    component));
    }
    else if ("Zoom Factor" == parameterName) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    fromNbStr(name()),
                    viewParamName(parameterName),
                    newValue,
                    component));
    }
    else if ("Mouse Sensitivity" == parameterName) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    fromNbStr(name()),
                    viewParamName(parameterName),
                    newValue,
                    component));
    }

    Ns3DCameraScope::updateParam(opName, parameterName, newValue, component);
}


// _zoom
// -----
//! TODO

void
Ns3DOrthoCameraScope::_zoom(const int mx0, const int my0,
                            const int mx1, const int my1,
                            const Ngl::Viewport &vp)
{
    const Nb::TimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
    const float ms = mouseSensitivity(cvftb);
    const float ndx = (ms*(_dragStartX - mx1))/vp.width();
    const float ndy = (ms*(_dragStartY - my1))/vp.height();
    const float zoom1 = 
        std::max<float>(0.1f, _dragStartZoom*(1 + (ndx - .5f*ndy)));
    setParam1f("Zoom Factor", zoom1);
    updateProjection(vp);
}

#if 0
// _dolly
// ------
//! Move camera in view direction.

void
Ns3DOrthoCameraScope::_dolly(const float          ndx,
                             const float          ndy,
                             const Ngl::Viewport &vp,
                             const Ns3DBBox      &bbox)
{
    const Nb::TimeBundle ctb(currentTime());
    const float mouseSpeed(mouseSensitivity(ctb)*20.f);

    const NtVec3f bdir(back());             // Back direction.
    const NtVec3f wsx0(worldPos());         // Original world space position.
    const float   orb0(orbitRadius(ctb));   // Original orbit radius.

    const NtVec3f piv(pivot(wsx0, bdir, orb0));

    // New orbit radius.

    const float orb1(
        std::max(2.f*nearClip(ctb), orb0 + (ndx - ndy)*mouseSpeed));

    // New world space position.

    const NtVec3f wsx1(piv + orb1*bdir);

    setParam3f("Translate", wsx1[0], wsx1[1], wsx1[2]);
    setParam1f("Orbit Radius", orb1);

    updateModelview();

}
#endif

// _track
// ------
//! Move camera in view plane.

void
Ns3DOrthoCameraScope::_track(const int mx0, const int my0,
                             const int mx1, const int my1,
                             const Ngl::Viewport &vp)
{
    const Nb::TimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
    const float ms = mouseSensitivity(cvftb);
    const float zoom = 2*zoomFactor(cvftb);
    const float aspect = static_cast<float>(vp.width())/vp.height();
    const float mdx = static_cast<float>(mx1 - mx0)/vp.width();
    const float mdy = static_cast<float>(my1 - my0)/vp.height();
    const float tdx = ms*zoom*aspect*mdx;
    const float tdy = ms*zoom*mdy;
    const NtVec3f t1 = getParam3f("Translate", cvftb) + tdx*right() + tdy*up();
    setParam3f("Translate", t1[0], t1[1], t1[2]);
    updateModelview();
}


// _frame
// ------
//! Adjust camera position, clip planes and zoom to encompass the bodies
//! in the graph. Framing does not affect rotation.

void
Ns3DOrthoCameraScope::_frame(const Ns3DBBox      &bbox,
                             const Ngl::Viewport &vp)
{
    NtVec3f mid(bbox.midX(), bbox.midY(), bbox.midZ());
    NtVec3f dims(
        std::max<float>(1.f, bbox.dimX()), 
        std::max<float>(1.f, bbox.dimY()), 
        std::max<float>(1.f, bbox.dimZ()));
    float orb1 = 1;
    float zoom1 = 1;
    if ("Front" == _viewName || 
        "Back"  == _viewName) {
        orb1 = 2*dims[2];
        zoom1 = 1.5f*em::mag(NtVec3f(.5f*dims[0], .5f*dims[1], 0));
    }
    else if ("Right" == _viewName ||
             "Left"  == _viewName) {
        orb1 = 2*dims[0];
        zoom1 = 1.5f*em::mag(NtVec3f(0, .5f*dims[1], .5f*dims[2]));
    }
    else if ("Up"    == _viewName ||
             "Down"  == _viewName) {
        orb1 = 2*dims[1];
        zoom1 = 1.5f*em::mag(NtVec3f(.5f*dims[0], 0, .5f*dims[2]));
    }
    const GLfloat aspect = static_cast<GLfloat>(vp.height())/vp.width();
    zoom1 *= aspect;
    const NtVec3f t1 = mid + orb1*back(); // New position.
    setParam3f("Translate", t1[0], t1[1], t1[2]);
    setParam1f("Orbit Radius", orb1);
    setParam1f("Zoom Factor", zoom1);
    updateModelview();
    updateProjection(vp);
}

#endif // NS3D_ORTHO_CAMERA_SCOPE_H
