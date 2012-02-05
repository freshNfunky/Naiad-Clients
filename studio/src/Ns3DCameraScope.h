// -----------------------------------------------------------------------------
//
// Ns3DCameraScope.h
//
// The interactive 3D Camera in the viewport.
//
// Copyright (c) 2011 Exotic Matter AB. All rights reserved.
//
// This file is part of Open Naiad Studio.
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

#ifndef NS3D_CAMERASCOPE_H
#define NS3D_CAMERASCOPE_H

#include <NgBodyOp.h>
#include <NgInput.h>

#include <NbTimeBundle.h>
#include <NbFilename.h>
#include <NbString.h>

#include <Ns3DBodyScope.h>
#include "NsStringUtils.h"
#include "NsCmdSetParam.h"
#include "NsCmdSetParam3.h"
#include "NsQuery.h"
#include "NsMessageWidget.h"

// Ngl
#include <NglState.h>
#include <NglUtils.h>
#include <NglViewport.h>

#include <em_mat44_algo.h>

#include <cmath>

//#include "NsSolveCache.h"
#include "Ns3DScopeUtils.h"
#include "NsMessageCallback.h"
//#include "NsOpItem.h"
//#include "NsGraph.h"

#include <QObject>
#include <QDebug>

class Ns3DBBox;

// -----------------------------------------------------------------------------

class Ns3DCameraScope : public Ns3DBodyScope
{
public:

    //! CTOR.
    Ns3DCameraScope() 
        : Ns3DBodyScope()
        , _texId(0)
        , _imagePlaneShader(0) // Null.
        , _imagePlaneFileName("")
    {

        std::stringstream ss;
        ss << qgetenv("NAIAD_PATH").constData() << "/clients/nstudio/shaders/";
        const std::string shaderPath(ss.str());

        _imagePlaneShader =
            new Ngl::ShaderProgram(NtString(shaderPath + "image-plane.vs"),
                                   NtString(shaderPath + "image-plane.fs"));
    }

    //! DTOR.
    virtual
    ~Ns3DCameraScope()
    {
        delete _imagePlaneShader;

        glDeleteTextures(1, &_texId);
    }

    // Default copy & assign.

    virtual NtString
    typeName() const = 0;
    
    virtual void
    setContext(const QGLContext* context)
    {}

    virtual void
    drawBodies(const Ns3DCameraScope* cam,
               const Ngl::Viewport&   vp,
               const int              frame)
    {}

    virtual bool
    draw(NsBodyObject*          nsBody,
         const Ns3DCameraScope* cam,
         const Ngl::Viewport&   vp)
    { return false; }

public:     // OpenGL related data.

    // Cameras must be able to provide an OpenGL compatible modelview matrix.

    virtual const GLfloat*
    modelview() const = 0;

    virtual const Ngl::mat44f&
    modelviewMat() const = 0;

    // Cameras must be able to provide an OpenGL compatible projection matrix.

    virtual const GLfloat*
    projection() const = 0;

    virtual const Ngl::mat44f&
    projectionMat() const = 0;


public:     // Event handling.

    //! Initialize dragging.
    virtual void
    initDrag(const int mx, const int my,
             const Ngl::Viewport &vp) = 0;

    //! Handle left mouse button + drag.
    virtual void
    mouseLeftDrag(const int mx0, const int my0,
                  const int mx1, const int my1,
                  const Ngl::Viewport &vp) = 0;

    //! Handle middle mouse button + drag.
    virtual void
    mouseMidDrag(const int mx0, const int my0,
                 const int mx1, const int my1,
                 const Ngl::Viewport &vp) = 0;

    //! Handle right mouse button + drag.
    virtual void
    mouseRightDrag(const int mx0, const int my0,
                   const int mx1, const int my1,
                   const Ngl::Viewport &vp) = 0;


public:     // Camera operations.

    virtual void
    frame(/*const NtVec3f       &sceneMin,
          const NtVec3f       &sceneMax,*/
          const Ns3DBBox      &bbox,
          const Ngl::Viewport &vp) = 0;


    //! Compute a new modelview matrix from parameters in GUI.
    virtual void
    updateModelview() = 0;

    //! Compute a new projection matrix from parameters in GUI.
    virtual void
    updateProjection(const Ngl::Viewport& vp) = 0;

    //! Do nothing by default.
    virtual void
    loadBody(const Ngl::Viewport& vp)
    {}

    virtual void
    updateParam(const QString &opName,
                const QString &parameterName,
                const QString &newValue,
                NtComponent    component)
    {
        if (fromNbStr(name()) != opName) {
            return;
        }

        if ("Image Plane" == parameterName) {

            int cvf;
            queryCurrentVisibleFrame(&cvf);

            const QString sequenceName(
                evalParam1s(queryParamLongName(opName, "Image Plane")));

            const int frameOffset(
                evalParam1i(queryParamLongName(opName, "Frame Offset")));

            const int framePadding(
                evalParam1i(queryParamLongName(opName, "Frame Padding")));

            const Nb::String fileName(
                Nb::sequenceToFilename(
                        fromQStr(queryProjectPath()),
                        fromQStr(sequenceName),
                        cvf + frameOffset,
                        -1,
                        framePadding));

            _setImagePlane(fromNbStr(fileName));

//            String
//            Nb::sequenceToFilename(const String projectPath,
//                               const String sequenceName,
//                               const int    frame,
//                               const int    timestep,
//                               const int    padding,
//                               const char   sep='.');

        }
    }

    virtual void
    currentVisibleFrameChanged(const int cvf)
    {
        const QString opName(fromNbStr(name()));

        const QString sequenceName(
            evalParam1s(queryParamLongName(opName, "Image Plane")));

        const int frameOffset(
            evalParam1i(queryParamLongName(opName, "Frame Offset")));

        const int framePadding(
            evalParam1i(queryParamLongName(opName, "Frame Padding")));

        const Nb::String fileName(
            Nb::sequenceToFilename(
                fromQStr(queryProjectPath()),
                fromQStr(sequenceName),
                cvf + frameOffset,
                -1,
                framePadding));

        _setImagePlane(fromNbStr(fileName));
    }

public:     // Camera information.

    //! Grab camera world position from modelview matrix.
    virtual NtVec3f
    worldPos() const
    {
        em::glmat44f invMv;
        em::invert(modelviewMat(), invMv);
        return NtVec3f(invMv[3][0], invMv[3][1], invMv[3][2]);
    }

    //! Camera local X+ axis. First row of modelview rotation. Points right
    //! with respect to view direction.
    virtual NtVec3f
    right() const
    {
        const float *mv(modelview());
        return NtVec3f(mv[0], mv[4], mv[8]);
    }

    //! Camera local Y+ axis. Second row of modelview rotation.
    virtual NtVec3f
    up() const
    {
        const float *mv(modelview());
        return NtVec3f(mv[1], mv[5], mv[9]);
    }

    //! Camera local Z+ axis. Note that this is not the view direction.
    //! In fact, the view direction is exactly opposite this vector (i.e.
    //! all components sign-flipped). Third row of modelview rotation.
    virtual NtVec3f
    back() const
    {
        const float *mv(modelview());
        return NtVec3f(mv[2], mv[6], mv[10]);
    }

    //! Camera local Z- axis. Corresponds to view direction.
    virtual NtVec3f
    view() const
    { return -back(); }

    //! Get pivot position for the current time.
    virtual NtVec3f
    pivot() const
    {
        const Nb::TimeBundle ctb(currentTime());
        return pivot(worldPos(), back(), orbitRadius(ctb));
    }

    //! Return viewport aspect ratio.
    static float
    aspectRatio(const Ngl::Viewport &vp)
    { return static_cast<float>((vp.width() - vp.x()))/(vp.height() - vp.y()); }

public: // Camera parameters.

    // Modelview parameters.

    //! All cameras have a world space position.
    virtual NtVec3f
    translate(const Nb::TimeBundle &tb) const
    { return getParam3f("Translate", tb); }

    //! All cameras have a world space rotation.
    virtual NtVec3f
    rotate(const Nb::TimeBundle &tb) const
    { return getParam3f("Rotate", tb); }

    //! All cameras have a scale vector.
    virtual NtVec3f
    scale(const Nb::TimeBundle &tb) const
    { return getParam3f("Scale", tb); }

    //! All cameras have an orbit radius.
    virtual float
    orbitRadius(const Nb::TimeBundle &tb) const
    { return getParam1f("Orbit Radius", tb); }

    //! Whether to show the pivot in 3D View.
    virtual bool
    showPivot(const Nb::TimeBundle &tb) const
    { return ("On" == param1e("Show Pivot")->eval(tb)); }

    //! Radius of rendered pivot sphere.
    virtual float
    pivotSize(const Nb::TimeBundle &tb) const
    { return getParam1f("Pivot Size", tb); }

    // Projection parameters.

    //! All cameras have a near clip plane.
    virtual float
    nearClip(const Nb::TimeBundle &tb) const
    { return getParam1f("Near Clip", tb); }

    //! All cameras have a far clip plane.
    virtual float
    farClip(const Nb::TimeBundle &tb) const
    { return getParam1f("Far Clip", tb); }

    //! All cameras have a mouse sensitivity.
    virtual float
    mouseSensitivity(const Nb::TimeBundle &tb) const
    { return getParam1f("Mouse Sensitivity", tb); }


    bool
    hasImagePlane() const
    { return 0 != _texId && !_imagePlaneFileName.isEmpty(); }

    void
    imagePlaneDimensions(GLint &w, GLint &h)
    {
        glActiveTexture(GL_TEXTURE0); // Texture unit 0.
        glBindTexture(GL_TEXTURE_2D, _texId);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void
    drawImagePlane(const em::glmat44f      &modelview,
                   const em::glmat44f      &projection,
                   const Ngl::VertexBuffer &positionVbo,
                   const Ngl::VertexBuffer &texVbo) const
    {
        glActiveTexture(GL_TEXTURE0); // Texture unit 0.
        glBindTexture(GL_TEXTURE_2D, _texId);

        _imagePlaneShader->use();
        _imagePlaneShader->storeUniform4m("modelview",  modelview[0]);
        _imagePlaneShader->storeUniform4m("projection", projection[0]);
        _imagePlaneShader->storeUniform1i("image", 0); // Texture unit 0.
        _imagePlaneShader->uploadUniforms(Nb::ZeroTimeBundle);

        Ngl::VertexAttrib::connect(
            _imagePlaneShader->attribMap().find("position")->second,
            positionVbo);
        Ngl::VertexAttrib::connect(
            _imagePlaneShader->attribMap().find("tex")->second,
            texVbo);

        glDrawArrays(GL_QUADS, 0, 4);

        Ngl::VertexAttrib::disconnect(
            _imagePlaneShader->attribMap().find("position")->second);
        Ngl::VertexAttrib::disconnect(
            _imagePlaneShader->attribMap().find("tex")->second);

        _imagePlaneShader->unuse();
        glBindTexture(GL_TEXTURE_2D, 0);
    }

public:

    //! Theta is angle in XZ plane. Phi is azimuth.
    static NtVec3f
    sphericalPosition(const float    phiDeg,
                      const float    thetaDeg,
                      const float    radius,
                      const NtVec3f &ctr)
    {
        static const float deg2rad(static_cast<float>(M_PI)/180.f);

        const float phiRad(deg2rad*(90.f - phiDeg));
        const float thetaRad(deg2rad*thetaDeg);

        return NtVec3f(ctr[0] + radius*sinf(thetaRad)*sinf(phiRad),
                       ctr[1] + radius*cosf(phiRad),
                       ctr[2] + radius*cosf(thetaRad)*sinf(phiRad));
    }

    //! Return spherical angles in degrees.
    static NtVec3f
    sphericalAngles(const NtVec3f &wsx,
                    const NtVec3f &ctr)
    {
        static const float rad2deg(180.f/static_cast<float>(M_PI));

        const NtVec3f dx(wsx - ctr);

        return NtVec3f(90.f - rad2deg*acosf(dx[1]/em::mag(dx)), // Phi
                       rad2deg*atan2f(dx[0], dx[2]),            // Theta
                       0.f);
    }

protected:      // Utilities for interacting with GUI parameters.

    //! TODO: Set NEL context!?
    virtual float
    getParam1f(const QString &paramName, const Nb::TimeBundle &tb) const
    { 
        return evalParam1f(
            queryParamLongName(fromNbStr(name()), paramName), tb);
        //return param1f(fromQStr(paramName))->eval(tb); 
    }

    //! TODO: Set NEL context!?
    virtual NtVec3f
    getParam3f(const QString &paramName, const Nb::TimeBundle &tb) const
    {
        const QString paramLongName = 
            queryParamLongName(fromNbStr(name()), paramName);
        return NtVec3f(
            evalParam3f(paramLongName, 0, tb),
            evalParam3f(paramLongName, 1, tb),
            evalParam3f(paramLongName, 2, tb));
        //const Nb::Value3f *prm(param3f(fromQStr(paramName)));
        //return NtVec3f(prm->eval(tb, 0), prm->eval(tb, 1), prm->eval(tb, 2));
    }

    //! Set the given parameter using a command.
    virtual void
    setParam1f(const QString &paramName, const float value, const int prec = 6)
    {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    fromNbStr(name()),
                    paramName,
                    QString::number(value, 'g', prec),
                    0));
    }

    //! Set the given vector parameter using a command.
    virtual void
    setParam3f(const QString &paramName,
               const float    value0,
               const float    value1,
               const float    value2,
               const int      prec = 6)
    {
        NsCmdSetParam3::exec(
            NsCmdSetParam3::ArgsList() <<
                NsCmdSetParam3::Args(
                    fromNbStr(name().str()),
                    paramName,
                    QString::number(value0, 'g', prec),
                    QString::number(value1, 'g', prec),
                    QString::number(value2, 'g', prec)));
    }

    //! Return pivot.
    static NtVec3f
    pivot(const NtVec3f &wsx, const NtVec3f &back, const GLfloat radius)
    { return (wsx - radius*back); }

protected:

    static bool
    trackDistance(const Ngl::vec3f    &ppOri,  
                  const Ngl::vec3f    &ppNml,  
                  const em::glmat44f  &mv,
                  const em::glmat44f  &proj,
                  const Ngl::Viewport &vp,
                  const int mx0, const int my0,   
                  const int mx1, const int my1,
                  const GLfloat n, const GLfloat f,
                  float &td0, float &td1)
    {
        Ngl::vec3f rayOri0;
        Ngl::vec3f rayDir0;
        if (!Ngl::mouseRay(vp, mv, proj, mx0, my0, n, f, &rayOri0, &rayDir0)) {
            //std::cerr << "RAY0 FAIL!\n";
            return false;
        }
        Ngl::vec3f rayOri1;
        Ngl::vec3f rayDir1;
        if (!Ngl::mouseRay(vp, mv, proj, mx1, my1, n, f, &rayOri1, &rayDir1)) {
            //std::cerr << "RAY1 FAIL!\n";
            return false;
        }
        Ngl::vec3f rpi0;
        if (!Ngl::rayPlaneIntersect(rayOri0, rayDir0, ppNml, ppOri, &rpi0)) {
            //std::cerr << "ISECT0 FAIL!\n";
            return false;
        }
        Ngl::vec3f rpi1;
        if (!Ngl::rayPlaneIntersect(rayOri1, rayDir1, ppNml, ppOri, &rpi1)) {
            //std::cerr << "ISECT1 FAIL!\n";
            return false;
        }

        td0 = rpi1[0] - rpi0[0];
        td1 = rpi1[1] - rpi0[1];
        return true;
    }

    static float
    worldBoundsScreenArea(const Ngl::vec3f    &wmin, 
                          const Ngl::vec3f    &wmax,
                          const em::glmat44f  &mv,
                          const em::glmat44f  &proj,
                          const Ngl::Viewport &vp)
    {
        const Ngl::vec3f wsx[8] = {
            Ngl::vec3f(wmin[0], wmin[1], wmin[2]),
            Ngl::vec3f(wmax[0], wmin[1], wmin[2]),
            Ngl::vec3f(wmax[0], wmin[1], wmax[2]),
            Ngl::vec3f(wmin[0], wmin[1], wmax[2]),
            Ngl::vec3f(wmin[0], wmax[1], wmin[2]),
            Ngl::vec3f(wmax[0], wmax[1], wmin[2]),
            Ngl::vec3f(wmax[0], wmax[1], wmax[2]),
            Ngl::vec3f(wmin[0], wmax[1], wmax[2])
        };
        Ngl::vec3f smin(
            (std::numeric_limits<float>::max)(),
            (std::numeric_limits<float>::max)(),
            (std::numeric_limits<float>::max)());
        Ngl::vec3f smax(
            -(std::numeric_limits<float>::max)(),
            -(std::numeric_limits<float>::max)(),
            -(std::numeric_limits<float>::max)());
        Ngl::vec3f esx;
        Ngl::vec4f csx;
        Ngl::vec3f ndc;
        Ngl::vec3f win;
        for (int i = 0; i < 8; ++i) {
            // Apply transformations.
        
            esx = mv*wsx[i]; // Assumes wsx[3] = 1. 
            csx = proj*Ngl::vec4f(esx[0], esx[1], esx[2], 1);
            const float inv_csx_w = 1/csx[3];
            ndc[0] = csx[0]*inv_csx_w;
            ndc[1] = csx[1]*inv_csx_w;
            ndc[2] = csx[2]*inv_csx_w;
            win[0] = vp.x() + 0.5*vp.width()*(ndc[0] + 1);
            win[1] = vp.y() + 0.5*vp.height()*(ndc[1] + 1);
            win[2] = 0.5*(ndc[2] + 1);

            smin[0] = std::min<float>(smin[0], win[0]);
            smin[1] = std::min<float>(smin[1], win[1]);
            smin[2] = std::min<float>(smin[2], win[2]);
            smax[0] = std::max<float>(smax[0], win[0]);
            smax[1] = std::max<float>(smax[1], win[1]);
            smax[2] = std::max<float>(smax[2], win[2]);
        }
        return (smax[0] - smin[0])*(smax[1] - smin[1]);
    }

    static float
    viewportArea(const Ngl::Viewport &vp)
    { return vp.width()*vp.height(); }

private:

    bool
    _setImagePlane(const QString &fileName)
    {
        bool success(true);

        if (fileName != _imagePlaneFileName) {
            success = false;

            if (0 != _texId) {
                glDeleteTextures(1, &_texId);   // Free existing texture!
            }

            QImage buf;

            if (buf.load(fileName)) {
                // Successfully read image from disk. Now convert image
                // to suitable texture format.

                glGenTextures(1, &_texId);

                if (0 != _texId) {
                    // Generated a valid texture Id.

                    const QImage tex = QGLWidget::convertToGLFormat(buf);

                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    glPixelStorei(GL_PACK_ALIGNMENT, 1);

                    glBindTexture(GL_TEXTURE_2D, _texId);

                    glTexImage2D(GL_TEXTURE_2D,
                                 0,
                                 GL_RGBA,//GL_RGBA,
                                 tex.width(),
                                 tex.height(),
                                 0,
                                 GL_RGBA,
                                 GL_UNSIGNED_BYTE,
                                 tex.bits());
                    glTexParameteri(GL_TEXTURE_2D,
                                    GL_TEXTURE_MAG_FILTER,
                                    GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D,
                                    GL_TEXTURE_MIN_FILTER,
                                    GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D,
                                    GL_TEXTURE_WRAP_S,
                                    GL_CLAMP);
                    glTexParameteri(GL_TEXTURE_2D,
                                    GL_TEXTURE_WRAP_T,
                                    GL_CLAMP);

                    glBindTexture(GL_TEXTURE_2D, 0);

                    _imagePlaneFileName = fileName;
                    success = true;
                }
                // NB: _texId is already zero.
            }
            else {
                _imagePlaneFileName = "";
                _texId = 0;

                NsMessageWidget::instance()->clientWarning(
                    QObject::tr("Cannot load image ") + fileName,
                    fromNbStr(name()));
            }
        }

        return success;
    }

private:    // Member variables.

    unsigned int _texId;
    Ngl::ShaderProgram *_imagePlaneShader;
    QString _imagePlaneFileName;
};

// -----------------------------------------------------------------------------

#endif // NS3D_CAMERASCOPE_H
