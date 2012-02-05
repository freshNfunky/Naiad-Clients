// -----------------------------------------------------------------------------
//
// Ns3DView.cc
//
// Naiad Studio 3D view source file.
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

#include <QtGui>
#include <QtOpenGL>

#include <Ni.h>
#include <NiNg.h>

#include <NbBody.h>
#include <NbParticleShape.h>

#include "Ns3DView.h"
#include "NsStringUtils.h"
//#include "Ns3DTumblerMode.h"
#include "Ns3DOpBoxItem.h"
#include "Ns3DGraphicsItem.h"
#include "Ns3DConstructionGridItem.h"
#include "Ns3DManipulator.h"
#include "Ns3DTManipulator.h"
#include "Ns3DSManipulator.h"
#include "Ns3DRManipulator.h"

#include "Ns3DCameraScope.h"
#include "Ns3DFieldScope.h"

#include "NsMainWindow.h"
#include "NsPreferences.h"

// thinks
//#include "NsCameraScopeItem.h"
#include "NsOpStore.h"
#include "NsOpObject.h"
#include "NsMessageWidget.h"
#include "NsPreferences.h"
#include "NsCmdCentral.h"
#include "NsCmdSelectAll.h"
#include "NsCmdSetCurrentVisibleFrame.h"
#include "NsCmdSetMeta.h"
#include "NsGraphCallback.h"
#include "NsPlayblastProgressWidget.h"

#include <em_mat44_algo.h>

#include <NgDummyBodyOp.h>

// Ngl
#include <Ngl.h>
#include <NglExtensions.h>
#include <NglState.h>

#include <sstream>

// -----------------------------------------------------------------------------

// Ns3DView
// --------
//! CTOR.

Ns3DView::Ns3DView(Ns3DScene *scene, QWidget *parent)
    : QGLWidget(
#if 0
          new Core3_2_context(
#endif
                QGLFormat(
                QGL::DoubleBuffer    |
                QGL::DepthBuffer     |
                QGL::StencilBuffer   |
                QGL::SampleBuffers   | // Multisampling.
                QGL::AlphaChannel    |
                QGL::Rgba            | // Don't use indexed colors.
                QGL::DirectRendering | // Render directly to GPU buffers.
                QGL::NoAccumBuffer   |
                QGL::NoStereoBuffers |
                QGL::NoOverlay,
                0), parent)
#if 0
          )
#endif
    , _scene(scene)
    , _drawOriginGridAction(0)  // Null.
    , _drawHudAction(0)         // Null.
    , _drawAxesAction(0)        // Null.
    , _drawItemLabelsAction(0)
    , _drawBodyLabelsAction(0)
    , _frameSceneAction(0)      // Null.
    , _playblastAction(0)       // Null.
    , _stepPlayblastAction(0)       // Null.
    , _manipActionGroup(0)
    , _translateManipAction(0)
    , _rotateManipAction(0)
    , _scaleManipAction(0)
    , _nullManipAction(0)
    , _drawSelection(true)
    , dragging(false)
    , glInitd(false)

{
    setFocusPolicy(Qt::StrongFocus); // Receive keyboard events
    setAutoFillBackground(false);    // Don't want this since we're mixing in GL
    setAutoBufferSwap(false);        // We do our own swapping (like Pro's!).

    //_scene->setParent(this);         // Own scene.

    _createActions();
    _onReadSettings();
}


// ~Ns3DView
// ---------
//! DTOR.

Ns3DView::~Ns3DView()
{
    _onWriteSettings();

    Ngl::End();
}


void
Ns3DView::setScene(Ns3DScene *scene)
{
    _scene = scene;
    //_scene.setParent(this); // Take ownership.
}

// -----------------------------------------------------------------------------

// playblast
// ---------
//! Exports the given frame range as a sequence of image files in the given
//! image format.

bool
Ns3DView::playblast(const QString &path,
                    const QString &fileNameBase,
                    const int      firstFrame,
                    const int      lastFrame,
                    const int      width,
                    const int      height,
                    const QString &format,
                    const int      quality)
{
    QDir d;
    if (!d.mkpath(path)) {
        return false;
    }

    _drawSelection = false;

    QProgressDialog pd("", "Cancel", firstFrame, lastFrame);
    pd.setWindowModality(Qt::WindowModal);
    pd.setWindowTitle("Playblast Progress");
    pd.setMinimumWidth(300);

    //this->updatesEnabled(false);

    bool success = true;
    int cvf(1);
    queryCurrentVisibleFrame(&cvf); // Ignore success.

    QGLFramebufferObject playblastFbo(width,
                                      height,
                                      QGLFramebufferObject::Depth);
    
    const int w0(_viewport.width());
    const int h0(_viewport.height());

    _viewport.setWidth(width);
    _viewport.setHeight(height);

    for (int frame = firstFrame; frame <= lastFrame; ++frame) {
        //Set progress value.

        pd.setValue(frame);
        pd.show();

        // If user has clicked cancel button of dialog.

        if (pd.wasCanceled()) {
             break;
        }

        const QString fileName(path + "/" + fileNameBase + QString(".%1%2.%3").
            arg(frame < 0 ? "-" : "").
            arg(qAbs(frame), 4, 10, QLatin1Char('0')).
            arg(format == "JPEG" ? "jpg" : format.toLower()));

        playblastFbo.bind();
        glPushAttrib(GL_COLOR_BUFFER_BIT |
                     GL_DEPTH_BUFFER_BIT |
                     GL_VIEWPORT_BIT);

        //glClearColor(0.69f, 0.71f, 0.72f, 1.f);
        //glClearDepth(1.0);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glViewport(0, 0, width, height);

        // Advance frame so that new stuff gets drawn in 3D view.

        NsCmdSetCurrentVisibleFrame::exec(frame);
        paintGL();

        glPopAttrib();

        const QImage image(playblastFbo.toImage());
        QImageWriter imageWriter;
        imageWriter.setFormat(format.toAscii());
        imageWriter.setQuality(quality);
        imageWriter.setFileName(fileName);
        imageWriter.setText("Creator", "Naiad(tm) Studio Playblast");

        playblastFbo.release();

        if(imageWriter.write(image)) {
            NsMessageWidget::instance()->clientInfo(
                tr("Exported frame ") + QString::number(frame) +
                tr(" to ") + fileName);
        }
        else {
            NsMessageWidget::instance()->clientError(
                tr("Failed to export frame ") + QString::number(frame) +
                tr(" to ") + fileName + ": " + imageWriter.errorString());
            success = false;
            break;
        }
    }

    //playblastFbo.release();

    // Restore frame from before playblast.

    NsCmdSetCurrentVisibleFrame::exec(cvf);

    _viewport.setWidth(w0);
    _viewport.setHeight(h0);

    if (success) {
        NsMessageWidget::instance()->clientInfo(
            tr("Image sequence succesfully exported to ") + path);
    }
    else {
        NsMessageWidget::instance()->clientError(
            tr("Failed to export image sequence to ") + path);
    }

    _drawSelection = true;

    return success;
}

// -----------------------------------------------------------------------------

void
Ns3DView::updateGL()
{
    QGLWidget::updateGL();
}

// -----------------------------------------------------------------------------

// onFrameScene
// ------------
//! [slot]

void
Ns3DView::onFrameScene()
{
    Ns3DCameraScope *cam = _activeCameraScope();
    if (0 != cam && 0 != _scene) {

        const bool validBodyBBox =
            (_sceneMin[0] < _sceneMax[0] &&
             _sceneMin[1] < _sceneMax[1] &&
             _sceneMin[2] < _sceneMax[2]);

        if (_scene->items().isEmpty() && !validBodyBBox) {
            // No items and no bodies, frame construction grid.

            if (0 != _scene->constructionGridItem() &&
                _drawOriginGridAction->isChecked()) {
                // There is a grid and it is visible.

                cam->frame(
                    _scene->constructionGridItem()->worldBoundingBox(),
                    _viewport);
            }
        }
        else {
            Ns3DBBox bbox;
            const QList<Ns3DGraphicsItem*> selItems =
                _scene->selectedItems();

            if (!selItems.isEmpty()) {
                // There are selected items, frame them.

                _scene->selectedItemsWorldBoundingBox(bbox);
            }
            else {
                // Frame union of all items and potential bodies.

                _scene->itemsWorldBoundingBox(bbox);
                if (validBodyBBox) {
                    bbox.unite(
                        Ns3DBBox(
                            _sceneMin[0], _sceneMax[0],
                            _sceneMin[1], _sceneMax[1],
                            _sceneMin[2], _sceneMax[2]));
                }
            }
            cam->frame(bbox, _viewport);
        }
    }
}


void
Ns3DView::onEndTimeStep(const NtTimeBundle &tb)
{
    updateGL();
}


// onEndFrame
// ----------
//! [slot]

void
Ns3DView::onEndFrame(const NtTimeBundle &tb)
{
    if (_stepPlayblastAction->isChecked()) {

        NsOpObject *globalOp(NsOpStore::instance()->mutableGlobalOp());

        QString path;

        NsMainWindow *mw(dynamic_cast<NsMainWindow*>(parent()->parent()));
        if (0 != mw) {
            path = mw->graphFileName();
        }

        if (!globalOp->metaValue("Step Playblast Path", &path)) {
            path = QDir::fromNativeSeparators(path);
            if (path.endsWith(".ni")) {
                path = path.left(path.length() - 3);
            }
            else if (!path.endsWith("/")) {
                path += "/";
            }

            path += ".stepplayblast";

            NsCmdSetMeta::exec("Global", "Step Playblast Path", path);
        }

        QDir d;
        if (!d.mkpath(path)) {
            return;
        }

        QString fileNameBase;
        if (0 != mw) {
            fileNameBase = QFileInfo(mw->graphFileName()).completeBaseName();
        }

        if (!globalOp->metaValue("Step Playblast File Name", &fileNameBase)) {
            NsCmdSetMeta::exec("Global",
                               "Step Playblast File Name",
                               fileNameBase);
        }

        QString format = "jpg";
        int quality = 80;

        int cvf(1);
        queryCurrentVisibleFrame(&cvf);

        const QString fileName(
            _playblastFileName(path, fileNameBase, format, cvf));

        QGLFramebufferObject fbo(_viewport.width(),
                                 _viewport.height(),
                                 QGLFramebufferObject::Depth);

        fbo.bind();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        paintGL();
        glPopAttrib();
        fbo.release();

        const QImage image(fbo.toImage());
        QImageWriter imageWriter;
        imageWriter.setFormat(format.toAscii());
        imageWriter.setQuality(quality);
        imageWriter.setFileName(fileName);
        imageWriter.setText("Creator", "Naiad(tm) Studio Playblast");

        if(imageWriter.write(image)) {
            NsMessageWidget::instance()->clientInfo(
                tr("Exported image to ") + imageWriter.fileName());
        }
        else {
            NsMessageWidget::instance()->clientError(
                tr("Failed to export image to ") +
                imageWriter.fileName() + ": " + imageWriter.errorString());
        }
    }

    updateGL();
}

void
Ns3DView::onCurrentVisibleFrameChanged(const int  cvf,
                                       const bool update3DView,
                                       const bool success)
{
    if (success && update3DView) {
        const QList<NsOpObject *> ops(
            NsOpStore::instance()->mutableOpFamily("CAMERA_SCOPE"));

        foreach (NsOpObject *op, ops) {
            Ns3DCameraScope* camScope(
                dynamic_cast<Ns3DCameraScope *>(
                    &op->mutableNgOp()));

            if (0 != camScope) {
                camScope->currentVisibleFrameChanged(cvf);
            }
        }

        updateGL();
    }
}

// -----------------------------------------------------------------------------

// initializeGL
// ------------
//! OpenGL initialization. Called once each time a new context is set.

void
Ns3DView::initializeGL()
{
    qDebug() << "Ns3DView::initializeGL";

    QGLWidget::initializeGL();  // Parent method.

    QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);

    // Bind OpenGL extensions before anything else happens...

    Ngl::Begin();

    if (!Ngl::getGLExtensionFunctions().resolve(context())) {
        QMessageBox::critical(
            0,
            tr("Naiad Studio - Error"),
            tr("Failed to resolve required OpenGL extensions."),
            QMessageBox::Ok, QMessageBox::Ok);
    }

    // Default OpenGL state. When objects below are destroyed they
    // will set default values.

    Ngl::FlipState<GL_DEPTH_TEST>          depthTestState;
    Ngl::FlipState<GL_MULTISAMPLE>         multisampleState;
    Ngl::FlipState<GL_ALPHA_TEST>          alphaTestState;
    Ngl::FlipState<GL_AUTO_NORMAL>         autoNormalState;
    Ngl::FlipState<GL_BLEND>               blendState;
    Ngl::FlipState<GL_COLOR_MATERIAL>      colorMaterialState;
    Ngl::FlipState<GL_CULL_FACE>           cullFaceState;
    Ngl::FlipState<GL_FOG>                 fogState;
    Ngl::FlipState<GL_LIGHTING>            lightingState;
    Ngl::FlipState<GL_LINE_SMOOTH>         lineSmoothState;
    Ngl::FlipState<GL_LINE_STIPPLE>        lineStippleState;
    Ngl::FlipState<GL_NORMALIZE>           normalizeState;
    Ngl::FlipState<GL_POINT_SMOOTH>        pointSmoothState;
    Ngl::FlipState<GL_POLYGON_OFFSET_FILL> polygonOffsetFillState;
    Ngl::FlipState<GL_POLYGON_OFFSET_LINE> polygonOffsetLineState;
    Ngl::FlipState<GL_POLYGON_STIPPLE>     polygonStippleState;
    Ngl::FlipState<GL_SCISSOR_TEST>        scissorTestState;
    Ngl::FlipState<GL_STENCIL_TEST>        stencilTestState;
    Ngl::FlipState<GL_TEXTURE_1D>          texture1DState;
    Ngl::FlipState<GL_TEXTURE_2D>          texture2DState;
    Ngl::FlipState<GL_TEXTURE_3D>          texture3DState;

    Ngl::ActiveTextureState                activeTextureState;
    Ngl::LightModelState                   lightModelState;
    Ngl::MatrixModeState                   matrixModeState;
    Ngl::ShadeModelState                   shadeModelState;
    Ngl::PixelStoreState                   pixelStoreState;
    Ngl::PolygonModeState                  polygonModeState;
    Ngl::DepthRangeState                   depthRangeState;


    // Hints

    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    qDebug() << "Context: " << context()->currentContext()->isValid();
    qDebug() << "OpenGL: {" << format().hasOpenGL() << "}";
    qDebug() << "OpenGL Shaders: {" <<
            QGLShaderProgram::hasOpenGLShaderPrograms(context()) << "}";
    qDebug() << "OpenGL FBO: {" <<
            QGLFramebufferObject::hasOpenGLFramebufferObjects() << "}";

    qDebug() << "GL_VENDOR: " <<
            QString(reinterpret_cast<const char*>(
                    glGetString(GL_VENDOR)));
    qDebug() << "GL_RENDERER: " <<
            QString(reinterpret_cast<const char*>(
                    glGetString(GL_RENDERER)));
    qDebug() << "GL_VERSION: " <<
            QString(reinterpret_cast<const char*>(
                    glGetString(GL_VERSION)));
    qDebug() << "GL_SHADING_LANGUAGE_VERSION: " <<
            QString(reinterpret_cast<const char*>(
                    glGetString(GL_SHADING_LANGUAGE_VERSION)));

    GLint i;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &i);
    qDebug() << "GL_MAX_TEXTURE_SIZE: " << i;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &i);
    qDebug() << "GL_MAX_3D_TEXTURE_SIZE: " << i;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &i);
    qDebug() << "GL_MAX_TEXTURE_UNITS: " << i;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &i);
    qDebug() << "GL_MAX_VERTEX_ATTRIBS: " << i;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &i);
    qDebug() << "GL_MAX_DRAW_BUFFERS: " << i << "\n";

    qDebug() << "Double Buffer: {" << format().doubleBuffer() << "}";
    qDebug() << "Depth Buffer: {" << format().depth() << "} ["
             << format().depthBufferSize() << "bits]";
    qDebug() << "Accum Buffer: {" << format().accum() << "} ["
             << format().accumBufferSize() << "bits]";
    qDebug() << "Alpha Buffer: {" << format().alpha() << "} ["
             << format().alphaBufferSize() << "bits]";
    qDebug() << "Stencil Buffer: {" << format().stencil() << "} ["
             << format().stencilBufferSize() << "bits]";
    qDebug() << "Sample Buffers: {" << format().sampleBuffers() << "} ["
             << format().samples() << "samples]";
    qDebug() << "RGBA: {" << format().rgba() << "} ["
             << format().redBufferSize() << ","
             << format().greenBufferSize() << ","
             << format().blueBufferSize() << ","
             << format().alphaBufferSize() << "bits]\n";

    _viewport.setX(0);
    _viewport.setY(0);
    _viewport.setWidth(std::max<GLsizei>(1, width()));
    _viewport.setHeight(std::max<GLsizei>(1, height()));
}


// resizeGL
// --------
//! OpenGL resizing. Called by resize events.

void
Ns3DView::resizeGL(const int w, const int h)
{
    QGLWidget::resizeGL(w, h);  // Parent method.

    _viewport.setX(0);
    _viewport.setY(0);
    _viewport.setWidth(w);
    _viewport.setHeight(h);

    _selMgr.resizeIdBuffer(_viewport);
}


// paintGL
// -------
//! This function is called whenever the widget needs to be painted.

void
Ns3DView::paintGL()
{
    QGLWidget::paintGL();   // Parent method.

    

    glViewport(_viewport.x(),
               _viewport.y(),
               _viewport.width(),
               _viewport.height());

    Ns3DCameraScope *cam(_activeCameraScope());

    _clear(cam);
//    glClearColor(0.69f, 0.71f, 0.72f, 1.f);
//    glClearDepth(1.0);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if (0 != cam) {
        _drawScene(*cam, _drawSelection);
    }
    else {
        

        QPainter painter(this);
        painter.setPen(QPen(QColor("#dfdfb9"), 5));
        painter.setRenderHint(QPainter::TextAntialiasing);

        QRect wbounds(painter.window());
        const QString text(tr("No active camera\n"));
        painter.setFont(QFont("Sans", 32));
        QRect tbounds(painter.boundingRect(wbounds, Qt::AlignCenter, text));
        painter.drawText(wbounds.width() - tbounds.width() - 10,
                         10,
                         tbounds.width(),
                         10 + tbounds.height(),
                         Qt::AlignCenter, 
                         text);
        painter.end();
    }

    swapBuffers();
}



// glInit
// ------
//! Calls initializeGL.

void
Ns3DView::glInit()
{
    QGLWidget::glInit();    // Parent method.
}


// glDraw
// ------
//! Calls paintGL and makes sure OpenGL is initialized, sets up context etc.

void
Ns3DView::glDraw()
{
    QGLWidget::glDraw();
}

// -----------------------------------------------------------------------------



// mousePressEvent
// ---------------
//! A mouse button has been pressed.

void
Ns3DView::mousePressEvent(QMouseEvent *event)
{
    Ns3DCameraScope *cam(_activeCameraScope());

    if (!cam) {
        return;
    }

    const int x(event->pos().x());
    const int y(_viewport.height() - 1 - event->pos().y());

    if (Qt::AltModifier == event->modifiers()) {
/*
        // Initialize camera motion in case user continues to drag mouse.
        // Grab raw z from depth buffer

        GLfloat camZ;
        Ngl::vec3f ws;
        GLfloat depth;
        glReadBuffer(GL_FRONT);
        glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

        //cameraScope()->unproject(x, y, depth, &camZ, &ws);
        //cameraScope()->setAnchor(ws, camZ);
*/
        cam->initDrag(x, y, _viewport);
    }
    else if (Qt::LeftButton == event->button()) {
        // No modifier-keys pressed

        const int32_t pid(_selMgr.queryIdPixel(x, y));

        if (Ns3DManipulator::isManipId(pid) && _scene->hasManip()) {
            // There is a manipulator and one of its components has been
            // clicked. Selection does not change.

            //const Ns3DCameraScope *cam(cameraScope());

            _scene->manip()->mousePress(_viewport,
                                       cam->modelviewMat(),
                                       cam->projectionMat(),
                                       pid,
                                       x, y);
        }
        else {
            // Mouse click is not on a manipulator, start selection rubber-band

            _selMgr.setAreaStart(x, y);
            _selMgr.setAreaEnd(x, y);
        }

        updateGL();
    }

    if (!dragging) {
        dragging = true;

        dragStart.setX(x);
        dragStart.setY(y);

        lastPos.setX(x);
        lastPos.setY(y);
    }
}


// mouseMoveEvent
// --------------
//! The mouse has been moved.

void
Ns3DView::mouseMoveEvent(QMouseEvent *event)
{
    Ns3DCameraScope *cam(_activeCameraScope());

    if (!cam) {
        return;
    }

    if (dragging)
    {
        const int x = event->pos().x();
        const int y = _viewport.height() - 1 - event->pos().y();
        const bool alt =  Qt::AltModifier & event->modifiers();
        const bool meta = Qt::MetaModifier & event->modifiers();
        const bool ctrl = Qt::ControlModifier & event->modifiers();
        const bool lmb =  Qt::LeftButton & event->buttons();
        const bool mmb =  Qt::MidButton & event->buttons();
        const bool rmb =  Qt::RightButton & event->buttons();

        if ((alt && rmb) || (alt && meta && lmb)) {
            cam->mouseRightDrag(x, y, lastPos.x(), lastPos.y(), _viewport);
        }
        else if (alt && lmb) {
            cam->mouseLeftDrag(x, y, lastPos.x(), lastPos.y(), _viewport);
        }
        else if ((alt && mmb) || (meta && lmb)) {
            cam->mouseMidDrag(x, y, lastPos.x(), lastPos.y(), _viewport);
        }
        else if (lmb) {
            // mouse motion in pixels
            const int dx(lastPos.x() - x);
            const int dy(lastPos.y() - y);

            if (_scene->hasManip() && _scene->manip()->isPressed()) {
                _scene->manip()->mouseDrag(_viewport,
                                          cam->modelviewMat(),
                                          cam->projectionMat(),
                                          dx, dy,
                                          x, y);
            }
            else {
                // Clamp to viewport

                const int sx(
                    std::min<int>(
                        std::max<int>(x, _viewport.x()),
                        _viewport.width() - 1));
                const int sy(
                    std::min<int>(
                        std::max<int>(y, _viewport.y()),
                        _viewport.height() - 1));

                _selMgr.setAreaEnd(sx, sy);
             }
        }

        lastPos.setX(x);
        lastPos.setY(y);
        update();
    }
}


// mouseReleaseEvent
// -----------------
//! A mouse button has been released.

void
Ns3DView::mouseReleaseEvent(QMouseEvent *event)
{
    Ns3DCameraScope *cam(_activeCameraScope());

    if (!cam) {
        return;
    }

    dragging = false;

    if (Qt::AltModifier == event->modifiers()) {
        // TODO: Mouse release after camera movement. Do nothing?

        if (_scene->hasManip() && _scene->manip()->isPressed()) {
            // Mouse was released after manipulator interaction

            _scene->manip()->mouseRelease();
        }

    }
    else {
        if (_scene->hasManip() && _scene->manip()->isPressed()) {
            // Mouse was released after manipulator interaction

            _scene->manip()->mouseRelease();
        }
        else {
            // Item selection is updated when mouse is released

            NsCmdSelectAll::exec(false);

            const std::vector<int32_t> idVec(
                _selMgr.queryIdImage(_selMgr.areaMinX(),
                                     _selMgr.areaMinY(),
                                     _selMgr.areaMaxX(),
                                     _selMgr.areaMaxY()));

            for (std::vector<int32_t>::size_type i(0); i < idVec.size(); ++i) {
                qDebug() << "Select Id:" << idVec[i];
                _scene->selectItem(idVec[i]);
            }

            _selMgr.clearArea();
        }
    }

    update();
}


// keyPressEvent
// -------------
//! A key has been pressed.

void
Ns3DView::keyPressEvent(QKeyEvent *event)
{
    //switch (event->key()) {
    //case (Qt::Key_W):
    //    // Set manipulator to be translating
    //    _scene->setManip(new Ns3DTManipulator(_scene->undoStack()));
    //    _selMgr.clearArea();
    //    break;
    //case (Qt::Key_E):
    //    // Set manipulator to be rotating
    //    _scene->setManip(new Ns3DRManipulator(_scene->undoStack()));
    //    _selMgr.clearArea();
    //    break;
    //case (Qt::Key_R):
    //    // Set manipulator to be scaling
    //    _scene->setManip(new Ns3DSManipulator(_scene->undoStack()));
    //    _selMgr.clearArea();
    //    break;
    //case (Qt::Key_Q):
    //    _scene->setManip(0);     // No manipulator
    //    break;
    //default:
    //    break;
    //}
    //update();

    //QGLWidget::keyPressEvent(event);
}


// -----------------------------------------------------------------------------

void
Ns3DView::_onValueChanged(const QString &valueLongName,
                          const QString &expr,
                          const int      component,
                          const bool     success)
{
    if (success) {
        const QList<NsOpObject *> ops(
            NsOpStore::instance()->mutableOpFamily("CAMERA_SCOPE"));

        foreach (NsOpObject *op, ops) {
            Ns3DCameraScope* camScope(
                dynamic_cast<Ns3DCameraScope *>(
                    &op->mutableNgOp()));
            if (0 != camScope) {
                Nb::String vln(fromQStr(valueLongName));
                const QString opInstance(fromNbStr(vln.parent()));
                const QString paramName(fromNbStr(vln.child()));

                camScope->updateParam(opInstance,
                                      paramName,
                                      expr,
                                      component);
            }
        }
    }

    update();


    // Pass on information about the changed parameter to camera, if any.

//    Ns3DCameraScope *cam(_activeCameraScope());

//    if (cam) {
//        cam->updateParam(opName, parameterName, newValue, component);
//    }
}




// -----------------------------------------------------------------------------


void
Ns3DView::_drawScene(Ns3DCameraScope &cam, const bool drawSelection)
{
    std::stringstream ssHud;

    // GL state altered in this function.

    const Ngl::FlipState<GL_DEPTH_TEST>  depthTestState;
    const Ngl::MatrixModeState           matrixModeState;
    const Ngl::ShadeModelState           shadeModelState;


    cam.loadBody(_viewport);
    cam.updateModelview();
    cam.updateProjection(_viewport);
    cam.hudInfo(&ssHud);

    Ngl::MatrixModeState::set(GL_PROJECTION);
    glLoadMatrixf(cam.projection());
    Ngl::MatrixModeState::set(GL_MODELVIEW);
    glLoadMatrixf(cam.modelview());

    Ngl::FlipState<GL_DEPTH_TEST>::enable();

    // Draw every scope in the graph that is active.

    QList<Ns3DBodyScope::LabelInfo> bodyLabels;
    _drawActiveScopes(cam, &ssHud, bodyLabels);

    // Draw graphics (Op) items.

    QList<Ns3DGraphicsItem::LabelInfo> itemLabels;
    _drawOpItems(cam, _scene->items(), itemLabels);

#if 0
    // TMP!!
    Ns3DBBox bbox;
    if (_scene->itemsWorldBoundingBox(bbox)) {
        _drawBBox(bbox);
    }
#endif
    

    // Draw the construction grid.

//    if (_drawOriginGridAction->isChecked()) {
//        _drawOriginGrid();

    if (0 != _scene->constructionGridItem() &&
        _drawOriginGridAction->isChecked()) {
        _scene->constructionGridItem()->draw();
    }

    if (cam.showPivot(Nb::ZeroTimeBundle)) {
        _drawCameraPivot(cam);
    }


    // Selection & Manipulation.

    if (_scene->hasManip()) {
        //glPolygonMode(GL_FRONT,GL_FILL);
        //const Ngl::MatrixModeState           matrixModeState;
        //Ngl::MatrixModeState::set(GL_PROJECTION);
        //GLfloat p[16];
        //glGetFloatv(GL_PROJECTION_MATRIX, p);
        //p[10] = 1;
        //p[11] = -1;
        //glPushMatrix();
        //glLoadMatrixf(p);
        _scene->manip()->draw(_viewport,
                              cam.modelviewMat(),
                              cam.projectionMat());
        //glPopMatrix();
    }

    if (drawSelection) {

        if (_selMgr.hasArea()) {
            _selMgr.drawArea(_viewport);
        }

        Ngl::ShadeModelState::set(GL_FLAT);

        //_selMgr.resizeIdBuffer(_viewport);  // TMP!!
        _selMgr.drawIdBuffer(_viewport,
                             cam.modelviewMat(),
                             cam.projectionMat(),
                             *_scene);
    }

    if (_drawAxesAction->isChecked()) {
        _drawAxes(cam.modelviewMat());
    }

    // OpenGL rendering is done - now do any 2D QPainter stuff.
    // Draw a HUD in the 3D view.

    _drawHud(fromNbStr(ssHud.str()), itemLabels, bodyLabels);

    //QPainter painter(this);
    //painter.setPen(QPen(QColor("#dfdfb9"),5));
    //painter.setRenderHint(QPainter::TextAntialiasing);

//    if (Ns::isSolving()) {
//        paintLiveSolveInfo(&painter);
//    }

    //painter.end();
}


// _drawHud
// --------
//! DOCS

void
Ns3DView::_drawHud(const QString &str,
                   const QList<Ns3DGraphicsItem::LabelInfo> &itemLabels,
                   const QList<Ns3DBodyScope::LabelInfo> &bodyLabels)
{
    QPainter painter(this);

    if (_drawHudAction->isChecked()) {
        // Draw HUD.

        painter.setPen(QPen(NsPreferences::instance()->scopeViewHudTextColor()));
        painter.setFont(QFont("Sans",10));
        painter.setRenderHint(QPainter::TextAntialiasing);
        QRect wbounds(painter.window());
        const QRect tbounds(painter.boundingRect(wbounds, Qt::AlignLeft, str));

        painter.drawText(5,
                         5,
                         tbounds.width(),
                         5 + tbounds.height(),
                         Qt::AlignLeft,
                         str);
    }

    if (_drawItemLabelsAction->isChecked()) {
        // Draw item labels.

        if (!itemLabels.isEmpty()) {
            painter.setPen(QPen(Qt::black));
            painter.setFont(QFont("Sans",10));
            painter.setRenderHint(QPainter::TextAntialiasing);
            foreach (const Ns3DGraphicsItem::LabelInfo &label, itemLabels) {
                if (!label.text().isEmpty()) {
                    painter.setPen(QPen(label.color()));
                    painter.drawText(
                        QPointF(label.pos().x(), height() - label.pos().y() - 1),
                        label.text());
                }
            }
        }
    }

    if (_drawBodyLabelsAction->isChecked()) {
        // Draw body labels.B

        if (!bodyLabels.isEmpty()) {
            painter.setPen(QPen(Qt::black));
            painter.setFont(QFont("Sans",10));
            painter.setRenderHint(QPainter::TextAntialiasing);
            foreach (const Ns3DBodyScope::LabelInfo &label, bodyLabels) {
                if (!label.text().isEmpty()) {
                    painter.setPen(QPen(label.color()));
                    painter.drawText(
                        QPointF(label.pos().x(), height() - label.pos().y() - 1),
                        label.text());
                }
            }
        }
    }

    painter.end();
}


// _drawActiveScopes
// -----------------
//! Draws all active scopes in the 3D view.

void
Ns3DView::_drawActiveScopes(const Ns3DCameraScope &cam,
                            std::stringstream     *ss,
                            QList<Ns3DBodyScope::LabelInfo> &labels)
{
#if 0
    static GLuint envMapHandle=0xffffffff;
    if(envMapHandle==0xffffffff) {
        envMapHandle=bindTexture(
            QImage(QString("/home/marcus/Download/maldives.png")),
            //QImage(QString("/home/marcus/Download/tranquility.jpg")),
            GL_TEXTURE_2D,
            GL_RGBA,
            QGLContext::LinearFilteringBindOption
            );
    }
#endif

    try {
        _sceneMin[0] = std::numeric_limits<float>::max();
        _sceneMin[1] = std::numeric_limits<float>::max();
        _sceneMin[2] = std::numeric_limits<float>::max();
        _sceneMax[0] = -std::numeric_limits<float>::max();
        _sceneMax[1] = -std::numeric_limits<float>::max();
        _sceneMax[2] = -std::numeric_limits<float>::max();


        // let the body scopes draw...

        int cvf(1);
        queryCurrentVisibleFrame(&cvf);

        const NtStringList bodyScopes =
            NiQueryOpNames(NI_INSTANCE, "BODY_SCOPE");

        for(unsigned int i(0); i<bodyScopes.size(); ++i) {
            Ng::Op* op(NiMutableOp(bodyScopes[i]));

            Nb::MessageCallback::pushValueObject(op);

            // Let the body scopes draw whatever bodies they admit.

            Ns3DBodyScope *bs(dynamic_cast<Ns3DBodyScope*>(op));
            Ns3DCameraScope *cs(dynamic_cast<Ns3DCameraScope*>(op));

            if (bs && !cs && bs->opState() == Ng::BodyOp::ActiveOpState) {
                bs->setContext(context());
                bs->drawBodies(&cam, _viewport, cvf);
                bs->hudInfo(ss);
                labels.append(bs->labels());

                // Get bounds of the bodies drawn by the scope.

                NtVec3f scopeMin(std::numeric_limits<float>::max(),
                                 std::numeric_limits<float>::max(),
                                 std::numeric_limits<float>::max());
                NtVec3f scopeMax(-std::numeric_limits<float>::max(),
                                 -std::numeric_limits<float>::max(),
                                 -std::numeric_limits<float>::max());

                bs->bounds(scopeMin, scopeMax);

                if (scopeMin[0] < scopeMax[0] &&
                    scopeMin[1] < scopeMax[1] &&
                    scopeMin[2] < scopeMax[2]) {

                    _sceneMin[0] = std::min<float>(_sceneMin[0], scopeMin[0]);
                    _sceneMin[1] = std::min<float>(_sceneMin[1], scopeMin[1]);
                    _sceneMin[2] = std::min<float>(_sceneMin[2], scopeMin[2]);
                    _sceneMax[0] = std::max<float>(_sceneMax[0], scopeMax[0]);
                    _sceneMax[1] = std::max<float>(_sceneMax[1], scopeMax[1]);
                    _sceneMax[2] = std::max<float>(_sceneMax[2], scopeMax[2]);
                }
            }

            if (!bs) {
                NB_WARNING("Op '" << op->name() <<
                           "' fraudulently claims to be a body scope when in "
                           << "fact it is a " << op->typeName());
            }

            Nb::MessageCallback::popValueObject();
        }

        // let the field scopes draw...
        const NtStringList fieldScopes=
            NiQueryOpNames(NI_INSTANCE, "FIELD_SCOPE");
        for(unsigned int i(0); i<fieldScopes.size(); ++i) {
            Ng::Op* op(NiMutableOp(fieldScopes[i]));

            Nb::MessageCallback::pushValueObject(op);

            // let the field scopes draw their fields
            Ns3DFieldScope *fs(dynamic_cast<Ns3DFieldScope*>(op));
            if(fs && fs->opState() == Ng::FieldOp::ActiveOpState) {
                fs->setContext(context());
                //fieldScope->envMapHandle=envMapHandle;
                fs->drawField(&cam,_viewport, cvf);
                fs->hudInfo(ss);

                // Get bounds of the bodies drawn by the scope.

                NtVec3f scopeMin(std::numeric_limits<float>::max(),
                                 std::numeric_limits<float>::max(),
                                 std::numeric_limits<float>::max());
                NtVec3f scopeMax(-std::numeric_limits<float>::max(),
                                 -std::numeric_limits<float>::max(),
                                 -std::numeric_limits<float>::max());

                fs->bounds(scopeMin, scopeMax);

                if (scopeMin[0] < scopeMax[0] &&
                    scopeMin[1] < scopeMax[1] &&
                    scopeMin[2] < scopeMax[2] ) {

                    _sceneMin[0] = std::min<float>(_sceneMin[0], scopeMin[0]);
                    _sceneMin[1] = std::min<float>(_sceneMin[1], scopeMin[1]);
                    _sceneMin[2] = std::min<float>(_sceneMin[2], scopeMin[2]);
                    _sceneMax[0] = std::max<float>(_sceneMax[0], scopeMax[0]);
                    _sceneMax[1] = std::max<float>(_sceneMax[1], scopeMax[1]);
                    _sceneMax[2] = std::max<float>(_sceneMax[2], scopeMax[2]);
                }
            }

            if (!fs) {
                NB_WARNING("Op '" << op->name() <<
                           "' fraudulently claims to be a field scope when in "
                           << "fact it is a " << op->typeName());
            }

            Nb::MessageCallback::popValueObject();
        }
    }
    catch (std::exception& e) {
        NB_ERROR("Scope draw error: " << e.what());
    }

    Nb::MessageCallback::clearValueObjectStack();
}


//// _drawOriginGrid
//// ---------------
////! Draws a grid centered at worldspace origin, where each cell in the grid
////! has dimensions equal to the MCS

//void
//Ns3DView::_drawOriginGrid()
//{
//    const float mcs(evalParam1f("Global.Master Cell Size"));
//    const int   count(21);
//    const int   hcount(count>>1); // Bit-shift divide by 2.

//    glLineWidth(1.);

//    glBegin(GL_LINES);

//    const NtVec3f gridColor(0.5f);
//    glColor3fv(&gridColor[0]);

//    for(int k(0); k < count; ++k) {
//        const float z((k - hcount)*mcs);
//        glVertex3f(-hcount*mcs, 0.f, z);
//        glVertex3f( hcount*mcs, 0.f, z);

//        glVertex3f(z, 0.f, -hcount*mcs);
//        glVertex3f(z, 0.f,  hcount*mcs);
//    }

//    glEnd();
//}


// _drawAxes
// ---------
//! DOCS

void
Ns3DView::_drawAxes(const Ngl::mat44f &mv)
{
    // Render the 3D axes.

    glViewport(0, 0, 75, 75);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); // Modelview.
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -4.f);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); // Projection
    glLoadIdentity();
    glFrustum(-0.5, 0.5, -0.5, 0.5, 1., 10.);
    //glOrtho(-0.5, 0.5, -0.5, 0.5, 1., 10.);

    glLineWidth(2.f);
    glBegin(GL_LINES);
    glColor3f(1.f, 0.f, 0.f);   // X-axis.
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(mv[0][0], mv[0][1], mv[0][2]);

    glColor3f(0.f, 1.f, 0.f);   // Y-axis.
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(mv[1][0], mv[1][1], mv[1][2]);

    glColor3f(0.f, 0.f, 1.f);   // Z-axis.
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(mv[2][0], mv[2][1], mv[2][2]);
    glEnd();

    glPopMatrix(); // Projection

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();  // Modelview
}


// _drawCameraPivot
// ----------------
//! DOCS

void
Ns3DView::_drawCameraPivot(const Ns3DCameraScope &cam)
{
    const NtVec3f piv(cam.pivot());

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    const Ngl::Material mat(
        Ngl::vec4f(0.3f, 0.2f, 0.3f, 1.f),
        Ngl::vec4f(0.8f, 0.2f, 0.8f, 1.f));
    mat.set();

    Ngl::Light light(Ngl::vec4f(1.f, 1.f, 1.f, 1.f));
    light.set();

    Ngl::Sphere sph(cam.pivotSize(Nb::ZeroTimeBundle), 64, piv);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_STIPPLE);
//        glEnable(GL_CULL_FACE);
//        glCullFace(GL_BACK);

    static const GLubyte halftone[] = {
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55 };
    glPolygonStipple(halftone);

    sph.draw();

//        glDisable(GL_CULL_FACE);
    glDisable(GL_POLYGON_STIPPLE);
    glEnable(GL_DEPTH_TEST);

    sph.draw();

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

void
Ns3DView::_drawOpItems(const Ns3DCameraScope &cam,
                       const QList<Ns3DGraphicsItem*> &items,
                       QList<Ns3DGraphicsItem::LabelInfo> &labels)
{
    foreach (Ns3DGraphicsItem* item, _scene->items()) {
        Ns3DOpItem* opItem(dynamic_cast<Ns3DOpItem*>(item));

        if (0 != opItem && opItem->isVisible() &&
            opItem->opObject()->longName() != fromNbStr(cam.name())) {

            Ns3DGraphicsItem::LabelInfo label;
            item->draw(&label);
            //itemLabels.append(label);
            labels.append(label);
        }
    }
}


void 
Ns3DView::_drawBBox(const Ns3DBBox &bbox)
{
    const Ngl::vec3f vtx[8] = {
        Ngl::vec3f(bbox.minX(), bbox.minY(), bbox.minZ()),
        Ngl::vec3f(bbox.maxX(), bbox.minY(), bbox.minZ()),
        Ngl::vec3f(bbox.maxX(), bbox.minY(), bbox.maxZ()),
        Ngl::vec3f(bbox.minX(), bbox.minY(), bbox.maxZ()),
        Ngl::vec3f(bbox.minX(), bbox.maxY(), bbox.minZ()),
        Ngl::vec3f(bbox.maxX(), bbox.maxY(), bbox.minZ()),
        Ngl::vec3f(bbox.maxX(), bbox.maxY(), bbox.maxZ()),
        Ngl::vec3f(bbox.minX(), bbox.maxY(), bbox.maxZ())
    };

    const Ngl::vec2i edge[12] = {
        Ngl::vec2i(0,1), Ngl::vec2i(1,2), Ngl::vec2i(2,3), Ngl::vec2i(3,0),
        Ngl::vec2i(4,5), Ngl::vec2i(5,6), Ngl::vec2i(6,7), Ngl::vec2i(7,4),
        Ngl::vec2i(0,4), Ngl::vec2i(1,5), Ngl::vec2i(2,6), Ngl::vec2i(3,7)
    };

    glColor3f(1.f, 0.f, 0.f);
    glBegin(GL_LINES);
    for (int e(0); e < 12; ++e) {
        for (int i(0); i < 2; ++i) {
            glVertex3fv(&vtx[edge[e][i]][0]);
        }
    }
    glEnd();
}


void
Ns3DView::_clear(Ns3DCameraScope *cam)
{
    glClearColor(
        NsPreferences::instance()->scopeViewBackground().red()/256.f,
        NsPreferences::instance()->scopeViewBackground().green()/256.f,
        NsPreferences::instance()->scopeViewBackground().blue()/256.f,
        1.f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (0 != cam && cam->hasImagePlane()) {
        GLint w(-1);
        GLint h(-1);
        cam->imagePlaneDimensions(w, h);

        const em::glmat44f modelview(
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f);

        const em::glmat44f projection(
            em::make_ortho_projection(
                 0.f, static_cast<float>(width()),
                 0.f, static_cast<float>(height()),
                -1.f, 1.f));

        const GLfloat wRatio(static_cast<GLfloat>(width())/w);
        const GLfloat hRatio(static_cast<GLfloat>(height())/h);

        const GLfloat stretch(qMin(hRatio, wRatio));
        const GLfloat stretchedW(w*stretch);
        const GLfloat stretchedH(h*stretch);

        const GLfloat halfDw(0.5f*(width() - stretchedW));
        const GLfloat halfDh(0.5f*(height() - stretchedH));

        const GLfloat xMin(halfDw);
        const GLfloat xMax(halfDw + stretchedW);
        const GLfloat yMin(halfDh);
        const GLfloat yMax(halfDh + stretchedH);

        // Vertices.

        GLfloat vtx[4][3] = {
            {xMin, yMin, 0.f},
            {xMax, yMin, 0.f},
            {xMax, yMax, 0.f},
            {xMin, yMax, 0.f}};

        // Texture coords.

        const GLfloat tex[4][2] = {
            {0.f, 0.f},
            {1.f, 0.f},
            {1.f, 1.f},
            {0.f, 1.f}};

        const Ngl::FlipState<GL_DEPTH_TEST> depthTestState;
        const Ngl::FlipState<GL_ALPHA_TEST> alphaTestState;
        const Ngl::FlipState<GL_BLEND>      blendState;
        Ngl::FlipState<GL_DEPTH_TEST>::disable();
        Ngl::FlipState<GL_ALPHA_TEST>::enable();
        Ngl::FlipState<GL_BLEND>::enable();

        glAlphaFunc(GL_GREATER, 0.f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE); // Disable writing to depth buffer.

        cam->drawImagePlane(modelview,
                            projection,
                            Ngl::VertexBuffer(4*3*sizeof(GLfloat), vtx),
                            Ngl::VertexBuffer(4*2*sizeof(GLfloat), tex));

        glDepthMask(GL_TRUE); // Enable writing to depth buffer.
    }
}


//// paintLiveSolveInfo
//// ----------------
////! Paints "Live Step" text in 3D view, along with current timestep etc..

//void
//Ns3DView::paintLiveSolveInfo(QPainter* painter)
//{
//    QRect wbounds(painter->window());
//    QString text = tr("Live Step");
//    painter->setFont(QFont("Sans",32));
//    QRect tbounds(painter->boundingRect(wbounds,Qt::AlignCenter,text));
//    painter->drawText(wbounds.width()-tbounds.width()-10,
//                      10,
//                      tbounds.width(),
//                      10+tbounds.height(),
//                      Qt::AlignCenter,text);
//}

// -----------------------------------------------------------------------------

// _onReadSettings
// ---------------
//! Reads settings from the given group. [slot]

void
Ns3DView::_onReadSettings()
{
    qDebug() << "Ns3DView::_onReadSettings";

    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       "Exotic Matter",
                       "Naiad Studio");

    settings.beginGroup("Ns3DView");

    // TODO: Read exactly what is needed to recreate current 3D view

    EM_ASSERT(0 != _drawOriginGridAction &&
              0 != _drawHudAction &&
              0 != _drawAxesAction);
    _drawOriginGridAction->setChecked(settings.value("DrawOriginGrid").toBool());
    _drawHudAction->setChecked(settings.value("DrawHUD").toBool());
    _drawAxesAction->setChecked(settings.value("DrawAxes").toBool());
    _drawItemLabelsAction->setChecked(settings.value("DrawItemLabels").toBool());
    _drawBodyLabelsAction->setChecked(settings.value("DrawBodyLabels").toBool());

    settings.endGroup();
}


// _onWriteSettings
// ----------------
//! Writes settings to the given group. [slot]

void
Ns3DView::_onWriteSettings()
{
    qDebug() << "Ns3DView::_onWriteSettings";

    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       "Exotic Matter",
                       "Naiad Studio");

    settings.beginGroup("Ns3DView");

    // TODO: Store exactly what is needed to recreate current 3D view

    EM_ASSERT(0 != _drawOriginGridAction &&
              0 != _drawHudAction &&
              0 != _drawAxesAction);

    settings.setValue("DrawOriginGrid",
                      QVariant(_drawOriginGridAction->isChecked()));
    settings.setValue("DrawHUD",
                      QVariant(_drawHudAction->isChecked()));
    settings.setValue("DrawAxes",
                      QVariant(_drawAxesAction->isChecked()));
    settings.setValue("DrawItemLabels",
                      QVariant(_drawItemLabelsAction->isChecked()));
    settings.setValue("DrawBodyLabels",
                      QVariant(_drawBodyLabelsAction->isChecked()));

    settings.endGroup();
}

void
Ns3DView::_onTranslateManip()
{
    // Set manipulator to be translating
    _scene->setManip(new Ns3DTManipulator(_scene->undoStack()));
    _selMgr.clearArea();
    update();
}

void
Ns3DView::_onRotateManip()
{
    // Set manipulator to be rotating
    _scene->setManip(new Ns3DRManipulator(_scene->undoStack()));
    _selMgr.clearArea();
    update();
}

void
Ns3DView::_onScaleManip()
{
    // Set manipulator to be scaling
    _scene->setManip(new Ns3DSManipulator(_scene->undoStack()));
    _selMgr.clearArea();
    update();
}

void
Ns3DView::_onNullManip()
{
    _scene->setManip(0);     // No manipulator
    update();
}

// -----------------------------------------------------------------------------

// createActions
// -------------
//! Creates the actions in this widget

void
Ns3DView::_createActions()
{
    if (0 == _drawOriginGridAction) {
        _drawOriginGridAction = new QAction(tr("Origin &Grid"), this); // Child.
        _drawOriginGridAction->setStatusTip(tr("Toggle the construction grid"));
        _drawOriginGridAction->setCheckable(true);
        _drawOriginGridAction->setChecked(true);
        connect(_drawOriginGridAction,
                SIGNAL(triggered()),
                SLOT(updateGL()));
    }

    if (0 == _drawHudAction) {
        _drawHudAction = new QAction(tr("Show &HUD"), this); // Child.
        _drawHudAction->setStatusTip(tr("Toggle the HUD"));
        _drawHudAction->setCheckable(true);
        _drawHudAction->setChecked(true);
        connect(_drawHudAction,
                SIGNAL(triggered()),
                SLOT(updateGL()));
    }

    if (0 == _drawAxesAction) {
        _drawAxesAction = new QAction(tr("Show &Axes"), this); // Child.
        _drawAxesAction->setStatusTip(tr("Toggle axes in 3D View"));
        _drawAxesAction->setCheckable(true);
        _drawAxesAction->setChecked(true);
        connect(_drawAxesAction,
                SIGNAL(triggered()),
                SLOT(updateGL()));
    }

    if (0 == _drawItemLabelsAction) {
        _drawItemLabelsAction = new QAction(tr("Show &Item labels"), this); // Child.
        _drawItemLabelsAction->setStatusTip(tr("Toggle item labels in 3D View"));
        _drawItemLabelsAction->setCheckable(true);
        _drawItemLabelsAction->setChecked(true);
        connect(_drawItemLabelsAction,
                SIGNAL(triggered()),
                SLOT(updateGL()));
    }

    if (0 == _drawBodyLabelsAction) {
        _drawBodyLabelsAction = new QAction(tr("Show &Body labels"), this); // Child.
        _drawBodyLabelsAction->setStatusTip(tr("Toggle body labels in 3D View"));
        _drawBodyLabelsAction->setCheckable(true);
        _drawBodyLabelsAction->setChecked(true);
        connect(_drawBodyLabelsAction,
                SIGNAL(triggered()),
                SLOT(updateGL()));
    }

    if (0 == _frameSceneAction) {
        _frameSceneAction= new QAction(tr("&Frame"), this); // Child.
        _frameSceneAction->setStatusTip(tr("Fit view"));
        connect(_frameSceneAction,
                SIGNAL(triggered()),
                SLOT(onFrameScene()));

        //TODO:
        //connect(_frameSceneAction, SIGNAL(triggered()), SLOT(updateGL()));
    }

    if (0 == _playblastAction) {
        _playblastAction= new QAction(tr("&Playblast"), this); // Child.
        _playblastAction->setStatusTip(tr("Open playblast dialog"));
        //TODO:
        // connect(...);
    }

    if (0 == _stepPlayblastAction) {
         // Child.

        _stepPlayblastAction = new QAction(tr("Playblast &Stepping"), this);
        _stepPlayblastAction->setStatusTip(tr("Toggle playblast stepping"));
        _stepPlayblastAction->setCheckable(true);
        _stepPlayblastAction->setChecked(false);
    }

    //QActionGroup 

    if (0 == _manipActionGroup) {
        _manipActionGroup = new QActionGroup(this); // Child.
        _manipActionGroup->setExclusive(true);
    }

    if (0 == _translateManipAction) {
        _translateManipAction = new QAction(tr("Translate Manipulator"), _manipActionGroup);
        _translateManipAction->setShortcut(Qt::Key_W);
        _translateManipAction->setStatusTip(
            tr("Translate items in the 3D View"));
        _translateManipAction->setCheckable(true);
        _translateManipAction->setChecked(false);
        connect(_translateManipAction, SIGNAL(triggered()), SLOT(_onTranslateManip()));
    }

    if (0 == _rotateManipAction) {
        _rotateManipAction = new QAction(tr("Rotate Manipulator"), _manipActionGroup);
        _rotateManipAction->setShortcut(Qt::Key_E);
        _rotateManipAction->setStatusTip(
            tr("Rotate items in the 3D View"));
        _rotateManipAction->setCheckable(true);
        _rotateManipAction->setChecked(false);
        connect(_rotateManipAction, SIGNAL(triggered()), SLOT(_onRotateManip()));
    }

    if (0 == _scaleManipAction) {
        _scaleManipAction = new QAction(tr("Scale Manipulator"), _manipActionGroup);
        _scaleManipAction->setShortcut(Qt::Key_R);
        _scaleManipAction->setStatusTip(
            tr("Scale items in the 3D View"));
        _scaleManipAction->setCheckable(true);
        _scaleManipAction->setChecked(false);
        connect(_scaleManipAction, SIGNAL(triggered()), SLOT(_onScaleManip()));
    }

    if (0 == _nullManipAction) {
        _nullManipAction = new QAction(tr("No Manipulator"), _manipActionGroup);
        _nullManipAction->setShortcut(Qt::Key_Q);
        _nullManipAction->setStatusTip(
            tr("Disable manipulators in the 3D View"));
        _nullManipAction->setCheckable(true);
        _nullManipAction->setChecked(true);
        connect(_nullManipAction, SIGNAL(triggered()), SLOT(_onNullManip()));
    }
}


// _activeCameraScope
// -----------
//! Returns a pointer to the graph's first (and only!) active Camera-Scope.

Ns3DCameraScope *
Ns3DView::_activeCameraScope() const
{
    Ns3DCameraScope *cam(0);    // Null.
    const QList<NsOpObject *> ops(
        NsOpStore::instance()->mutableOpFamily("CAMERA_SCOPE"));

    foreach (NsOpObject *op, ops) {
        Ns3DCameraScope* camScope(
            dynamic_cast<Ns3DCameraScope *>(&op->mutableNgOp()));

        if (0 != camScope && "ACTIVE" == op->state()) {
            cam = camScope;
        }
    }

    return cam;
}

// -----------------------------------------------------------------------------

QString
Ns3DView::_playblastFileName(const QString &path,
                             const QString &fileNameBase,
                             const QString &format,
                             const int      frame)
{
    QString fmt(format);

    if (!QImageWriter::supportedImageFormats().contains(fmt.toAscii())) {
        NsMessageWidget::instance()->clientWarning(
            tr("Image format ") + fmt +
            tr(" is not supported. Defaulting to JPEG."));
        fmt = "JPEG";
    }

    if ("JPEG" == fmt) {
        fmt = "jpg";
    }
    else {
        fmt = fmt.toLower();
    }

    return QString(path + "/" + fileNameBase + QString(".%1%2.%3").
                   arg(frame < 0 ? "-" : "").
                   arg(qAbs(frame), 4, 10, QLatin1Char('0')).
                   arg(fmt));
}

