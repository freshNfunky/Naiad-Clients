// -----------------------------------------------------------------------------
//
// Ns3DBodyScope.h
//
// Naiad Studio base class for scopes which draw bodies in the 3D View.
//
// Scopes are implemented as follows:
//
// * Every scope is a BodyOp since it operates on bodies (it draws them)
//
// * The scopes are implemented here in Naiad Studio using Naiad's plug-in
//   architecture: each scope is derived from the dummy-BodyOp class, so that
//   in Naiad they do nothing.  The derived class declared here in Naiad Studio
//   takes care of drawing the bodies, etc.
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
//
// -----------------------------------------------------------------------------

#ifndef NS3D_BODY_SCOPE_H
#define NS3D_BODY_SCOPE_H

#undef min
#undef max

#include "NsPlatform.h"
#include "NsStringUtils.h"
#include "Ns3DScopeUtils.h"
#include "NsMessageCallback.h"
#include "NsOpStore.h"
#include "NsOpObject.h"
#include "NsBodyObject.h"
#include "NsBodyInputPlugObject.h"
#include "NsBodyOutputPlugObject.h"
#include "NsQuery.h"
#include "NsPreferences.h"

#include <NglViewport.h>
#include <NglShaderProgram.h>

#include <NgDummyBodyOp.h>
#include <NgBodyInput.h>
#include <NgNelContext.h>

#include <climits>

class Ns3DCameraScope;

// -----------------------------------------------------------------------------

class Ns3DBodyScope : public Ng::DummyBodyOp
{
public:

    class LabelInfo
    {
    public:

        LabelInfo()
            : _pos(0.f, 0.f)
            , _color(0, 0, 0)
        {}

        explicit
        LabelInfo(const QPointF &pos, const QString &text, const QColor &color)
            : _pos(pos)
            , _text(text)
            , _color(color)
        {}

        const QPointF&
        pos() const
        { return _pos; }

        const QString&
        text() const
        { return _text; }

        const QColor&
        color() const 
        { return _color; }

        void
        setPos(const QPointF &pos)
        { _pos = pos; }

        void
        setText(const QString &text)
        { _text = text; }

        void
        setColor(const QColor &color)
        { _color = color; }

    private:    // Member variables

        QPointF _pos;
        QString _text;
        QColor  _color;
    };

public:

    Ns3DBodyScope()
        : Ng::DummyBodyOp("You Have Not overridden typeName()")
        , _scopeMin(std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max())
        , _scopeMax(-std::numeric_limits<float>::max(),
                    -std::numeric_limits<float>::max(),
                    -std::numeric_limits<float>::max())
    {}

    virtual void
    drawBodies(const Ns3DCameraScope* cam,
               const Ngl::Viewport&   vp,
               const int              frame)
    {   
        ssHud.str(""); // Clear HUD info.

        resetBounds(_scopeMin, _scopeMax);

        const NsOpObject *op(
            NsOpStore::instance()->queryConstOp(fromNbStr(name())));

        if (0 != op && "ACTIVE" == op->state()) {
            Nb::TimeBundle tb(Nb::ZeroTimeBundle);  // TODO: current time?

            const QString showBodies(
                evalParam1s(    // TODO: current time?
                    queryParamLongName(op->longName(), "Show Bodies")));

            const bool displayCellShadow(
                hasParam("Display Cell Shadow") &&
                "On" == param1e("Display Cell Shadow")->eval(tb));

            int bodyCount = 0;
            int bodyDrawCount = 0;

            NtVec3f bodyMin(std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::max());
            NtVec3f bodyMax(-std::numeric_limits<float>::max(),
                            -std::numeric_limits<float>::max(),
                            -std::numeric_limits<float>::max());

            foreach (const NsBodyInputPlugObject *bipo, op->constBodyInputs()) {
                if (0 != bipo->source()) {
                    NsPlugObject *po(
                        NsOpStore::instance()->queryMutablePlug(
                            fromNbStr((bipo->source()->longname()))));

                    if (0 != po) {
                        //const QList<const NsBodyObject*> bodies(
                        //    po->constBodies());

                        //qDebug() << bodies;


//                        QList<const NsBodyObject*> bodies;
//                        int cvf(1);
//                        queryCurrentVisibleFrame(&cvf); // Ignore success.
//                        if (queryIsFrameLive(cvf)) {
//                            // Current visible frame is the live frame.

//                            bodies = po->constLiveBodies();
//                        }
//                        else {
//                            // Current visible frame is not the live frame.
                            
//                            //po->mutableOp()->updateEmpBodyCache();
//                            bodies = po->constOp()->constCachedBodies();
//                        }

                        QList<const NsBodyObject*> bodies;
                        const bool activeOp(!("INACTIVE" == po->constOp()->state()));
                        int cvf(1);
                        queryCurrentVisibleFrame(&cvf); // Ignore success.
                        const bool liveFrame(queryIsFrameLive(cvf));

                        if (liveFrame && activeOp) {
                            qDebug() << "Scope Live bodies";
                            bodies = po->constLiveBodies();
                        }
                        else {
                            qDebug() << "Scope cached bodies";
                            po->mutableOp()->updateEmpBodyCache();
                            bodies = po->constOp()->constCachedBodies();
                        }

                        _labels.clear();

                        for (int i(0); i < bodies.size(); ++i) {
                            NsBodyObject *bo(const_cast<NsBodyObject *>(bodies[i]));
                            
                            bodyCount++;

                            if (admitBody(*bo, *bipo, showBodies)) {
                                // Body is admitted for drawing.

                                //qDebug() << "Admitted:" << bo;

                                // Important to bind a NEL context before
                                // drawing bodies, since the scope and 
                                // the body properties may contain NEL 
                                // expressions!
                                const Nb::TimeBundle tb = 
                                    queryCurrentVisibleFrameTimeBundle();
                                Ng::NelContext nelContext(
                                    this,&bo->nbBody(),"",tb
                                    );
                                
                                try {
                                    
                                    if(bo->nbBody().empty()) {
                                        ssHud << "Empty Body: '" 
                                              << fromQStr(bo->name()) 
                                              << "'";
                                        bodyDrawCount++;
                                        continue;
                                    }

                                    if (draw(bo, cam, vp)) {
                                        // Query drawn body's bounds and 
                                        // update scope bounds.
                                        
                                        resetBounds(bodyMin, bodyMax);
                                        bo->nbBody().bounds(bodyMin, bodyMax);
                                        updateScopeBounds(bodyMin, bodyMax);
                                        

                                        GLfloat mv[16];
                                        GLfloat p[16];
                                        GLint glvp[4];
                                        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
                                        glGetFloatv(GL_PROJECTION_MATRIX, p);
                                        glGetIntegerv(GL_VIEWPORT, glvp);
                                        em::vec<4,int> vp(glvp[0], glvp[1], glvp[2], glvp[3]);

                                        em::vec<4,GLfloat> eye;
                                        em::vec<4,GLfloat> clip;
                                        em::vec<3,GLfloat> ndc;
                                        em::vec<3,GLfloat> win;

                                        const Ngl::vec3f vtx(
                                            0.5*(bodyMin[0] + bodyMax[0]),
                                            0.5*(bodyMin[1] + bodyMax[1]),
                                            0.5*(bodyMin[2] + bodyMax[2]));

                                        em::project(vtx,
                                                    vp,
                                                    em::glmat44<GLfloat>(mv),
                                                    em::glmat44<GLfloat>(p),
                                                    eye,
                                                    clip,
                                                    ndc,
                                                    win);
                                        if ((vp[0] < win[0] && win[0] < vp[2]) &&
                                            (vp[1] < win[1] && win[1] < vp[3]) &&
                                            0.f < win[2]) {
                                            _labels.append(
                                                LabelInfo(
                                                    QPointF(win[0], win[1]),
                                                    bo->name() + QString(" [Body]"),
                                                    NsPreferences::instance()->scopeViewBodyLabelTextColor()));
                                        }

                                        if (displayCellShadow) {
                                            drawCellShadow(
                                                bo->ns3DBody(),
                                                param1e("Shadow Cell Size")
                                                );
                                        }
                                        
                                        //qDebug() << "Drawn:" << bo;
                                        bodyDrawCount++;
                                    }
                                }
                                catch(std::exception& e) {
                                    NB_ERROR(name() << ": " << e.what());
                                }
                                catch(...) {
                                    NB_ERROR("Error drawing scope: " << name());
                                }
                            }
                        }
                    }
                }
            }

            if (bodyDrawCount==0 && bodyCount>0) {
                NB_WARNING("No bodies admitted for drawing in scope '" <<
                           name() << "': check scope parameters or body feed");
            }
        }

    }
    
    virtual void
    setContext(const QGLContext* context)
    {}

    virtual bool
    draw(NsBodyObject*           body,
         const Ns3DCameraScope*  cam,
         const Ngl::Viewport&    vp) = 0;

    virtual void
    hudInfo(std::stringstream *ss)
    {
        *ss << typeName() << ": '" << name() << "'\n";

        if (!ssHud.str().empty()) {
            *ss << ssHud.str() << "\n";
        }
        else {
            *ss << "No bodies\n\n";
        }
    }

    virtual const QList<LabelInfo>&
    labels() const
    { return _labels; }

    virtual void
    bounds(NtVec3f &bmin, NtVec3f &bmax)
    {
        bmin = _scopeMin;
        bmax = _scopeMax;
    }

    //! Convenience.
    static Nb::TimeBundle
    currentTime()
    {
        return queryCurrentVisibleFrameTimeBundle();
        //int cvf;
        //queryCurrentVisibleFrame(&cvf);
        //const double dt = 1./evalParam1i("Global.Fps", 0);
        //return Nb::TimeBundle(cvf, 0, cvf*dt, 0, dt, dt, true);
    }

    //GLuint envMapHandle;

protected:

    void
    updateScopeBounds(const NtVec3f &bodyMin, const NtVec3f &bodyMax)
    {
        if (bodyMin[0] < bodyMax[0] &&
            bodyMin[1] < bodyMax[1] &&
            bodyMin[2] < bodyMax[2] ) {
            _scopeMin[0] = std::min<float>(_scopeMin[0], bodyMin[0]);
            _scopeMin[1] = std::min<float>(_scopeMin[1], bodyMin[1]);
            _scopeMin[2] = std::min<float>(_scopeMin[2], bodyMin[2]);
            _scopeMax[0] = std::max<float>(_scopeMax[0], bodyMax[0]);
            _scopeMax[1] = std::max<float>(_scopeMax[1], bodyMax[1]);
            _scopeMax[2] = std::max<float>(_scopeMax[2], bodyMax[2]);
        }
    }

    void
    resetBounds(NtVec3f &bMin, NtVec3f &bMax)
    {
        bMin[0] =  std::numeric_limits<float>::max();
        bMin[1] =  std::numeric_limits<float>::max();
        bMin[2] =  std::numeric_limits<float>::max();
        bMax[0] = -std::numeric_limits<float>::max();
        bMax[1] = -std::numeric_limits<float>::max();
        bMax[2] = -std::numeric_limits<float>::max();
    }

    static bool
    admitBody(const NsBodyObject          &body,
              const NsBodyInputPlugObject &bipo,
              const QString               &showBodies)
    {
        return (body.nbBody().matches(fromQStr(bipo.sigName())) &&
                body.nbBody().name().listed_in(fromQStr(showBodies)));
    }

protected:

    std::stringstream ssHud;
    QList<LabelInfo> _labels;

    NtVec3f _scopeMin;  //! Bounds of the bodies most recently drawn.
    NtVec3f _scopeMax;  //! Bounds of the bodies most recently drawn.
};

// -----------------------------------------------------------------------------

#endif // NS3D_BODY_SCOPE_H
