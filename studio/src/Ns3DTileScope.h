// -----------------------------------------------------------------------------
//
// Ns3DTileScope.h
//
// Naiad Studio Tilesurface scope - renders a tile layout.
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

#include <Ni.h>
#include <NgBodyOp.h>
#include <NgInput.h>

#include <NbBody.h>
#include <NbTileLayout.h>
#include <NbTile.h>

#include <NglVertexBuffer.h>
#include <NglVertexAttrib.h>
#include <NglShaderProgram.h>

#include "Ns3DCameraScope.h"
#include "Ns3DTileScopeUtils.h"

#include <map>
#include <set>

// -----------------------------------------------------------------------------

class Ns3DTileScope : public Ns3DBodyScope
{
public:
    Ns3DTileScope()
        : Ns3DBodyScope()
        , _surfaceShader(0)
        , _lineShader(0)
    {
    }
    
    virtual
    ~Ns3DTileScope()
    {
        // Free shader resources.

        delete _surfaceShader;
        delete _lineShader;
    }

    virtual void
    reset()
    {
        _surfaceShader = new Ngl::ShaderProgram(
            NtString(shaderPath() + "tile-surface.vs"),
            NtString(shaderPath() + "tile-surface.fs"));
        _lineShader = new Ngl::ShaderProgram(
            NtString(shaderPath() + "tile-lines.vs"),
            NtString(shaderPath() + "tile-lines.fs"));
    }

    virtual NtString
    typeName() const
    { return "Tile-Scope"; }

    virtual bool
    draw(NsBodyObject          *nsBody,
         const Ns3DCameraScope *cam,
         const Ngl::Viewport   &vp)
    {
        if (0 == cam || 0 == nsBody->ns3DBody()) {
            return true; // Early exit!
        }

        _drawTiles(
            name(),
            queryCurrentVisibleFrameTimeBundle(),
            *cam,
            *nsBody->ns3DBody());

        ssHud << "Body: '" << fromQStr(nsBody->name()) << "\n";

        return true;
    }

private:    // _Vec3<T>

    template<typename T>
    class _Vec3
    {
    public:

        //! CTOR.
        explicit
        _Vec3(const T v0, const T v1, const T v2)
        {
            _v[0] = v0;
            _v[1] = v1;
            _v[2] = v2;
        }

        //! Copy CTOR.
        _Vec3(const _Vec3<T> &rhs)
        {
            _v[0] = rhs._v[0];
            _v[1] = rhs._v[1];
            _v[2] = rhs._v[2];
        }

        //! Assign.
        _Vec3<T>&
        operator=(const _Vec3<T> &rhs)
        {
            _v[0] = rhs._v[0];
            _v[1] = rhs._v[1];
            _v[2] = rhs._v[2];
            return *this;
        }

        T&
        operator[](const int i)
        { return _v[i]; }

        const T&
        operator[](const int i) const
        { return _v[i]; }

        bool
        operator<(const _Vec3<T> &rhs) const
        {
            const bool eq0(_v[0] == rhs._v[0]);

            return (_v[0] < rhs._v[0]) ||
                   (eq0 && (_v[1] <  rhs._v[1])) ||
                   (eq0 && (_v[1] == rhs._v[1]) && (_v[2] < rhs._v[2]));
        }

    public:     // Member variables.

        T _v[3];
    };

    typedef _Vec3<int>   _Vec3i;
    typedef _Vec3<float> _Vec3f;

private:    // _Vtx

    class _Vtx
    {
    public:

        //! CTOR.
        explicit
        _Vtx(const _Vec3i &tsx, const _Vec3f &wsx)
            : _tsx(tsx), _wsx(wsx)
        {}

        int i() const { return _tsx[0]; }
        int j() const { return _tsx[1]; }
        int k() const { return _tsx[2]; }

        float x() const { return _wsx[0]; }
        float y() const { return _wsx[1]; }
        float z() const { return _wsx[2]; }

        const _Vec3i&
        tsx() const
        { return _tsx; }

        const _Vec3f&
        wsx() const
        { return _wsx; }

        bool
        operator<(const _Vtx &rhs) const
        { return _tsx < rhs._tsx; }

    private:    // Member variables.

        _Vec3i _tsx;   //!< Tile space.
        _Vec3f _wsx;   //!< World space.
    };

private:    // _Line.

    class _Line
    {
    public:

        //! CTOR.
        explicit
        _Line(const GLuint v0, const GLuint v1)
        {
            _v[0] = v0;
            _v[1] = v1;
        }

        //! Copy CTOR.
        _Line(const _Line &rhs)
        {
            _v[0] = rhs._v[0];
            _v[1] = rhs._v[1];
        }

        //! Assign.
        _Line&
        operator=(const _Line &rhs)
        {
            _v[0] = rhs._v[0];
            _v[1] = rhs._v[1];
            return *this;
        }

        bool
        operator<(const _Line &rhs) const
        {
            const GLuint sv0[] = {
                qMin(_v[0], _v[1]), qMax(_v[0], _v[1])
            };

            const GLuint sv1[] = {
                qMin(rhs._v[0], rhs._v[1]), qMax(rhs._v[0], rhs._v[1])
            };

            return (sv0[0] < sv1[0]) ||
                   ((sv0[0] == sv1[0]) && (sv0[1] < sv1[1]));
        }

        //! No bounds checking!
        const GLuint&
        operator[](const int i) const
        { return _v[i]; }

        //! No bounds checking!
        GLuint&
        operator[](const int i)
        { return _v[i]; }

    private:

        GLuint _v[2];   //!< Indices.
    };

private:    // _Quad.

    class _Quad
    {
    public:

        //! CTOR.
        explicit
        _Quad(const GLuint v0,const GLuint v1,const GLuint v2,const GLuint v3)
        {
            _v[0] = v0;
            _v[1] = v1;
            _v[2] = v2;
            _v[3] = v3;
        }

        //! Copy CTOR.
        _Quad(const _Quad &rhs)
        {
            _v[0] = rhs._v[0];
            _v[1] = rhs._v[1];
            _v[2] = rhs._v[2];
            _v[3] = rhs._v[3];
        }

        //! Assign.
        _Quad&
        operator=(const _Quad &rhs)
        {
            _v[0] = rhs._v[0];
            _v[1] = rhs._v[1];
            _v[2] = rhs._v[2];
            _v[3] = rhs._v[3];
            return *this;
        }

        bool
        operator<(const _Quad &rhs) const
        {
            GLuint sv0[] = {     _v[0],     _v[1],     _v[2],     _v[3] };
            GLuint sv1[] = { rhs._v[0], rhs._v[1], rhs._v[2], rhs._v[3] };
            std::sort(sv0, sv0 + 4);
            std::sort(sv1, sv1 + 4);
            const bool v0_eq(sv0[0] == sv1[0]);
            const bool v1_eq(sv0[1] == sv1[1]);
            return (sv0[0] < sv1[0]) ||
                   (v0_eq && (sv0[1] < sv1[1])) ||
                   (v0_eq && v1_eq && (sv0[2] <  sv1[2])) ||
                   (v0_eq && v1_eq && (sv0[2] == sv1[2]) && (sv0[3]<sv1[3]));
        }

        //! No bounds checking!
        const GLuint&
        operator[](const int i) const
        { return _v[i]; }

        //! No bounds checking!
        GLuint&
        operator[](const int i)
        { return _v[i]; }

    private:    // Member variables.

        GLuint _v[4];   //!< Indices.
    };

private:    // _Cube.

    class _Cube
    {
    public:

        //! CTOR.
        explicit
        _Cube(const _Vtx &vtx0, const _Vtx &vtx1)
            : _vtx0(vtx0)
            , _vtx1(vtx1)
        {}

        int i0() const { return _vtx0.i(); }
        int j0() const { return _vtx0.j(); }
        int k0() const { return _vtx0.k(); }
        int i1() const { return _vtx1.i(); }
        int j1() const { return _vtx1.j(); }
        int k1() const { return _vtx1.k(); }

        float x0() const { return _vtx0.x(); }
        float y0() const { return _vtx0.y(); }
        float z0() const { return _vtx0.z(); }
        float x1() const { return _vtx1.x(); }
        float y1() const { return _vtx1.y(); }
        float z1() const { return _vtx1.z(); }

    private:    // Member variables.

        _Vtx _vtx0;
        _Vtx _vtx1;
    };

private:

    typedef std::map<_Vec3i, GLuint> _TsxMapType;
    typedef std::vector<_Vec3f>      _WsxVecType;
    typedef std::map<_Line, int>     _LineMapType;
    typedef std::map<_Quad, int>     _QuadMapType;


    void
    _drawTiles(const NtString        &clientName,
               const Nb::TimeBundle  &cvftb,
               const Ns3DCameraScope &cam,
               Ns3DResourceObject    &robject)
    {
        const Nb::TileLayout *layout(robject.constLayoutPtr());
        if (0 == layout) {
            return; // No layout, nothing to draw...
        }

        const NtString ftVisibility(
            param1e("Fine Tiles Visibility")->eval(cvftb));

        const bool drawCoarse("On" == param1e("Coarse Tiles")->eval(cvftb));
        const bool drawFine(!("None" == ftVisibility));
        const bool drawCells("On" == param1e("Cells")->eval(cvftb));

        if (!drawCoarse && !drawFine && !drawCells) {
            return; // Early exit!
        }

        // Cells.

        if (drawCells) {
            _drawCells(*layout, cvftb, cam, robject);
        }

        // Coarse tiles.

        if (drawCoarse) {
            _drawCoarseTiles(*layout, cvftb, cam, robject);
        }

        // Fine tiles.

        if (drawFine) {
            _drawFineTiles(ftVisibility, *layout, cvftb, cam, robject);
        }
    }

    void
    _drawCoarseTiles(const Nb::TileLayout  &layout,
                     const Nb::TimeBundle  &cvftb,
                     const Ns3DCameraScope &cam,
                     Ns3DResourceObject    &robject)
    {
        // Surface.

        if ("On" == param1e("Coarse Surface")->eval(cvftb)) {
            static const NtString posVboName("Tile/coarse-surface");
            const Ngl::VertexBuffer* posVbo(
                robject.queryConstVertexBuffer("SHARED", posVboName));

            if (0 == posVbo) {
                std::vector<_Cube> cubes;
                _extractCoarseCubes(layout, cubes);

                _WsxVecType  wsxVec;
                _LineMapType lineMap;
                _QuadMapType quadMap;
                _addCubes(cubes, wsxVec, lineMap, quadMap);

                std::vector<_Vec3f> surfaceVtx;
                _surfaceVertices(quadMap,wsxVec, surfaceVtx);

                posVbo =
                    robject.createVertexBuffer(
                        "SHARED",
                        posVboName,
                        sizeof(_Vec3f)*surfaceVtx.size(),
                        &surfaceVtx[0]);
            }

            const Nb::Value3f *ambientParam(
                param3f(("Coarse Ambient Color")));
            const Ngl::vec4f matAmbient(
                ambientParam->eval(cvftb, 0),
                ambientParam->eval(cvftb, 1),
                ambientParam->eval(cvftb, 2),
                param1f("Coarse Ambient Alpha")->eval(cvftb));

            const Nb::Value3f *diffuseParam(
                param3f("Coarse Diffuse Color"));
            const Ngl::vec4f matDiffuse(
                diffuseParam->eval(cvftb, 0),
                diffuseParam->eval(cvftb, 1),
                diffuseParam->eval(cvftb, 2),
                param1f("Coarse Diffuse Alpha")->eval(cvftb));

            const Nb::Value3f *specularParam(
                param3f("Coarse Specular Color"));
            const Ngl::vec4f matSpecular(
                specularParam->eval(cvftb, 0),
                specularParam->eval(cvftb, 1),
                specularParam->eval(cvftb, 2),
                param1f("Coarse Specular Alpha")->eval(cvftb));

            const NtString cullMode(param1e("Coarse Culling")->eval(cvftb));
            const bool culling(!("None" == cullMode));

            GLenum cullFace(GL_BACK);
            if (culling && "Front" == cullMode) {
                cullFace = GL_FRONT;
            }

            _drawSurface(*posVbo,
                         cam.modelviewMat(),
                         cam.projectionMat(),
                         matAmbient,
                         matDiffuse,
                         matSpecular,
                         cvftb,
                         "On" == param1e("Coarse Blend")->eval(cvftb),
                         culling,
                         cullFace);
        }

        // Surface lines.

        if ("On" == param1e("Coarse Surface Lines")->eval(cvftb)) {
            static const NtString posVboName("Tile/coarse-surface-lines");
            const Ngl::VertexBuffer* posVbo(
                robject.queryConstVertexBuffer("SHARED", posVboName));

            if (0 == posVbo) {
                std::vector<_Cube> cubes;
                _extractCoarseCubes(layout, cubes);

                _WsxVecType  wsxVec;
                _LineMapType lineMap;
                _QuadMapType quadMap;
                _addCubes(cubes, wsxVec, lineMap, quadMap);

                std::vector<_Vec3f> surfaceLineVtx;
                _surfaceLineVertices(lineMap, wsxVec, surfaceLineVtx);

                posVbo =
                    robject.createVertexBuffer(
                        "SHARED",
                        posVboName,
                        sizeof(_Vec3f)*surfaceLineVtx.size(),
                        &surfaceLineVtx[0]);
            }

            const Nb::Value3f *lineColorParam(
                param3f("Coarse Surface Line Color"));

            const Ngl::vec4f lineColor(
                lineColorParam->eval(cvftb, 0),
                lineColorParam->eval(cvftb, 1),
                lineColorParam->eval(cvftb, 2),
                param1f("Coarse Surface Line Alpha")->eval(cvftb));

            _drawLines(*posVbo,
                       param1f("Pixel Radius")->eval(cvftb),
                       cam.modelviewMat(),
                       cam.projectionMat(),
                       lineColor,
                       cvftb,
                       true);
        }

        // Internal lines.

        if ("On" == param1e("Coarse Internal Lines")->eval(cvftb)) {
            static const NtString posVboName("Tile/coarse-internal-lines");
            const Ngl::VertexBuffer* posVbo(
                robject.queryConstVertexBuffer("SHARED", posVboName));

            if (0 == posVbo) {
                std::vector<_Cube> cubes;
                _extractCoarseCubes(layout, cubes);

                _WsxVecType  wsxVec;
                _LineMapType lineMap;
                _QuadMapType quadMap;
                _addCubes(cubes, wsxVec, lineMap, quadMap);

                std::vector<_Vec3f> internalLineVtx;
                _internalLineVertices(lineMap, wsxVec, internalLineVtx);

                posVbo =
                    robject.createVertexBuffer(
                        "SHARED",
                        posVboName,
                        sizeof(_Vec3f)*internalLineVtx.size(),
                        &internalLineVtx[0]);
            }

            const Nb::Value3f *lineColorParam(
                param3f("Coarse Internal Line Color"));

            const Ngl::vec4f lineColor(
                lineColorParam->eval(cvftb, 0),
                lineColorParam->eval(cvftb, 1),
                lineColorParam->eval(cvftb, 2),
                param1f("Coarse Internal Line Alpha")->eval(cvftb));

            _drawLines(*posVbo,
                       param1f("Pixel Radius")->eval(cvftb),
                       cam.modelviewMat(),
                       cam.projectionMat(),
                       lineColor,
                       cvftb,
                       true);
        }
    }

    void
    _drawFineTiles(const NtString        &ftVisibility,
                   const Nb::TileLayout  &layout,
                   const Nb::TimeBundle  &cvftb,
                   const Ns3DCameraScope &cam,
                   Ns3DResourceObject    &robject)
    {
        const int ftStart(param1i("Fine Tile Range Start")->eval(cvftb));
        const int ftEnd(param1i("Fine Tile Range End")->eval(cvftb));

        const bool ftAll("All" == ftVisibility);
        const int ftCount(layout.fineTileCount());

        const int tile0(ftAll ? 0 : qMax(0, ftStart));
        const int tile1(ftAll ? ftCount : qMin(ftCount, ftEnd + 1));

        // Surface.

        if ("On" == param1e("Fine Surface")->eval(cvftb)) {
            static const NtString posVboName("Tile/fine-surface");
            Ngl::VertexBuffer* posVbo(
                robject.queryMutableVertexBuffer("SHARED", posVboName));

            // Rebuild tile VBO's if range changed.

            if (0 != posVbo) {
                if (tile0 != posVbo->metaData1i("tile0") ||
                    tile1 != posVbo->metaData1i("tile1")) {
                    robject.destroyVertexBuffer("SHARED", posVboName);
                    posVbo = 0; // Rebuild below.
                }
            }

            if (0 == posVbo) {
                std::vector<_Cube> cubes;
                _extractFineCubes(layout, tile0, tile1, cubes);

                _WsxVecType  wsxVec;
                _LineMapType lineMap;
                _QuadMapType quadMap;
                _addCubes(cubes, wsxVec, lineMap, quadMap);

                std::vector<_Vec3f> surfaceVtx;
                _surfaceVertices(quadMap, wsxVec, surfaceVtx);

                posVbo =
                    robject.createVertexBuffer(
                        "SHARED",
                        posVboName,
                        sizeof(_Vec3f)*surfaceVtx.size(),
                        &surfaceVtx[0]);
                posVbo->attachMetaData1i("tile0", tile0);
                posVbo->attachMetaData1i("tile1", tile1);
            }

            const Nb::Value3f *ambientParam(
                param3f(("Fine Ambient Color")));
            const Ngl::vec4f matAmbient(
                ambientParam->eval(cvftb, 0),
                ambientParam->eval(cvftb, 1),
                ambientParam->eval(cvftb, 2),
                param1f("Fine Ambient Alpha")->eval(cvftb));

            const Nb::Value3f *diffuseParam(
                param3f("Fine Diffuse Color"));
            const Ngl::vec4f matDiffuse(
                diffuseParam->eval(cvftb, 0),
                diffuseParam->eval(cvftb, 1),
                diffuseParam->eval(cvftb, 2),
                param1f("Fine Diffuse Alpha")->eval(cvftb));

            const Nb::Value3f *specularParam(
                param3f("Fine Specular Color"));
            const Ngl::vec4f matSpecular(
                specularParam->eval(cvftb, 0),
                specularParam->eval(cvftb, 1),
                specularParam->eval(cvftb, 2),
                param1f("Fine Specular Alpha")->eval(cvftb));

            const NtString cullMode(param1e("Fine Culling")->eval(cvftb));
            const bool culling(!("None" == cullMode));

            GLenum cullFace(GL_BACK);
            if (culling && "Front" == cullMode) {
                cullFace = GL_FRONT;
            }

            _drawSurface(*posVbo,
                         cam.modelviewMat(),
                         cam.projectionMat(),
                         matAmbient,
                         matDiffuse,
                         matSpecular,
                         cvftb,
                         "On" == param1e("Fine Blend")->eval(cvftb),
                         culling,
                         cullFace);
        }

        // Surface lines.

        if ("On" == param1e("Fine Surface Lines")->eval(cvftb)) {
            static const NtString posVboName("Tile/fine-surface-lines");
            Ngl::VertexBuffer* posVbo(
                robject.queryMutableVertexBuffer("SHARED", posVboName));

            if (0 != posVbo) {
                if (tile0 != posVbo->metaData1i("tile0") ||
                    tile1 != posVbo->metaData1i("tile1")) {
                    robject.destroyVertexBuffer("SHARED", posVboName);
                    posVbo = 0; // Rebuild below.
                }
            }

            if (0 == posVbo) {
                std::vector<_Cube> cubes;
                _extractFineCubes(layout, tile0, tile1, cubes);

                _WsxVecType  wsxVec;
                _LineMapType lineMap;
                _QuadMapType quadMap;
                _addCubes(cubes, wsxVec, lineMap, quadMap);

                std::vector<_Vec3f> surfaceLineVtx;
                _surfaceLineVertices(lineMap, wsxVec, surfaceLineVtx);

                posVbo =
                    robject.createVertexBuffer(
                        "SHARED",
                        posVboName,
                        sizeof(_Vec3f)*surfaceLineVtx.size(),
                        &surfaceLineVtx[0]);
                posVbo->attachMetaData1i("tile0", tile0);
                posVbo->attachMetaData1i("tile1", tile1);
            }

            const Nb::Value3f *lineColorParam(
                param3f("Fine Surface Line Color"));

            const Ngl::vec4f lineColor(
                lineColorParam->eval(cvftb, 0),
                lineColorParam->eval(cvftb, 1),
                lineColorParam->eval(cvftb, 2),
                param1f("Fine Surface Line Alpha")->eval(cvftb));

            _drawLines(*posVbo,
                       param1f("Pixel Radius")->eval(cvftb),
                       cam.modelviewMat(),
                       cam.projectionMat(),
                       lineColor,
                       cvftb,
                       "On" == param1e("Fine Blend")->eval(cvftb));
        }

        // Internal lines.

        if ("On" == param1e("Fine Internal Lines")->eval(cvftb)) {
            static const NtString posVboName("Tile/fine-internal-lines");
            Ngl::VertexBuffer* posVbo(
                robject.queryMutableVertexBuffer("SHARED", posVboName));

            if (0 != posVbo) {
                if (tile0 != posVbo->metaData1i("tile0") ||
                    tile1 != posVbo->metaData1i("tile1")) {
                    robject.destroyVertexBuffer("SHARED", posVboName);
                    posVbo = 0; // Rebuild below.
                }
            }

            if (0 == posVbo) {
                std::vector<_Cube> cubes;
                _extractFineCubes(layout, tile0, tile1, cubes);

                _WsxVecType  wsxVec;
                _LineMapType lineMap;
                _QuadMapType quadMap;
                _addCubes(cubes, wsxVec, lineMap, quadMap);

                std::vector<_Vec3f> internalLineVtx;
                _internalLineVertices(lineMap, wsxVec, internalLineVtx);

                posVbo =
                    robject.createVertexBuffer(
                        "SHARED",
                        posVboName,
                        sizeof(_Vec3f)*internalLineVtx.size(),
                        &internalLineVtx[0]);
                posVbo->attachMetaData1i("tile0", tile0);
                posVbo->attachMetaData1i("tile1", tile1);
            }

            const Nb::Value3f *lineColorParam(
                param3f("Fine Internal Line Color"));

            const Ngl::vec4f lineColor(
                lineColorParam->eval(cvftb, 0),
                lineColorParam->eval(cvftb, 1),
                lineColorParam->eval(cvftb, 2),
                param1f("Fine Internal Line Alpha")->eval(cvftb));

            _drawLines(*posVbo,
                       param1f("Pixel Radius")->eval(cvftb),
                       cam.modelviewMat(),
                       cam.projectionMat(),
                       lineColor,
                       cvftb,
                       "On" == param1e("Fine Blend")->eval(cvftb));
        }
    }

    void
    _drawCells(const Nb::TileLayout  &layout,
               const Nb::TimeBundle  &cvftb,
               const Ns3DCameraScope &cam,
               Ns3DResourceObject    &robject)
    {
        const Nb::Value3f *cellColorParam(param3f("Cell Color"));
        const Nb::Vec4f lineColor(
            cellColorParam->eval(cvftb, 0),
            cellColorParam->eval(cvftb, 1),
            cellColorParam->eval(cvftb, 2),
            1.f);

        em::vec3f tmin, tmax;
        layout.allTileBounds(tmin, tmax);

        // Assume same cell count in every dimension.

        const float cellSize(layout.cellSize());
        const int cellCount[] = {
            ((tmax[0] - tmin[0])/cellSize),
            ((tmax[1] - tmin[1])/cellSize),
            ((tmax[2] - tmin[2])/cellSize)
        };

        const Nb::Value1e *cellPlaneParam(param1e("Cell Plane"));
        const NtString planeStr(cellPlaneParam->eval(cvftb));

        if ("XY" == planeStr) {
            static const NtString posVboName("Tile/cell-xy");
            const Ngl::VertexBuffer *posVbo(
                robject.queryConstVertexBuffer("SHARED", posVboName));

            if (0 == posVbo) {
                std::vector<_Vec3f> p;
                p.reserve(2*cellCount[1] + 2*cellCount[0]);
                for (int c(0); c <= cellCount[1]; ++c) {
                    // "horizontal"
                    p.push_back(_Vec3f(tmin[0],tmin[1]+c*cellSize,tmin[2]));
                    p.push_back(_Vec3f(tmax[0],tmin[1]+c*cellSize,tmin[2]));
                }

                for(int c(0); c <= cellCount[0]; ++c) {
                    // "vertical"
                    p.push_back(_Vec3f(tmin[0]+c*cellSize,tmin[1],tmin[2]));
                    p.push_back(_Vec3f(tmin[0]+c*cellSize,tmax[1],tmin[2]));
                }

                posVbo =
                    robject.createVertexBuffer(
                        "SHARED",
                        posVboName,
                        sizeof(em::vec<3,GLfloat>)*p.size(),
                        &p[0]);
            }

            _drawLines(*posVbo,
                       param1f("Pixel Radius")->eval(cvftb),
                       cam.modelviewMat(),
                       cam.projectionMat(),
                       lineColor,
                       cvftb,
                       false);
        }
        else if ("XZ" == planeStr) {
            static const NtString posVboName("Tile/cell-xz");
            const Ngl::VertexBuffer *posVbo(
                robject.queryConstVertexBuffer("SHARED", posVboName));

            if (0 == posVbo) {
                std::vector<_Vec3f> p;
                p.reserve(2*cellCount[2] + 2*cellCount[0]);
                for(int c(0); c <= cellCount[2]; ++c) {
                    // "horizontal"
                    p.push_back(_Vec3f(tmin[0],tmin[1],tmin[2]+c*cellSize));
                    p.push_back(_Vec3f(tmax[0],tmin[1],tmin[2]+c*cellSize));
                }

                for(int c(0); c <= cellCount[0]; ++c) {
                    // "vertical"
                    p.push_back(_Vec3f(tmin[0]+c*cellSize,tmin[1],tmin[2]));
                    p.push_back(_Vec3f(tmin[0]+c*cellSize,tmin[1],tmax[2]));
                }

                posVbo =
                    robject.createVertexBuffer(
                        "SHARED",
                        posVboName,
                        sizeof(em::vec<3,GLfloat>)*p.size(),
                        &p[0]);
            }

            _drawLines(*posVbo,
                       param1f("Pixel Radius")->eval(cvftb),
                       cam.modelviewMat(),
                       cam.projectionMat(),
                       lineColor,
                       cvftb,
                       false);
        }
        else if ("YZ" == planeStr) {
            static const NtString posVboName("Tile/cell-yz");
            const Ngl::VertexBuffer *posVbo(
                robject.queryConstVertexBuffer("SHARED", posVboName));

            if (0 == posVbo) {
                std::vector<_Vec3f> p;
                p.reserve(2*cellCount[2] + 2*cellCount[1]);
                for(int c(0); c <= cellCount[2]; ++c) {
                    // "horizontal"
                    p.push_back(_Vec3f(tmin[0],tmin[1],tmin[2]+c*cellSize));
                    p.push_back(_Vec3f(tmin[0],tmax[1],tmin[2]+c*cellSize));
                }

                for(int c(0); c <= cellCount[1]; ++c) {
                    // "vertical"
                    p.push_back(_Vec3f(tmin[0],tmin[1]+c*cellSize,tmin[2]));
                    p.push_back(_Vec3f(tmin[0],tmin[1]+c*cellSize,tmax[2]));
                }

                posVbo =
                    robject.createVertexBuffer(
                        "SHARED",
                        posVboName,
                        sizeof(em::vec<3,GLfloat>)*p.size(),
                        &p[0]);
            }

            _drawLines(*posVbo,
                       param1f("Pixel Radius")->eval(cvftb),
                       cam.modelviewMat(),
                       cam.projectionMat(),
                       lineColor,
                       cvftb,
                       false);
        }
    }

    void
    _drawSurface(const Ngl::VertexBuffer &posVbo,
                 const em::glmat44f      &modelview,
                 const em::glmat44f      &projection,
                 const Ngl::vec4f        &matAmbient,
                 const Ngl::vec4f        &matDiffuse,
                 const Ngl::vec4f        &matSpecular,
                 const NtTimeBundle      &cvftb,
                 const bool               blend,
                 const bool               culling,
                 const GLenum             cullFace)
    {
        const Ngl::ShaderAttrib* posAttrib(
            _surfaceShader->queryConstAttrib("position"));

        EM_ASSERT(0 != posAttrib);

        const GLsizei minCount(Ngl::VertexAttrib::connect(*posAttrib, posVbo));

        _surfaceShader->use();
        _surfaceShader->storeUniform4m("projection",   &projection[0][0]);
        _surfaceShader->storeUniform4m("modelview",    &modelview[0][0]);

        _surfaceShader->storeUniform4f("matAmbient",  &matAmbient[0]);
        _surfaceShader->storeUniform4f("matDiffuse",  &matDiffuse[0]);
        _surfaceShader->storeUniform4f("matSpecular", &matSpecular[0]);

        _surfaceShader->uploadUniforms(cvftb);

        Ngl::FlipState<GL_BLEND>      blendState;
        Ngl::FlipState<GL_CULL_FACE>  cullFaceState;
        //Ngl::FlipState<GL_DEPTH_TEST> depthTestState;
        
        if (culling) {
            glCullFace(cullFace);
            Ngl::FlipState<GL_CULL_FACE>::enable();
        }

        if (blend) {
            Ngl::FlipState<GL_BLEND>::enable();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
        }

        glDrawArrays(GL_QUADS, 0, minCount);

        if (blend) {
            glDepthMask(GL_TRUE);
        }

        _surfaceShader->unuse();
        Ngl::VertexAttrib::disconnect(*posAttrib);
    }

    void
    _drawLines(const Ngl::VertexBuffer &posVbo,
               const GLfloat            lineWidth,
               const em::glmat44f      &modelview,
               const em::glmat44f      &projection,
               const Ngl::vec4f        &lineColor,
               const NtTimeBundle      &cvftb,
               const bool               blend)
    {
        const Ngl::ShaderAttrib* posAttrib(
            _lineShader->queryConstAttrib("position"));

        EM_ASSERT(0 != posAttrib);

        const GLsizei minCount(Ngl::VertexAttrib::connect(*posAttrib, posVbo));

        _lineShader->use();
        _lineShader->storeUniform4m("projection", &projection[0][0]);
        _lineShader->storeUniform4m("modelview",  &modelview[0][0]);

        _lineShader->storeUniform4f("lineColor",  &lineColor[0]);

        _lineShader->uploadUniforms(cvftb);

        Ngl::FlipState<GL_BLEND>      blendState;
        Ngl::FlipState<GL_DEPTH_TEST> depthTestState;
        if (blend) {
            Ngl::FlipState<GL_BLEND>::enable();
            Ngl::FlipState<GL_DEPTH_TEST>::disable();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
        }

        glLineWidth(lineWidth);
        glDrawArrays(GL_LINES, 0, minCount);

        if (blend) {
            glDepthMask(GL_TRUE);
        }

        _lineShader->unuse();
        Ngl::VertexAttrib::disconnect(*posAttrib);
    }

    void
    _addCubes(const std::vector<_Cube> &cubes,
              _WsxVecType              &wsxVec,
              _LineMapType             &lineMap,
              _QuadMapType             &quadMap)
    {
        _TsxMapType  tsxMap;

        std::vector<_Cube>::const_iterator cubeIter(cubes.begin());
        for (; cubeIter != cubes.end(); ++cubeIter) {
            _addCube(*cubeIter, tsxMap, wsxVec, lineMap, quadMap);
        }
    }

    void
    _addCube(const _Cube  &c,
             _TsxMapType  &tsxMap,
             _WsxVecType  &wsxVec,
             _LineMapType &lineMap,
             _QuadMapType &quadMap)
    {
        const GLuint v[] = {
            _addVtx(
                _Vtx(_Vec3i(c.i0(),c.j0(),c.k0()),
                     _Vec3f(c.x0(),c.y0(),c.z0())), tsxMap, wsxVec),
            _addVtx(
                _Vtx(_Vec3i(c.i1(),c.j0(),c.k0()),
                     _Vec3f(c.x1(),c.y0(),c.z0())), tsxMap, wsxVec),
            _addVtx(
                _Vtx(_Vec3i(c.i1(),c.j0(),c.k1()),
                     _Vec3f(c.x1(),c.y0(),c.z1())), tsxMap, wsxVec),
            _addVtx(
                _Vtx(_Vec3i(c.i0(),c.j0(),c.k1()),
                     _Vec3f(c.x0(),c.y0(),c.z1())), tsxMap, wsxVec),
            _addVtx(
                _Vtx(_Vec3i(c.i0(),c.j1(),c.k0()),
                     _Vec3f(c.x0(),c.y1(),c.z0())), tsxMap, wsxVec),
            _addVtx(
                _Vtx(_Vec3i(c.i1(),c.j1(),c.k0()),
                     _Vec3f(c.x1(),c.y1(),c.z0())), tsxMap, wsxVec),
            _addVtx(
                _Vtx(_Vec3i(c.i1(),c.j1(),c.k1()),
                     _Vec3f(c.x1(),c.y1(),c.z1())), tsxMap, wsxVec),
            _addVtx(
                _Vtx(_Vec3i(c.i0(),c.j1(),c.k1()),
                     _Vec3f(c.x0(),c.y1(),c.z1())), tsxMap, wsxVec)};

        _addLine(_Line(v[0], v[1]), lineMap);
        _addLine(_Line(v[1], v[2]), lineMap);
        _addLine(_Line(v[2], v[3]), lineMap);
        _addLine(_Line(v[3], v[0]), lineMap);
        _addLine(_Line(v[4], v[5]), lineMap);
        _addLine(_Line(v[5], v[6]), lineMap);
        _addLine(_Line(v[6], v[7]), lineMap);
        _addLine(_Line(v[7], v[4]), lineMap);
        _addLine(_Line(v[0], v[4]), lineMap);
        _addLine(_Line(v[1], v[5]), lineMap);
        _addLine(_Line(v[2], v[6]), lineMap);
        _addLine(_Line(v[3], v[7]), lineMap);

        _addQuad(_Quad(v[1], v[5], v[6], v[2]), quadMap); // X+
        _addQuad(_Quad(v[7], v[4], v[0], v[3]), quadMap); // X-
        _addQuad(_Quad(v[7], v[6], v[5], v[4]), quadMap); // Y+
        _addQuad(_Quad(v[3], v[0], v[1], v[2]), quadMap); // Y-
        _addQuad(_Quad(v[2], v[6], v[7], v[3]), quadMap); // Z+
        _addQuad(_Quad(v[4], v[5], v[1], v[0]), quadMap); // Z-
    }

    //! Add the vertex to the data-structures.
    const GLuint
    _addVtx(const _Vtx  &vtx, _TsxMapType &tsxMap, _WsxVecType &wsxVec)
    {
        const _TsxMapType::const_iterator tsxIter(
            tsxMap.find(vtx.tsx()));

        if (tsxIter != tsxMap.end()) {
            return tsxIter->second; // Vertex exists in map, return index.
        }
        else {
            // Vertex not found in map, add it.

            wsxVec.push_back(vtx.wsx());
            const GLuint tsxMapSize(static_cast<GLuint>(tsxMap.size()));
            tsxMap.insert(_TsxMapType::value_type(vtx.tsx(), tsxMapSize));
            return tsxMapSize;
        }
    }

    void
    _addLine(const _Line  &line, _LineMapType &lineMap)
    {
        const _LineMapType::iterator lineIter(lineMap.find(line));

        if (lineIter != lineMap.end()) {
            ++lineIter->second;    // Line exists, increment count.
        }
        else {
            lineMap.insert(_LineMapType::value_type(line, 1)); // Count = 1.
        }
    }

    void
    _addQuad(const _Quad  &quad, _QuadMapType &quadMap)
    {
        const _QuadMapType::iterator quadIter(quadMap.find(quad));

        if (quadIter != quadMap.end()) {
            ++quadIter->second;    // Quad exists, increment count.
        }
        else {
            quadMap.insert(_QuadMapType::value_type(quad, 1)); // Count = 1.
        }
    }

    void
    _surfaceVertices(const _QuadMapType  &quadMap,
                     const _WsxVecType   &wsxVec,
                     std::vector<_Vec3f> &vtx)
    {
        vtx.clear();
        vtx.reserve(4*quadMap.size());

        _QuadMapType::const_iterator iter(quadMap.begin());
        const _QuadMapType::const_iterator iend(quadMap.end());
        for (; iter != iend; ++iter) {
            if (1 == iter->second) {
                for (int i(0); i < 4; ++i) {
                    vtx.push_back(wsxVec[iter->first[i]]);
                }
            }
        }
    }

    void
    _surfaceLineVertices(const _LineMapType  &lineMap,
                         const _WsxVecType   &wsxVec,
                         std::vector<_Vec3f> &vtx)
    {
        vtx.clear();
        vtx.reserve(4*lineMap.size());

        _LineMapType::const_iterator iter(lineMap.begin());
        const _LineMapType::const_iterator iend(lineMap.end());
        for (; iter != iend; ++iter) {
            if (3 >= iter->second) {
                for (int i(0); i < 2; ++i) {
                    vtx.push_back(wsxVec[iter->first[i]]);
                }
            }
        }
    }

    void
    _internalLineVertices(const _LineMapType  &lineMap,
                          const _WsxVecType   &wsxVec,
                          std::vector<_Vec3f> &vtx)
    {
        vtx.clear();
        vtx.reserve(4*lineMap.size());

        _LineMapType::const_iterator iter(lineMap.begin());
        const _LineMapType::const_iterator iend(lineMap.end());
        for (; iter != iend; ++iter) {
            if (4 <= iter->second) {
                for (int i(0); i < 2; ++i) {
                    vtx.push_back(wsxVec[iter->first[i]]);
                }
            }
        }
    }

    void
    _extractCoarseCubes(const Nb::TileLayout &layout,
                        std::vector<_Cube>   &cubes)
    {
        cubes.clear();
        cubes.reserve(layout.coarseTileCount());

        for (int t(0); t < layout.coarseTileCount(); ++t) {
            em::vec3f wsx0;
            em::vec3f wsx1;
            Nb::Tile ct(layout.coarseTile(t));
            ct.bounds(wsx0, wsx1);

            cubes.push_back(
                _Cube(_Vtx(_Vec3i(ct.ti(), ct.tj(), ct.tk()),
                           _Vec3f(wsx0[0], wsx0[1], wsx0[2])),
                      _Vtx(_Vec3i(ct.ti()+1, ct.tj()+1, ct.tk()+1),
                           _Vec3f(wsx1[0], wsx1[1], wsx1[2]))));
        }
    }

    void
    _extractFineCubes(const Nb::TileLayout &layout,
                      const int             tile0,
                      const int             tile1,
                      std::vector<_Cube>   &cubes)
    {
        const int tileCount(tile1 - tile0);

        cubes.clear();
        cubes.reserve(tileCount);

        for (int t(tile0); t < tile1; ++t) {
            em::vec3f wsx0;
            em::vec3f wsx1;
            Nb::Tile ft(layout.fineTile(t));
            ft.bounds(wsx0, wsx1);

            cubes.push_back(
                _Cube(_Vtx(_Vec3i(ft.ti(), ft.tj(), ft.tk()),
                           _Vec3f(wsx0[0], wsx0[1], wsx0[2])),
                      _Vtx(_Vec3i(ft.ti()+1, ft.tj()+1, ft.tk()+1),
                           _Vec3f(wsx1[0], wsx1[1], wsx1[2]))));
        }
    }

private:    // Member variables.

    Ngl::ShaderProgram *_surfaceShader;
    Ngl::ShaderProgram *_lineShader;
};

// -----------------------------------------------------------------------------
