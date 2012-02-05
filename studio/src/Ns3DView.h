// -----------------------------------------------------------------------------
//
// Ns3DView.h
//
// Naiad Studio 3D view, header file.
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

#ifndef NS_3DVIEW_H
#define NS_3DVIEW_H

#include <QGLWidget>

#include <Ni.h>

//#include "Ns3DTumblerUser.h"
#include "Ns3DCameraScope.h"
#include "Ns3DSelectionManager.h"
#include "Ns3DScene.h"
#include "Ns3DGraphicsItem.h"

#include <QtGui>    // TODO: TMP!!

// -----------------------------------------------------------------------------

#if 0
void* select_3_2_mac_visual(GDHandle handle);

struct Core3_2_context : public QGLContext
{
    Core3_2_context(const QGLFormat& format, QPaintDevice* device) 
    : QGLContext(format,device) 
    {}

    Core3_2_context(const QGLFormat& format) 
        : QGLContext(format) 
    {}
 
    virtual void* 
    chooseMacVisual(GDHandle handle)
    {
        return select_3_2_mac_visual(handle);
    }
};
#endif

// -----------------------------------------------------------------------------

// Ns3DView
// --------
//! The Ns3DView class, representing the 3D view.

class Ns3DView : public QGLWidget
{
    Q_OBJECT

public:

    explicit
    Ns3DView(Ns3DScene *scene, QWidget *parent = 0);

    virtual
    ~Ns3DView();

public:

    void
    setScene(Ns3DScene *scene);

public:

    QAction*
    drawOriginGridAction() const
    { return _drawOriginGridAction; }

    QAction*
    drawHudAction() const
    { return _drawHudAction; }

    QAction*
    drawAxesAction() const
    { return _drawAxesAction; }

    QAction*
    drawItemLabelsAction() const
    { return _drawItemLabelsAction; }

    QAction*
    drawBodyLabelsAction() const
    { return _drawBodyLabelsAction; }

    QAction*
    frameSceneAction() const
    { return _frameSceneAction; }

    QAction*
    playblastAction() const
    { return _playblastAction; }

    QAction*
    stepPlayblastAction() const
    { return _stepPlayblastAction; }

    QAction*
    translateManipAction() const
    { return _translateManipAction; }

    QAction*
    rotateManipAction() const
    { return _rotateManipAction; }

    QAction*
    scaleManipAction() const
    { return _scaleManipAction; }

    QAction*
    nullManipAction() const
    { return _nullManipAction; }

public slots:

    bool
    playblast(const QString &path,
              const QString &fileNameBase,
              int            first,
              int            last,
              int            width,
              int            height,
              const QString &format  = "JPEG",
              int            quality = -1);

public slots:

    virtual void
    updateGL();

protected slots:

    void
    onFrameScene();

    void
    onEndTimeStep(const NtTimeBundle &tb);

    void
    onEndFrame(const NtTimeBundle &tb);

    void
    onCurrentVisibleFrameChanged(int cvf, bool update3DView, bool success);

protected:  // GL

    virtual void
    initializeGL();

    virtual void
    resizeGL(int w, int h);

    virtual void
    paintGL();

    virtual void
    glInit();

    virtual void
    glDraw();

protected:  // User interaction.

    virtual void
    mousePressEvent(QMouseEvent *event);

    virtual void
    mouseMoveEvent(QMouseEvent *event);

    virtual void
    mouseReleaseEvent(QMouseEvent *event);

    virtual void
    keyPressEvent(QKeyEvent *event);

private slots:

    void
    _onReadSettings();

    void
    _onWriteSettings();

    void
    _onValueChanged(const QString &valueLongName,
                    const QString &expr,
                    int            comp,
                    bool           success);

    void
    _onTranslateManip();

    void
    _onRotateManip();

    void
    _onScaleManip();

    void
    _onNullManip();


//    void connectionUpdate(const QString &terminal, bool scope);

//    void
//    _onFeedChanged();

//    void
//    _onOpStateChanged();

//    void
//    _onValueChanged(const QString &paramLongName,
//                    const QString &expr,
//                    int            comp,
//                    bool           success)
//    { update(); }

//    void
//    _onMetaValueChanged(const QString &longName,
//                        const QString &valueType,
//                        const QString &value,
//                        bool           success)
//    { update(); }


private:    // Draw.

    void _drawScene(Ns3DCameraScope &cam, bool drawSelections);
    void _drawHud(const QString &str,
                  const QList<Ns3DGraphicsItem::LabelInfo> &itemLabels,
                  const QList<Ns3DBodyScope::LabelInfo> &bodyLabels);
    void _drawActiveScopes(const Ns3DCameraScope &cam,
                           std::stringstream *ss,
                           QList<Ns3DBodyScope::LabelInfo> &labels);
    //void _drawOriginGrid();
    void _drawAxes(const Ngl::mat44f &mv);
    void _drawCameraPivot(const Ns3DCameraScope &cam);
    void _drawOpItems(const Ns3DCameraScope              &cam,
                      const QList<Ns3DGraphicsItem*>     &items,
                      QList<Ns3DGraphicsItem::LabelInfo> &labels);
    void _drawBBox(const Ns3DBBox &bbox);
    //void _drawLiveSolveInfo(QPainter* painter);
    //void paint3D(Ns3DCameraScope *cam);

    void
    _clear(Ns3DCameraScope *cam);

private:

    void
    _createActions();

    Ns3DCameraScope *
    _activeCameraScope() const;


private:

    static QString
    _playblastFileName(const QString &path,
                       const QString &fileNameBase,
                       const QString &format,
                       int            frame);

private:    // Member variables.

    Ns3DScene            *_scene;

    //Ns3DTumblerUser      user;
    Ns3DSelectionManager _selMgr;

    Ngl::Viewport        _viewport;

    NtVec3f              _sceneMin;
    NtVec3f              _sceneMax;

    QAction             *_drawOriginGridAction;
    QAction             *_drawHudAction;
    QAction             *_drawAxesAction;
    QAction             *_drawItemLabelsAction;
    QAction             *_drawBodyLabelsAction;
    QAction             *_frameSceneAction;
    QAction             *_playblastAction;
    QAction             *_stepPlayblastAction;

    QActionGroup        *_manipActionGroup;
    QAction             *_translateManipAction;
    QAction             *_rotateManipAction;
    QAction             *_scaleManipAction;
    QAction             *_nullManipAction;

    bool                 _drawSelection;

    QPoint               dragStart;
    QPoint               lastPos;
    bool                 dragging;
    bool                 glInitd;
};

// -----------------------------------------------------------------------------

//void GLWidget::saveGLState()
//{
//    glPushAttrib(GL_ALL_ATTRIB_BITS);
//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//    glMatrixMode(GL_MODELVIEW);
//    glPushMatrix();
//}


//void GLWidget::restoreGLState()
//{
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//    glMatrixMode(GL_MODELVIEW);
//    glPopMatrix();
//    glPopAttrib();
//}


#endif // NS_3DVIEW_H
