// -----------------------------------------------------------------------------
//
// Ns3DMeshScope.h
//
// Naiad Studio Mesh scope - renders the point/triangle shapes of a body.
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

#ifndef NS3D_MESH_SCOPE_H
#define NS3D_MESH_SCOPE_H

#include <Ni.h>
#include <NgBodyOp.h>
#include <NgInput.h>

#include <NbBody.h>
#include <NbPointShape.h>
#include <NbTriangleShape.h>
#include <NbBufferChannel.h>

#include <NglShaderProgram.h>
#include <NglVertexAttrib.h>
#include <NglState.h>
#include <NglUtils.h>

#include "Ns3DCameraScope.h"
#include "NsQuery.h"

#include <em_vec.h>

// -----------------------------------------------------------------------------

class Ns3DMeshScope : public Ns3DBodyScope
{
public:

    Ns3DMeshScope() : Ns3DBodyScope()
    {
        _shaderMap.insert(
            ShaderMap::value_type(
                "Flat Shaded",
                new Ngl::ShaderProgram(
                    NtString(shaderPath() + "mesh-solid-flat.vs"),
                    NtString(shaderPath() + "mesh-solid-flat.fs"))));

        _shaderMap.insert(
            ShaderMap::value_type(
                "Smooth Shaded",
                new Ngl::ShaderProgram(
                    NtString(shaderPath() + "mesh-solid-smooth.vs"),
                    NtString(shaderPath() + "mesh-solid-smooth.fs"))));

        _shaderMap.insert(
            ShaderMap::value_type(
                "Flat Shaded + Wireframe",
                new Ngl::ShaderProgram(
                    NtString(shaderPath() + "mesh-solid-wireframe-flat.vs"),
                    NtString(shaderPath() + "mesh-solid-wireframe-flat.fs"))));

        _shaderMap.insert(
            ShaderMap::value_type(
                "Smooth Shaded + Wireframe",
                new Ngl::ShaderProgram(
                    NtString(shaderPath() + "mesh-solid-wireframe-smooth.vs"),
                    NtString(shaderPath() + "mesh-solid-wireframe-smooth.fs"))));

        _shaderMap.insert(
            ShaderMap::value_type(
                "Wireframe",
                new Ngl::ShaderProgram(
                    NtString(shaderPath() + "mesh-wireframe.vs"),
                    NtString(shaderPath() + "mesh-wireframe.fs"))));



        _shaderMap.insert(
            ShaderMap::value_type(
                "mesh-lines",
                new Ngl::ShaderProgram(
                    NtString(shaderPath() + "mesh-lines.vs"),
                    NtString(shaderPath() + "mesh-lines.fs"))));
    }

    virtual
    ~Ns3DMeshScope()
    {
        // Free shader resources.

        for (ShaderMap::iterator iter = _shaderMap.begin();
             iter != _shaderMap.end();
             ++iter) {
             delete iter->second;
        }
    }

    virtual NtString
    typeName() const
    { return "Mesh-Scope"; }

    virtual void
    setContext(const QGLContext* context)
    {}

    virtual bool
    draw(NsBodyObject*          nsBody,
         const Ns3DCameraScope* cam,
         const Ngl::Viewport&   vp)
    {        
        try {
            drawMesh(nsBody);

            if(param1e("Velocity")->eval(currentTime()) == "On") {
                drawVelocityVectors(nsBody);
            }
            
            if(param1e("Vertex Normals")->eval(currentTime())=="On") {
                drawNormalVectors(nsBody);
            }           
        }
        catch(std::exception& e) {
            
        }

        return true;
    }

public:

    Nb::Buffer3f    vn;

private:

    typedef std::map<NtString, Ngl::ShaderProgram*> ShaderMap;

    ShaderMap               _shaderMap;


private:

    GLsizei
    connectShaderAttribs(const Ngl::ShaderProgram& shader,
                         NsBodyObject* nsBody)
    {
        if (shader.attribMap().empty()) {
            return 0;
        }

        Ns3DBody* ns3DBody = nsBody->ns3DBody();
        const Nb::Body* body(&nsBody->nbBody());

        // no body, nothing to do...
        if(!body) {
            NB_WARNING("Empty body: '" << fromQStr(nsBody->name()) << "'");
            return 0;
        }

        GLsizei minCount((std::numeric_limits<GLsizei>::max)());

        for (Ngl::ShaderProgram::AttribMap::const_iterator iter
                = shader.attribMap().begin();
             iter != shader.attribMap().end();
             ++iter) {
#if 0
            std::cerr   << "Attrib Query:\n"
                        << "Name: '" << iter->first << "'\n"
                        << "Type: " << Ngl::GLType::str(iter->second.type()) << "\n"
                        << "Size: " << iter->second.size() << "\n"
                        << "Location: " << iter->second.location() << "\n\n";
#endif

            const Ngl::VertexBuffer* vtxBuf(0);

            if ("position" == iter->first) {

                vtxBuf = ns3DBody->queryConstVertexBuffer(
                    "SHARED","Point/position"
                    );

                if (0 == vtxBuf) {
                    const Nb::PointShape& point(body->constPointShape());
                    const Nb::TriangleShape& triangle(body->constTriangleShape());
                    const Nb::Buffer3f& position3f(point.constBuffer3f("position"));
                    const Nb::Buffer3i& index3i(triangle.constBuffer3i("index"));

                    std::vector<em::vec<3,GLfloat> > positions;
                    positions.reserve(3*index3i.size());

                    for (int64_t i = 0; i < index3i.size(); ++i) {
                        const NtVec3i tri(index3i(i));
                        positions.push_back(position3f[tri[0]]);
                        positions.push_back(position3f[tri[1]]);
                        positions.push_back(position3f[tri[2]]);
                    }

                    vtxBuf = nsBody->ns3DBody()->createVertexBuffer(
                        "SHARED",
                        "Point/position",
                        sizeof(em::vec<3,GLfloat>)*positions.size(),
                        &positions[0]
                        );
                }
            }
            else if("normal" == iter->first) {

                vtxBuf = ns3DBody->queryConstVertexBuffer(
                    "SHARED","Point/normal"
                    );

                if (0 == vtxBuf) {
                    const Nb::PointShape& point(body->constPointShape());
                    const Nb::TriangleShape& triangle(body->constTriangleShape());
                    const Nb::Buffer3f& position3f(point.constBuffer3f("position"));
                    const Nb::Buffer3i& index3i(triangle.constBuffer3i("index"));

                    Nb::Buffer3f normal3f;
                    computeVertexNormals(position3f, index3i, normal3f);

                    std::vector<em::vec<3,GLfloat> > normals;
                    normals.reserve(3*index3i.size());

                    for (int64_t i = 0; i < index3i.size(); ++i) {
                        const NtVec3i tri(index3i(i));
                        normals.push_back(normal3f[tri[0]]);
                        normals.push_back(normal3f[tri[1]]);
                        normals.push_back(normal3f[tri[2]]);
                    }

                    vtxBuf
                        = nsBody->ns3DBody()->createVertexBuffer(
                            "SHARED",
                            "Point/normal",
                            sizeof(em::vec<3,GLfloat>)*normals.size(),
                            &normals[0]
                            );
                }
            }
            else if("triVtx0" == iter->first) {

                vtxBuf = ns3DBody->queryConstVertexBuffer(
                    "SHARED","Point/vtx0"
                    );

                if (0 == vtxBuf) {
                    const Nb::PointShape& point(body->constPointShape());
                    const Nb::TriangleShape& triangle(body->constTriangleShape());
                    const Nb::Buffer3f& position3f(point.constBuffer3f("position"));
                    const Nb::Buffer3i& index3i(triangle.constBuffer3i("index"));

                    std::vector<em::vec<3,GLfloat> > vtx0;
                    vtx0.reserve(3*index3i.size());

                    for (int64_t i = 0; i < index3i.size(); ++i) {
                        const NtVec3i tri(index3i(i));
                        vtx0.push_back(position3f[tri[0]]);
                        vtx0.push_back(position3f[tri[0]]);
                        vtx0.push_back(position3f[tri[0]]);
                    }

                    vtxBuf
                        = nsBody->ns3DBody()->createVertexBuffer(
                            "SHARED",
                            "Point/vtx0",
                            sizeof(em::vec<3,GLfloat>)*vtx0.size(),
                            &vtx0[0]);
                }
            }
            else if("triVtx1" == iter->first) {

                vtxBuf = ns3DBody->queryConstVertexBuffer(
                    "SHARED","Point/vtx1"
                    );

                if (0 == vtxBuf) {
                    const Nb::PointShape& point(body->constPointShape());
                    const Nb::TriangleShape& triangle(body->constTriangleShape());
                    const Nb::Buffer3f& position3f(point.constBuffer3f("position"));
                    const Nb::Buffer3i& index3i(triangle.constBuffer3i("index"));

                    std::vector<em::vec<3,GLfloat> > vtx1;
                    vtx1.reserve(3*index3i.size());

                    for (int64_t i = 0; i < index3i.size(); ++i) {
                        const NtVec3i tri(index3i(i));
                        vtx1.push_back(position3f[tri[1]]);
                        vtx1.push_back(position3f[tri[1]]);
                        vtx1.push_back(position3f[tri[1]]);
                    }

                    vtxBuf
                        = nsBody->ns3DBody()->createVertexBuffer(
                            "SHARED",
                            "Point/vtx1",
                            sizeof(em::vec<3,GLfloat>)*vtx1.size(),
                            &vtx1[0]
                            );
                }
            }
            else if("triVtx2" == iter->first) {

                vtxBuf = ns3DBody->queryConstVertexBuffer(
                    "SHARED","Point/vtx2"
                    );

                if (0 == vtxBuf) {
                    const Nb::PointShape& point(body->constPointShape());
                    const Nb::TriangleShape& triangle(body->constTriangleShape());
                    const Nb::Buffer3f& position3f(point.constBuffer3f("position"));
                    const Nb::Buffer3i& index3i(triangle.constBuffer3i("index"));

                    std::vector<em::vec<3,GLfloat> > vtx2;
                    vtx2.reserve(3*index3i.size());

                    for (int64_t i = 0; i < index3i.size(); ++i) {
                        const NtVec3i tri(index3i(i));
                        vtx2.push_back(position3f[tri[2]]);
                        vtx2.push_back(position3f[tri[2]]);
                        vtx2.push_back(position3f[tri[2]]);
                    }

                    vtxBuf
                        = nsBody->ns3DBody()->createVertexBuffer(
                            "SHARED",
                            "Point/vtx2",
                            sizeof(em::vec<3,GLfloat>)*vtx2.size(),
                            &vtx2[0]
                            );
                }
            }
            // else: ERROR!!

            const GLsizei count(
                Ngl::VertexAttrib::connect(iter->second, *vtxBuf));

            minCount = std::min(minCount, count);
        }

        return minCount;
    }


    void
    disconnectShaderAttribs(const Ngl::ShaderProgram& shader)
    {
        for (Ngl::ShaderProgram::AttribMap::const_iterator iter
                = shader.attribMap().begin();
             iter != shader.attribMap().end();
             ++iter) {
             Ngl::VertexAttrib::disconnect(iter->second);
        }
    }


    void
    setShaderUniforms(Ngl::ShaderProgram& shader,
                      const NtString& displayMode)
    {
        GLfloat proj[16];
        glGetFloatv(GL_PROJECTION_MATRIX, proj);

        GLfloat mv[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);

        shader.storeUniform4m("modelview", mv);
        shader.storeUniform4m("projection", proj);

        // Old settings

//      const GLfloat lightAmbient[] = {0.7f, 0.7f, 0.7f, 1.0f};
//      const GLfloat lightDiffuse[] = {0.25f, 0.25f, 0.25f, 1.0f};
//      const GLfloat frontMatShininess[] = {15.0f};
//      const GLfloat frontMatSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f};
//      const GLfloat frontMatDiffuse[] = {0.8705882f,
//                                         0.8745098f,
//                                         0.8000000f, 1.0};
//      const GLfloat backMatShininess[] = {15.0f};
//      const GLfloat backMatSpecular[] = {0.2f, 0.2f, 0.2f, 1.0f};
//      const GLfloat backMatDiffuse[] = {0.8705882f,
//                                        0.8745098f,
//                                        0.8000000f, 1.0};

        if ("Flat Shaded"               == displayMode ||
            "Flat Shaded + Wireframe"   == displayMode ||
            "Smooth Shaded"             == displayMode ||
            "Smooth Shaded + Wireframe" == displayMode) {
            // Any time we are drawing surfaces set material and
            // light parameters.

            const GLfloat frontMatAmbient[]  =
            {
                param3f("Front Ambient")->eval(currentTime(), 0),
                param3f("Front Ambient")->eval(currentTime(), 1),
                param3f("Front Ambient")->eval(currentTime(), 2),
                1.0f
            };

            const GLfloat frontMatDiffuse[]  =
            {
                param3f("Front Diffuse")->eval(currentTime(), 0),
                param3f("Front Diffuse")->eval(currentTime(), 1),
                param3f("Front Diffuse")->eval(currentTime(), 2),
                1.0f
            };

            const GLfloat frontMatSpecular[] =
            {
                param3f("Front Specular")->eval(currentTime(), 0),
                param3f("Front Specular")->eval(currentTime(), 1),
                param3f("Front Specular")->eval(currentTime(), 2),
                1.0f
            };

            const GLfloat frontMatShininess
                = param1f("Front Shininess")->eval(currentTime());

            const GLfloat backMatAmbient[] =
            {
                param3f("Back Ambient")->eval(currentTime(), 0),
                param3f("Back Ambient")->eval(currentTime(), 1),
                param3f("Back Ambient")->eval(currentTime(), 2),
                1.0f
            };

            const GLfloat backMatDiffuse[] =
            {
                param3f("Back Diffuse")->eval(currentTime(), 0),
                param3f("Back Diffuse")->eval(currentTime(), 1),
                param3f("Back Diffuse")->eval(currentTime(), 2),
                1.0f
            };

            const GLfloat backMatSpecular[] =
            {
                param3f("Back Specular")->eval(currentTime(), 0),
                param3f("Back Specular")->eval(currentTime(), 1),
                param3f("Back Specular")->eval(currentTime(), 2),
                1.0f
            };

            const GLfloat backMatShininess
                = param1f("Back Shininess")->eval(currentTime());

            const GLfloat lightAmbient[] =
            {
                param3f("Light Ambient")->eval(currentTime(), 0),
                param3f("Light Ambient")->eval(currentTime(), 1),
                param3f("Light Ambient")->eval(currentTime(), 2),
                1.0f
            };

            const GLfloat lightDiffuse[] =
            {
                param3f("Light Diffuse")->eval(currentTime(), 0),
                param3f("Light Diffuse")->eval(currentTime(), 1),
                param3f("Light Diffuse")->eval(currentTime(), 2),
                1.0f
            };

            const GLfloat lightSpecular[] =
            {
                param3f("Light Specular")->eval(currentTime(), 0),
                param3f("Light Specular")->eval(currentTime(), 1),
                param3f("Light Specular")->eval(currentTime(), 2),
                1.0f
            };

            const GLfloat lightPosition[] =
            {
                param3f("Light Position")->eval(currentTime(), 0),
                param3f("Light Position")->eval(currentTime(), 1),
                param3f("Light Position")->eval(currentTime(), 2)
            };

            shader.storeUniform4f("frontMatAmbient", frontMatAmbient);
            shader.storeUniform4f("frontMatDiffuse", frontMatDiffuse);
            shader.storeUniform4f("frontMatSpecular", frontMatSpecular);
            shader.storeUniform1f("frontMatShininess", frontMatShininess);
            shader.storeUniform4f("backMatAmbient", backMatAmbient);
            shader.storeUniform4f("backMatDiffuse", backMatDiffuse);
            shader.storeUniform4f("backMatSpecular", backMatSpecular);
            shader.storeUniform1f("backMatShininess", backMatShininess);
            shader.storeUniform4f("lightAmbient", lightAmbient);
            shader.storeUniform4f("lightDiffuse", lightDiffuse);
            shader.storeUniform4f("lightSpecular", lightSpecular);
            shader.storeUniform3f("lightPosition", lightPosition);

            const em::glmat33f nm(
                em::make_cam_normal_matrix(
                    em::glmat44f(mv)));

            shader.storeUniform3m("normalMatrix", &nm[0][0]);
        }

        if ("Wireframe"                 == displayMode ||
            "Flat Shaded + Wireframe"   == displayMode ||
            "Smooth Shaded + Wireframe" == displayMode) {
            // Any time we are drawing lines set line parameters. This
            // is also the only time we need the viewport.

            const GLfloat lineColor[] =
            {
                param3f("Line Color")->eval(currentTime(), 0),
                param3f("Line Color")->eval(currentTime(), 1),
                param3f("Line Color")->eval(currentTime(), 2),
                1.0f
            };

            const GLfloat lineWidth
                = param1f("Line Width")->eval(currentTime());

            const GLfloat lineExp   = 4.f;

            shader.storeUniform1f( "lineWidth", lineWidth);
            shader.storeUniform1f( "lineExp", lineExp);
            shader.storeUniform4f("lineColor", lineColor);

            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            shader.storeUniform4i("viewport", viewport);
        }
    }


    void
    computeVertexNormals(const Nb::Buffer3f& position3f,
                         const Nb::Buffer3i& index3i,
                         Nb::Buffer3f&       normal3f)
    {
        normal3f.resize(position3f.size());

        // Initialize to zero length.

        for(int64_t n(0); n < normal3f.size(); ++n) {
            normal3f[n] = em::vec<3,GLfloat>(0.f, 0.f, 0.f);
        }

        // Compute face normals and add contributions to vertices.

        for(int64_t i(0); i < index3i.size(); ++i) {
            const NtVec3i& tri(index3i[i]);

            // Assume CCW ordering.

            const em::vec<3,GLfloat>& pos3f0(position3f[tri[0]]);
            const em::vec<3,GLfloat> faceNml(
                    em::cross(
                        position3f[tri[1]] - pos3f0,
                        position3f[tri[2]] - pos3f0));

            // Add face normal to normal at vertex index in triangle.

            normal3f[tri[0]] += faceNml;
            normal3f[tri[1]] += faceNml;
            normal3f[tri[2]] += faceNml;
        }

        // Normalize vertex normals.

        for(int64_t n(0); n < normal3f.size(); ++n) {
            em::normalize(normal3f[n]);
            //normal3f[n] = em::normalized(normal3f[n]);
        }
    }

    void
    drawMesh(NsBodyObject* nsBody)
    {
        // Set shader

        const NtString prmDisplayMode(
            param1e("Display Mode")->eval(currentTime())
            );

        Ngl::FlipState<GL_BLEND> blendState;
        if ("Wireframe" == prmDisplayMode) {
            Ngl::FlipState<GL_BLEND>::enable();
        }

        Ngl::ShaderProgram* shader(0);
        ShaderMap::iterator shit = _shaderMap.find(prmDisplayMode);
        if(shit != _shaderMap.end())
            shader = shit->second;

        if(!shader) {
            NB_WARNING("Unknown shader '" << prmDisplayMode <<
                       "': defaulting to Flat Shaded");
            shader = _shaderMap.find("Flat Shaded")->second;
        }

        const GLsizei minCount(connectShaderAttribs(*shader, nsBody));

        setShaderUniforms(*shader, prmDisplayMode);
        shader->use();
        shader->uploadUniforms(currentTime());
        glDrawArrays(GL_TRIANGLES, 0, minCount);
        shader->unuse();

        disconnectShaderAttribs(*shader);

        ssHud << "Body: '" << fromQStr(nsBody->name())
              << "': " << minCount/3 << " triangles\n";
    }

    void
    drawVelocityVectors(NsBodyObject* nsBody)
    {
        const Nb::Body* body(&nsBody->nbBody());
        Ns3DBody* ns3DBody(nsBody->ns3DBody());

        const Nb::PointShape& point(body->constPointShape()); // May throw

        const float dt = queryCurrentVisibleFrameTimeBundle().frame_dt;
        
        if (point.hasChannels3f("velocity")) {

            // NB: Hard-coded shader!
            Ngl::ShaderProgram* shader = _shaderMap.find("mesh-lines")->second;

            // Set vertex attributes

            GLsizei minCount((std::numeric_limits<GLsizei>::max)());

            for (Ngl::ShaderProgram::AttribMap::const_iterator iter
                    = shader->attribMap().begin();
                 iter != shader->attribMap().end();
                 ++iter) {

                const Ngl::VertexBuffer* vtxBuf(0);

                if ("position" == iter->first) {

                    // HACK: Hard-coded!
                    static const NtString vtxBufName("velocity-position");

                    vtxBuf = ns3DBody->queryConstVertexBuffer(
                        "SHARED",vtxBufName
                        );

                    if (0 == vtxBuf) {
                        const GLfloat uscale(
                             param1f("Velocity Display Scale")->eval(
                                currentTime()));

                        // Get values from Nb::Body channels

                        const Nb::Buffer3f& position3f(
                            point.constBuffer3f("position"));
                        const Nb::Buffer3f& velocity3f(
                            point.constBuffer3f("velocity"));

                        std::vector<em::vec<3,GLfloat> > positions;
                        positions.reserve(2*position3f.size());

                        for (int64_t p = 0; p < position3f.size(); ++p) {
                            positions.push_back(position3f[p]);
                            positions.push_back(position3f[p] +
                                                velocity3f[p]*uscale*dt);
                        }

                        vtxBuf
                            = ns3DBody->createVertexBuffer(
                                "SHARED",
                                vtxBufName,
                                sizeof(em::vec<3,GLfloat>)*positions.size(),
                                &positions[0]
                                );
                     }
                 }
                 else if ("color" == iter->first) {

                     // HACK: Hard-coded!
                     static const NtString vtxBufName("velocity-color");

                     vtxBuf = ns3DBody->queryConstVertexBuffer(
                         "SHARED",
                         vtxBufName
                         );

                     if (0 == vtxBuf) {
                         const Nb::Buffer3f& position3f(
                            point.constBuffer3f("position"));

                         std::vector<em::vec<4, GLfloat> > colors;
                         colors.reserve(2*position3f.size());

                         for (int64_t i = 0; i < position3f.size(); ++i) {
                             // HACK: hard-coded colors!
                             colors.push_back(
                                 em::vec<4,GLfloat>(0.9176471f,
                                                    0.9215686f,
                                                    0.2901961f,
                                                    1.f));
                             colors.push_back(
                                 em::vec<4,GLfloat>(0.4176471f,
                                                    0.4784314f,
                                                    0.f,
                                                    1.f));
                         }

                         vtxBuf
                             = ns3DBody->createVertexBuffer(
                                 "SHARED",
                                 vtxBufName,
                                 sizeof(em::vec<4,GLfloat>)*colors.size(),
                                 &colors[0]
                                 );
                     }
                 }

                 const GLsizei count(
                     Ngl::VertexAttrib::connect(iter->second, *vtxBuf));

                 minCount = std::min(minCount, count);
            }

            GLfloat proj[16];
            glGetFloatv(GL_PROJECTION_MATRIX, proj);

            GLfloat mv[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, mv);

            shader->storeUniform4m("projection", proj);
            shader->storeUniform4m("modelview",  mv);

            shader->use();
            shader->uploadUniforms(currentTime());
            glDrawArrays(GL_LINES, 0, minCount);
            shader->unuse();

            disconnectShaderAttribs(*shader);
        }
    }


    void
    drawNormalVectors(NsBodyObject* nsBody)
    {
        const Nb::Body* body(&nsBody->nbBody());
        Ns3DBody*   ns3DBody(nsBody->ns3DBody());

        const Nb::PointShape& point(body->constPointShape());

        Ngl::ShaderProgram* shader = _shaderMap.find("mesh-lines")->second;

        GLsizei minCount((std::numeric_limits<GLsizei>::max)());

        for (Ngl::ShaderProgram::AttribMap::const_iterator iter
                = shader->attribMap().begin();
             iter != shader->attribMap().end();
             ++iter) {
             const Ngl::VertexBuffer* vtxBuf(0);

             if ("position" == iter->first) {

                 static const NtString vtxBufName("normal-position");

                 vtxBuf = ns3DBody->queryConstVertexBuffer("SHARED",vtxBufName);

                 if (0 == vtxBuf) {
                     const GLfloat nscale(
                         param1f("Normal Display Scale")->eval(
                                 currentTime()));

                     const Nb::Buffer3f& position3f(
                        point.constBuffer3f("position"));
                     const Nb::TriangleShape& triangle(
                        body->constTriangleShape());
                     const Nb::Buffer3i& index3i(
                        triangle.constBuffer3i("index"));

                     Nb::Buffer3f normal3f;
                     computeVertexNormals(position3f, index3i, normal3f);

                     std::vector<em::vec<3,GLfloat> > positions;
                     positions.reserve(2*position3f.size());

                     for (int64_t i = 0; i < position3f.size(); ++i) {
                         positions.push_back(position3f(i));
                         positions.push_back(position3f(i) +
                                             normal3f(i)*nscale);
                     }

                     vtxBuf
                         = ns3DBody->createVertexBuffer(
                             "SHARED",
                             vtxBufName,
                             sizeof(em::vec<3,GLfloat>)*positions.size(),
                             &positions[0]
                             );
                 }
             }
             else if ("color" == iter->first) {

                 static const NtString vtxBufName("normal-color");

                 vtxBuf = ns3DBody->queryConstVertexBuffer("SHARED",vtxBufName);

                 if (0 == vtxBuf) {
                     const Nb::Buffer3f& position3f(
                        point.constBuffer3f("position"));

                     std::vector<em::vec<4, GLfloat> > colors;
                     colors.reserve(2*position3f.size());

                     for (int64_t i = 0; i < position3f.size(); ++i) {
                         colors.push_back(
                             em::vec<4,GLfloat>(0.5411765f,
                                                0.5411765f,
                                                0.5411765f,
                                                0.5f));
                         colors.push_back(
                             em::vec<4,GLfloat>(0.5411765f,
                                                0.5411765f,
                                                0.5411765f,
                                                0.5f));
                     }

                     vtxBuf
                         = ns3DBody->createVertexBuffer(
                             "SHARED",
                             vtxBufName,
                             sizeof(em::vec<4,GLfloat>)*colors.size(),
                             &colors[0]
                             );

                 }
             }

             const GLsizei count(
                 Ngl::VertexAttrib::connect(iter->second, *vtxBuf));

             minCount = std::min(minCount, count);
        }

        Ngl::FlipState<GL_BLEND> blendState;
        Ngl::FlipState<GL_BLEND>::enable();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLfloat proj[16];
        glGetFloatv(GL_PROJECTION_MATRIX, proj);
        
        GLfloat mv[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        
        shader->storeUniform4m("projection", proj);
        shader->storeUniform4m("modelview",  mv);

        shader->use();
        shader->uploadUniforms(currentTime());
        glDrawArrays(GL_LINES, 0, minCount);
        shader->unuse();

        disconnectShaderAttribs(*shader);
    }

};

// -----------------------------------------------------------------------------

#endif  // NS3D_MESH_SCOPE_H



// Unit cube
//
//const Ngl::vec3f vtx[] = {
//    Ngl::vec3f(-1.f, -1.f, -1.f),
//    Ngl::vec3f( 1.f, -1.f, -1.f),
//    Ngl::vec3f( 1.f, -1.f,  1.f),
//    Ngl::vec3f(-1.f, -1.f,  1.f),
//    Ngl::vec3f(-1.f,  1.f, -1.f),
//    Ngl::vec3f( 1.f,  1.f, -1.f),
//    Ngl::vec3f( 1.f,  1.f,  1.f),
//    Ngl::vec3f(-1.f,  1.f,  1.f)
//};
//
//const Ngl::vec3i tri[] = {
//    Ngl::vec3i(1,5,6), Ngl::vec3i(6,2,1), // X+
//    Ngl::vec3i(7,4,0), Ngl::vec3i(0,3,7), // X-
//    Ngl::vec3i(6,5,4), Ngl::vec3i(4,7,6), // Y+
//    Ngl::vec3i(3,0,1), Ngl::vec3i(1,2,3), // Y-
//    Ngl::vec3i(2,6,7), Ngl::vec3i(7,3,2), // Z+
//    Ngl::vec3i(0,4,5), Ngl::vec3i(5,1,0)  // Z-
//};
//
//const Ngl::vec3f nml[] = {
//    Ngl::vec3f( 1.f,  0.f,  0.f),     // X+
//    Ngl::vec3f(-1.f,  0.f,  0.f),     // X-
//    Ngl::vec3f( 0.f,  1.f,  0.f),     // Y+
//    Ngl::vec3f( 0.f, -1.f,  0.f),     // Y-
//    Ngl::vec3f( 0.f,  0.f,  1.f),     // Z+
//    Ngl::vec3f( 0.f,  0.f, -1.f)      // Z-
//};
