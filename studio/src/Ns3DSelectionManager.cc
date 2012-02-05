// -----------------------------------------------------------------------------
//
// Ns3DSelectionManager.cc
//
// Class for managing selection.
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

#include "Ns3DSelectionManager.h"
#include "Ns3DGraphicsItem.h"
#include "Ns3DManipulator.h"
#include "Ns3DScene.h"
#include "Ns3DOpItem.h"
#include "NsQuery.h"
#include <NglState.h>
#include <NglViewport.h>

#include <Nbx.h>
#include <QString>
#include <set>


// Ns3DSelectionManager
// --------------------
//! Destroy instance, free memory.

Ns3DSelectionManager::~Ns3DSelectionManager()
{
    qDebug() << "~Ns3DSelectionManager";
    delete _idFbo;
}

// -----------------------------------------------------------------------------

// resizeIdBuffer
// --------------
//! Adapt size of renderbuffer (FBO) to viewport size.

void
Ns3DSelectionManager::resizeIdBuffer(const Ngl::Viewport &vp)
{
    if (0 == _idFbo) {


        // HACK: Suggested hacks to fix ATI FBO issues... don't work properly.
        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        _idFbo = new QGLFramebufferObject(vp.width(),
                                          vp.height(),
                                          QGLFramebufferObject::Depth,
                                          GL_TEXTURE_2D,
                                          GL_RGBA8);
        qDebug() << "Created FBO";
        return;
    }

    if (_idFbo->width()  != vp.width() ||
        _idFbo->height() != vp.height()) {
        qDebug() << "Ns3DSelectionManager::resizeIdBuffer";
        qDebug() << "Viewport: [" << vp.x() << vp.y() << vp.width() << vp.height() << "]";

        delete _idFbo;
        qDebug() << "Deleted FBO";

        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        _idFbo = new QGLFramebufferObject(vp.width(),
                                          vp.height(),
                                          QGLFramebufferObject::Depth,
                                          GL_TEXTURE_2D,
                                          GL_RGBA8);
        qDebug() << "Created FBO";
    }
}


// drawArea
// --------
//! Draw the current selection area to the screen.

void
Ns3DSelectionManager::drawArea(const Ngl::Viewport &vp) const
{
    const GLint xMin(areaMinX());
    const GLint xMax(areaMaxX());
    const GLint yMin(areaMinY());
    const GLint yMax(areaMaxY());

    const Ngl::vec2i v0(xMin, yMax);
    const Ngl::vec2i v1(xMin, yMin);
    const Ngl::vec2i v2(xMax, yMin);
    const Ngl::vec2i v3(xMax, yMax);

    Ngl::FlipState<GL_DEPTH_TEST>   depthTestState;
    Ngl::FlipState<GL_BLEND>        blendState;
    Ngl::MatrixModeState            matrixModeState;

    glViewport(vp.x(), vp.y(), vp.width(), vp.height());

    glPushMatrix();     // NB: Modelview
    glLoadIdentity();

    Ngl::MatrixModeState::set(GL_PROJECTION);
    //glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(vp.x(), vp.width(), vp.y(), vp.height(), -1.0, 1.0);

    Ngl::FlipState<GL_BLEND>::enable();
    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Ngl::FlipState<GL_DEPTH_TEST>::disable();
    //glDisable(GL_DEPTH_TEST);

    glColor4fv(&_areaQuadColor[0]);
    glBegin(GL_QUADS);
    glVertex2iv(&v0[0]);
    glVertex2iv(&v1[0]);
    glVertex2iv(&v2[0]);
    glVertex2iv(&v3[0]);
    glEnd();

    glLineWidth(_areaBorderWidth);
    glColor4fv(&_areaBorderColor[0]);
    glBegin(GL_LINE_LOOP);
    glVertex2iv(&v0[0]);
    glVertex2iv(&v1[0]);
    glVertex2iv(&v2[0]);
    glVertex2iv(&v3[0]);
    glEnd();

    //glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

    glPopMatrix();  // Projection
    Ngl::MatrixModeState::set(GL_MODELVIEW);//glMatrixMode(GL_MODELVIEW);
    glPopMatrix();  // Modelview
}


// drawIdBuffer
// ------------
//! Draw item selection representation into off-screen buffer (FBO).

void
Ns3DSelectionManager::drawIdBuffer(const Ngl::Viewport &vp,
                                   const em::glmat44f  &mv,
                                   const em::glmat44f  &proj,
                                   const Ns3DScene     &graph)
{
    if(0 != _idFbo) {
        Ngl::FlipState<GL_DEPTH_TEST> depthTestState;
        Ngl::MatrixModeState matrixModeState;

        glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

        _idFbo->bind();
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(vp.x(), vp.y(), vp.width(), vp.height());
        glDepthRange(0., 1.);
        Ngl::MatrixModeState::set(GL_PROJECTION);
        glLoadMatrixf(&proj[0][0]);
        Ngl::MatrixModeState::set(GL_MODELVIEW);
        glLoadMatrixf(&mv[0][0]);

        foreach (Ns3DGraphicsItem* item, graph.items()) {
            Ns3DOpItem* opItem(dynamic_cast<Ns3DOpItem*>(item));

            if (item->selectable() &&
                0 != opItem && opItem->isVisible()) {
                item->selectionDraw();
            }
        }

        // Draw manipulator on top of items.

        Ngl::FlipState<GL_DEPTH_TEST>::disable();
        if (graph.hasManip()) {
            graph.manip()->selectionDraw(vp, mv, proj);
        }

        _idFbo->release();
        _hasIdImage = false;// Next query will trigger transfer from GPU to CPU.
    }
}


// queryIdImage
// ------------
//! Iterate over specified region in ID-buffer and select items if their ID
//! is found in this region. NB: Does not unselect items that are not found
//! in the region.

std::vector<int32_t>
Ns3DSelectionManager::queryIdImage(const int x0, const int y0,
                                   const int x1, const int y1)
{
    if (!_hasIdImage && 0 != _idFbo) {
        // It is not strictly necessary to fetch the image data from the GPU
        // until a query is made, which may not happen every time the camera
        // moves.
        // TODO: It is probably much faster to transfer data in FBO to
        //       CPU using PBO, rather then extracting a copy of a QImage.

//        GLenum error = glGetError();
//        qDebug() << (error == GL_NO_ERROR);

        //glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

        _idImage = _idFbo->toImage();
        //_idImage.save("./id.png", "PNG", -1);   // TMP!!
        _hasIdImage = true;
    }

    // Clamp.

    const int xMin(std::max(std::min(x0, x1), 0));
    const int yMin(std::max(std::min(y0, y1), 0));
    const int xMax(std::min(std::max(x0, x1), _idImage.width() - 1));
    const int yMax(std::min(std::max(y0, y1), _idImage.height() - 1));

    QRgb pval;
    int32_t pid(0);
    std::set<int32_t> idSet;

    for (int px(xMin); px <= xMax; ++px) {
        for (int py(yMin); py <= yMax; ++py) {
            pval = _idImage.pixel(px, _idImage.height() - 1 - py);  // Flip y

            Ns3DSelectionManager::rgbaToId(
                Ngl::vec4uc(static_cast<GLubyte>(qRed(pval)),
                            static_cast<GLubyte>(qGreen(pval)),
                            static_cast<GLubyte>(qBlue(pval)),
                            static_cast<GLubyte>(qAlpha(pval))),
                &pid);

            // Build list of unique, positive Id's in selected region

            if (0 < pid) {
                qDebug() << "queryIdImage: " << pid;
                idSet.insert(pid);
            }
        }
    }

    return std::vector<int32_t>(idSet.begin(), idSet.end());
}


// queryIdPixel
// ------------
//! Return the Id of a pixel in the selection buffer.

int32_t
Ns3DSelectionManager::queryIdPixel(const int x, const int y)
{
    if (!_hasIdImage && 0 != _idFbo) {

//        GLenum error = glGetError();
//        qDebug() << (error == GL_NO_ERROR);

        //glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

        _idImage = _idFbo->toImage();
        //_idImage.save("./id.png", "PNG", -1);   // TMP!!
        _hasIdImage = true;
    }

    int32_t pid(0);

    if (0 <= x && x < _idImage.width() &&
        0 <= y && y < _idImage.height()) {
        const QRgb pval(_idImage.pixel(x, _idImage.height() - 1 - y));
        Ns3DSelectionManager::rgbaToId(
            Ngl::vec4uc(static_cast<GLubyte>(qRed(pval)),
                        static_cast<GLubyte>(qGreen(pval)),
                        static_cast<GLubyte>(qBlue(pval)),
                        static_cast<GLubyte>(qAlpha(pval))),
            &pid);
        qDebug() << "queryIdPixel: " << pid
                 << "(" << qRed(pval) << qGreen(pval) << qBlue(pval) << ")";
    }

    return pid;
}
