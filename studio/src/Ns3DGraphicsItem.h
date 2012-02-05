// -----------------------------------------------------------------------------
//
// Ns3DGraphicsItem.h
//
// Interface for renderable objects.
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

#ifndef NS3D_GRAPHICSITEM_H
#define NS3D_GRAPHICSITEM_H

#include "Ns3DBBox.h"
#include <NglTypes.h>
#include <em_glmat44_algo.h>
//#include <em_vec.h>

// -----------------------------------------------------------------------------

class Ns3DGraphicsItem
{
public:

    //! TODO: Should be in Ns3DOpItem
    enum SectionFlags {
        TransformFlag       = 0x01,
        ClipBoxFlag         = 0x02,
        PlaneFlag           = 0x04,
        FrustumFlag         = 0x08,
        SphereFlag          = 0x10,
        AxisFlag            = 0x20,
        VectorFlag          = 0x40,
        ImportTransformFlag = 0x80
    };

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
        setColor(const QColor & color)
        { _color = color; }

    private:    // Member variables

        QPointF _pos;
        QString _text;
        QColor  _color;
    };

public:

    explicit
    Ns3DGraphicsItem(const int32_t id,
                     const bool selectable/*,
                     const Ns3DBBox &bbox = Ns3DBBox()*/)
        : _id(id)
        , _selected(false)
        , _selectable(selectable)
    {
        // TODO: Check that id is not a manipulator id:
        //           assert(!Ns3Dmanipulator::isManipId(id));
    }

    //! DTOR.
    virtual
    ~Ns3DGraphicsItem()
    {}

    virtual void
    draw(LabelInfo *label = 0) const = 0;

    virtual void
    selectionDraw() const
    {}

    //virtual void
    //setBBox(const Ns3DBBox &bbox)
    //{ _bbox = bbox; }

    virtual void
    setSelected(const bool sel)
    { _selected = sel; }

    virtual void
    setSelectable(const bool sel)
    { _selectable = sel; }

public:

    virtual QString
    translateParamName() const
    { return QString("Translate"); }

    virtual QString
    rotateParamName() const
    { return QString("Rotate"); }

    virtual QString
    scaleParamName() const
    { return QString("Scale"); }

    virtual Ngl::vec3f
    translate() const = 0;

    virtual Ngl::vec3f
    rotate() const = 0;

    virtual Ngl::vec3f
    scale() const = 0;

    virtual bool 
    isVisible() const = 0;

    virtual Ns3DBBox
    worldBoundingBox() const = 0;

public:

    //const Ns3DBBox &bbox() const    { return _bbox; }
    int32_t 
    id() const              
    { return _id; }

    bool 
    selected() const           
    { return _selected; }

    bool 
    selectable() const         
    { return _selectable; }

protected:

    virtual bool
    labelPos(const Ngl::vec3f *vtx, const int n, QPointF &pos) const
    {
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

        int v(0);
        while (v < n) {
            em::project(vtx[v],
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
                pos = QPointF(win[0], win[1]);
                return true;
            }
            ++v;
        }

        return false;
    }

private:    // Member variables.

    //Ns3DBBox _bbox;
    int32_t _id;
    bool _selected;
    bool _selectable;

private:

    // Disabled.
    Ns3DGraphicsItem(const Ns3DGraphicsItem&);
    Ns3DGraphicsItem& operator=(const Ns3DGraphicsItem&);
};

#endif // NS3D_GRAPHICSITEM_H
