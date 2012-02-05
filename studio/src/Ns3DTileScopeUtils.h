// -----------------------------------------------------------------------------
//
// Ns3DTileScopeUtils.h
//
// Utilities & Tools for tile scopes.
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

#ifndef NS3D_TILE_SCOPE_UTILS_H
#define NS3D_TILE_SCOPE_UTILS_H

#include "Ns3DScopeUtils.h"

//inline void
//addTileLines(std::vector<em::vec<3,GLfloat> >& buf,
//             const em::vec<3,GLfloat>&         tmin,
//             const em::vec<3,GLfloat>&         tmax)
//{
//    // top loop
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmin[1],tmin[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmin[1],tmin[2]));
    
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmin[1],tmin[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmin[1],tmax[2]));
    
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmin[1],tmax[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmin[1],tmax[2]));
    
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmin[1],tmax[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmin[1],tmin[2]));
    
//    // bottom loop
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmax[1],tmin[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmax[1],tmin[2]));
    
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmax[1],tmin[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmax[1],tmax[2]));
    
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmax[1],tmax[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmax[1],tmax[2]));
    
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmax[1],tmax[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmax[1],tmin[2]));
    
//    // "vertical" lines
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmin[1],tmin[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmax[1],tmin[2]));
    
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmin[1],tmin[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmax[1],tmin[2]));
    
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmin[1],tmax[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmax[0],tmax[1],tmax[2]));
    
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmin[1],tmax[2]));
//    buf.push_back(em::vec<3,GLfloat>(tmin[0],tmax[1],tmax[2]));
//}

// -----------------------------------------------------------------------------

//inline void
//drawTiles(const NtString&     clientName,
//          const bool          prmCoarseTiles,
//          const NtString&     fineTilesVisibility,
//          const int           startFineTile,
//          const int           endFineTile,
//          const bool          prmCells,
//          Ns3DResourceObject* robject,
//          Ngl::ShaderProgram* shader,
//          const Nb::Value1f*  pixelRadiusParam,
//          const Nb::Value3f*  fineTileColorParam,
//          const Nb::Value3f*  coarseTileColorParam,
//          const Nb::Value3f*  cellColorParam,
//          const Nb::Value1e*  cellPlaneParam)
//{
//    // early exit
//    if (!prmCoarseTiles && fineTilesVisibility=="None" && !prmCells)
//        return;
    
//    if(robject==0)
//        return;

//    const Nb::TileLayout* layout(robject->constLayoutPtr());
//    if(layout==0)
//        return; // no layout, nothing to draw...
    
//    glLineWidth(pixelRadiusParam->eval(Nb::ZeroTimeBundle));
    
//    const Ngl::ShaderAttrib* posAttrib=shader->queryConstAttrib("position");

//    GLfloat proj[16];
//    glGetFloatv(GL_PROJECTION_MATRIX, proj);

//    GLfloat mv[16];
//    glGetFloatv(GL_MODELVIEW_MATRIX, mv);

//    if (prmCoarseTiles) {

//        const Ngl::VertexBuffer* vtxBuf(
//            robject->queryConstVertexBuffer("SHARED","Tile/coarse")
//            );

//        if (0 == vtxBuf) {

//            // add the coarse tiles
//            std::vector<em::vec<3,GLfloat> > buf;
//            buf.reserve(2*12*layout->coarseTileCount());
//            for(int t(0); t < layout->coarseTileCount(); ++t) {
//                em::vec3f tmin;
//                em::vec3f tmax;
//                Nb::Tile coarseTile(layout->coarseTile(t));
//                coarseTile.bounds(tmin, tmax);
//                addTileLines(buf, tmin, tmax);
//            }

//            vtxBuf =
//                robject->createVertexBuffer(
//                    "SHARED",
//                    "Tile/coarse",
//                    sizeof(em::vec<3,GLfloat>)*buf.size(),
//                    &buf[0]
//                    );
//        }

//        const GLsizei minCount(
//            Ngl::VertexAttrib::connect(*posAttrib, *vtxBuf)
//            );

//        const Nb::Vec4f rgb(
//            coarseTileColorParam->eval(Nb::ZeroTimeBundle,0),
//            coarseTileColorParam->eval(Nb::ZeroTimeBundle,1),
//            coarseTileColorParam->eval(Nb::ZeroTimeBundle,2),
//            1.f
//            );

//        shader->use();
//        shader->storeUniform4f("lineColor", rgb);
//        shader->storeUniform4m("projection", proj);
//        shader->storeUniform4m("modelview", mv);
//        shader->uploadUniforms(Nb::ZeroTimeBundle);

//        glDrawArrays(GL_LINES, 0, minCount);

//        shader->unuse();
//        Ngl::VertexAttrib::disconnect(*posAttrib);
//    }

//    if (fineTilesVisibility!="None") {

//        const int tile0=
//            fineTilesVisibility=="All" ?
//            0 :
//            std::max(0,startFineTile);

//        const int tile1=
//            fineTilesVisibility=="All" ?
//            layout->fineTileCount() :
//            std::min(layout->fineTileCount(),endFineTile+1);
        
//        Ngl::VertexBuffer* vtxBuf(
//            robject->queryMutableVertexBuffer(
//                "SHARED","Tile/fine"
//                )
//            );

//        // rebuild tile vertex buffer if range changed...

//        if(vtxBuf) {
//            if(vtxBuf->metaData1i("tile0")!=tile0 ||
//               vtxBuf->metaData1i("tile1")!=tile1) {
//                robject->destroyVertexBuffer("SHARED","Tile/fine");
//                vtxBuf=0;
//            }
//        }

//        if (0 == vtxBuf) {

//            // add the fine tiles
//            std::vector<em::vec<3,GLfloat> > buf;
//            const int tileCount=(tile1-tile0);
//            buf.reserve(2*12*tileCount);
//            for(int t(tile0); t<tile1; ++t) {
//                em::vec3f tmin;
//                em::vec3f tmax;
//                Nb::Tile fineTile(layout->fineTile(t));
//                fineTile.bounds(tmin, tmax);
//                addTileLines(buf,tmin,tmax);
//            }

//            vtxBuf =
//                robject->createVertexBuffer(
//                    "SHARED",
//                    "Tile/fine",
//                    sizeof(em::vec<3,GLfloat>)*buf.size(),
//                    &buf[0]);
//            vtxBuf->attachMetaData1i("tile0",tile0);
//            vtxBuf->attachMetaData1i("tile1",tile1);
//        }

//        const GLsizei minCount(
//            Ngl::VertexAttrib::connect(*posAttrib, *vtxBuf));

//        const Nb::Vec4f rgb(
//            fineTileColorParam->eval(Nb::ZeroTimeBundle,0),
//            fineTileColorParam->eval(Nb::ZeroTimeBundle,1),
//            fineTileColorParam->eval(Nb::ZeroTimeBundle,2),
//            1.f
//            );

//        shader->use();
//        shader->storeUniform4f("lineColor", rgb);
//        shader->storeUniform4m("projection", proj);
//        shader->storeUniform4m("modelview", mv);
//        shader->uploadUniforms(Nb::ZeroTimeBundle);

//        glDrawArrays(GL_LINES, 0, minCount);

//        shader->unuse();
//        Ngl::VertexAttrib::disconnect(*posAttrib);
//    }

//    if (prmCells) {

//        const Nb::Vec4f rgb(
//            cellColorParam->eval(Nb::ZeroTimeBundle,0),
//            cellColorParam->eval(Nb::ZeroTimeBundle,1),
//            cellColorParam->eval(Nb::ZeroTimeBundle,2),
//            1.f
//            );

//        em::vec3f tmin, tmax;
//        layout->allTileBounds(tmin, tmax);

//        // assume same cell count in every dimension
//        const float cellSize(layout->cellSize());
//        const int cellCount[] = {
//            ((tmax[0] - tmin[0])/cellSize),
//            ((tmax[1] - tmin[1])/cellSize),
//            ((tmax[2] - tmin[2])/cellSize),
//        };

//        const std::string planeStr(cellPlaneParam->eval(Nb::ZeroTimeBundle));

//        if ("XY" == planeStr) {

//            const Ngl::VertexBuffer* vtxBuf(
//                robject->queryConstVertexBuffer(
//                    "SHARED","Tile/cell-xy"
//                    )
//                );

//            if (0 == vtxBuf) {
//                std::vector<em::vec<3,GLfloat> > buf;

//                for(int c(0); c <= cellCount[1]; ++c) {
//                    // "horizontal"
//                    buf.push_back(
//                        em::vec<3,GLfloat>(tmin[0],
//                                           tmin[1] + c*cellSize,
//                                           tmin[2]));
//                    buf.push_back(
//                        em::vec<3,GLfloat>(tmax[0],
//                                           tmin[1] + c*cellSize,
//                                           tmin[2]));
//                }

//                for(int c(0); c <= cellCount[0]; ++c) {
//                    // "vertical"
//                    buf.push_back(
//                        em::vec3f(tmin[0] + c*cellSize,
//                                  tmin[1],
//                                  tmin[2]));
//                    buf.push_back(
//                        em::vec3f(tmin[0] + c*cellSize,
//                                  tmax[1],
//                                  tmin[2]));
//                }

//                vtxBuf =
//                    robject->createVertexBuffer(
//                        "SHARED",
//                        "Tile/cell-xy",
//                        sizeof(em::vec<3,GLfloat>)*buf.size(),
//                        &buf[0]
//                        );
//            }

//            const GLsizei minCount(
//                Ngl::VertexAttrib::connect(*posAttrib, *vtxBuf));

//            shader->use();
//            shader->storeUniform4f("lineColor", rgb);
//            shader->storeUniform4m("projection", proj);
//            shader->storeUniform4m("modelview", mv);
//            shader->uploadUniforms(Nb::ZeroTimeBundle);

//            glDrawArrays(GL_LINES, 0, minCount);

//            shader->unuse();
//            Ngl::VertexAttrib::disconnect(*posAttrib);
//        }
//        else if ("XZ" == planeStr) {

//            const Ngl::VertexBuffer* vtxBuf(
//                robject->queryConstVertexBuffer(
//                    "SHARED","Tile/cell-xz"
//                    )
//                );

//            if (0 == vtxBuf) {
//                std::vector<em::vec<3,GLfloat> > buf;

//                for(int c(0); c <= cellCount[2]; ++c) {
//                    // "horizontal"
//                    buf.push_back(
//                        em::vec<3,GLfloat>(tmin[0],
//                                           tmin[1],
//                                           tmin[2] + c*cellSize));
//                    buf.push_back(
//                        em::vec<3,GLfloat>(tmax[0],
//                                           tmin[1],
//                                           tmin[2] + c*cellSize));
//                }

//                for(int c(0); c <= cellCount[0]; ++c) {
//                    // "vertical"
//                    buf.push_back(
//                        em::vec<3,GLfloat>(tmin[0] + c*cellSize,
//                                           tmin[1],
//                                           tmin[2]));
//                    buf.push_back(
//                        em::vec<3,GLfloat>(tmin[0] + c*cellSize,
//                                           tmin[1],
//                                           tmax[2]));
//                }

//                vtxBuf =
//                    robject->createVertexBuffer(
//                        "SHARED",
//                        "Tile/cell-xz",
//                        sizeof(em::vec<3,GLfloat>)*buf.size(),
//                        &buf[0]
//                        );
//            }

//            const GLsizei minCount(
//                Ngl::VertexAttrib::connect(*posAttrib, *vtxBuf));

//            shader->use();
//            shader->storeUniform4f("lineColor", rgb);
//            shader->storeUniform4m("projection", proj);
//            shader->storeUniform4m("modelview", mv);
//            shader->uploadUniforms(Nb::ZeroTimeBundle);

//            glDrawArrays(GL_LINES, 0, minCount);

//            shader->unuse();
//            Ngl::VertexAttrib::disconnect(*posAttrib);
//        }
//        else if ("YZ" == planeStr) {

//            const Ngl::VertexBuffer* vtxBuf(
//                robject->queryConstVertexBuffer(
//                    "SHARED","Tile/cell-yz"
//                    )
//                );

//            if (0 == vtxBuf) {
//                std::vector<em::vec<3,GLfloat> > buf;

//                for(int c(0); c <= cellCount[2]; ++c) {
//                    // "horizontal"
//                    buf.push_back(
//                        em::vec<3,GLfloat>(tmin[0],
//                                           tmin[1],
//                                           tmin[2] + c*cellSize));
//                    buf.push_back(
//                        em::vec<3,GLfloat>(tmin[0],
//                                           tmax[1],
//                                           tmin[2] + c*cellSize));
//                }

//                for(int c(0); c <= cellCount[1]; ++c) {
//                    // "vertical"
//                    buf.push_back(
//                        em::vec<3,GLfloat>(tmin[0],
//                                           tmin[1] + c*cellSize,
//                                           tmin[2]));
//                    buf.push_back(
//                        em::vec<3,GLfloat>(tmin[0],
//                                           tmin[1] + c*cellSize,
//                                           tmax[2]));
//                }

//                vtxBuf =
//                    robject->createVertexBuffer(
//                        "SHARED",
//                        "Tile/cell-yz",
//                        sizeof(em::vec<3,GLfloat>)*buf.size(),
//                        &buf[0]
//                        );
//            }

//            const GLsizei minCount(
//                Ngl::VertexAttrib::connect(*posAttrib, *vtxBuf));

//            shader->use();
//            shader->storeUniform4f("lineColor", rgb);
//            shader->storeUniform4m("projection", proj);
//            shader->storeUniform4m("modelview", mv);
//            shader->uploadUniforms(Nb::ZeroTimeBundle);

//            glDrawArrays(GL_LINES, 0, minCount);

//            shader->unuse();
//            Ngl::VertexAttrib::disconnect(*posAttrib);
//        }
//    }
//}

// -----------------------------------------------------------------------------

#endif // NS3D_TILE_SCOPE_UTILS_H
