// -----------------------------------------------------------------------------
//
// Ns3DParticleScope.h
//
// Naiad Studio Particle scope - renders the particle shape of a body.
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

#include <Ni.h>
#include <NgBodyOp.h>
#include <NgInput.h>

#include <NbParticleShape.h>

#include <NglExtensions.h>
#include <NglShaderProgram.h>
#include <NglVertexArrayObject.h>
#include <NglVertexAttrib.h>
//#include <NglPointVao.h>

#include "NsStringUtils.h"
#include "NsCmdSetParam3.h"

#include "Ns3DCameraScope.h"

#include <em_vec.h>

#include <sstream>
#include <map>
#include <limits>

// -----------------------------------------------------------------------------

// At present the particle-scope contains a lot of hard-coded information.
// I will give a few ideas for future development below.
//
// (1) The list of available shaders is currently created in the constructor
//     and assumes the presence of certain files in a certain directory. A
//     more flexible solution would be to let the scope 'crawl' a given directory
//     for 'appropriate' shaders instead. This way, users could easily write
//     their own shaders and use them inside nstudio.
//
//     At present, shaders are specified in two separate files
//     (vertex + fragment shader). Most likely things would be easier to manage
//     if both components were given in the same file.
//
//     In order to determine if a shader is appropriate for use with a
//     particle scope it could either be placed in a certain folder
//     (e.g. .../shaders/particle/bigBlue.shader) or have a unique extension
//     (e.g. .../shader/bigBlue.part). Both these solutions seem reasonable.
//
//     In the scenario where the particle-scope 'crawls' a directory the user
//     could be presented with a drop-down list of available shaders. This list
//     would include user-shaders, as long as they are placed in the correct
//     location on disk. If required, there could also be a text-input field
//     that lets the user specify a full path to a shader file, but this may
//     complicate sharing of shader resources between scope instances
//     [see (2)].
//
// (2) All particle-scope instances should share shader resources. There is no
//     point in generating separate (identical) shader programs for each
//     instance. In fact, this is a clear waste of resources.
//
// (3) The user should be presented with shader settings in the
//     'Op Parameter Tab' of the scope and the settings should reflect the
//     current shader only. Seeing as information about shaders does not become
//     available until they are linked (which is necessarily done at run-time),
//     this part of the GUI needs to be dynamically created. Further, it needs
//     to adapt to the user switching between shaders.
//
//     I will give a proposed layout for the 'Op Parameters' for the
//     particle-scope below. I use asterisks and indentation to represent
//     header level.
//
//     *Particle-Scope*
//         Type: [string] (read-only)
//         Name: [string]
//
//       **System**
//           Solve Priority: [int]
//           Enabled:        [int]
//
//       **Bodies**
//           Show Bodies: [string]
//
//       **Style**           # OpenGL state that's not available in shaders
//           Point Smooth     : [bool]  #check-box
//
//           # NB: Could be done in vertex shader
//           Point Size       : [float]
//           Point Attenuation: [float] [float] [float]
//
//       **Shader**
//           Name: [string]      # Drop-down or text-input
//
//           ***Settings***                              # (i.e. uniforms)
//           Gradient Lighting : [bool]                  #check-box
//           Normalize Gradient: [bool]                  #check-box
//           Light Direction   : [float] [float] [float]
//           # ... list is dynamically generated after shader is linked.
//
//           ***Channels***                              # (i.e. attributes)
//           # (Attrib name) (Attrib type) | (Body Channel) (Sample Channel)
//              position      Vec3f        | [string]       [string]
//              velocity      Vec3f        | [string]       [string]
//              gradient      Vec3f        | [string]       [string]
//           # ... list is dynamically generated after shader is linked.
//
// # Examples: position Vec3f [Particle/position   ] [<empty>          ]
//             gradient Vec3f [Field/fluid-distance] [Particle/position]
//
//     The lists of uniforms and attributes become available once shaders are
//     linked. Information regarding type and name are the most important in
//     this case. The types are used to determine which calls need to be made
//     to the Ns3DBody that is being drawn.
//
//     Channel data is requested from an Ns3DBody, which is responsible for
//     maintaining channel data during its existence. This way several scopes
//     looking at the same body are able to share channel data. Also, switching
//     between shaders doesn't always require all channel data to be updated.
//
// (4) There are two uniform variables that need to be treated separately at
//     the moment. These are the modelview and projection matrices. These
//     should not be listed above, but rather set according to the given
//     camera.
//
// (5) In an ideal case the user would be presented with a graphical view of
//     existing body channels and shader attributes, and would be able to
//     connect them up, perhaps even using some filters in the process. A lot
//     of error checking could then be moved to this module, making illegal
//     connections impossible.
//

class Ns3DParticleScope : public Ns3DBodyScope
{
public:

    Ns3DParticleScope()
        : Ns3DBodyScope(),
          _bodyMin(std::numeric_limits<float>::max(),
                   std::numeric_limits<float>::max(),
                   std::numeric_limits<float>::max()),
          _bodyMax(-std::numeric_limits<float>::max(),
                   -std::numeric_limits<float>::max(),
                   -std::numeric_limits<float>::max())
    {
        // Create shaders.
        // TODO: This list of shaders should be generated automatically
        //       by examining shader files in a certain directory.

        _shaderMap.insert(
            ShaderMap::value_type(
                "The Big Blue",
                new Ngl::ShaderProgram(
                    NtString(shaderPath() + "big-blue.vs"),
                    NtString(shaderPath() + "big-blue.fs"))));

        _shaderMap.insert(
            ShaderMap::value_type(
                "Channel Color",
                new Ngl::ShaderProgram(
                    NtString(shaderPath() + "channel-color.vs"),
                    NtString(shaderPath() + "channel-color.fs"))));

        _shaderMap.insert(
            ShaderMap::value_type(
                "Gradient False Color",
                new Ngl::ShaderProgram(
                    NtString(shaderPath() + "false-color.vs"),
                    NtString(shaderPath() + "false-color.fs"))));

//        _shaderMap.insert(
//            ShaderMap::value_type(
//                "Maldives",
//                new Ngl::ShaderProgram(
//                    NtString(shaderPath() + "maldives.vs"),
//                    NtString(shaderPath() + "maldives.fs"))));

        // TMP!!
        // This map contains information required to request a
        // body channel [value] given a shader attribute name [key]

        _bodyChannelMap.insert(
            BodyChannelMap::value_type(
                "position",
                BodyChannel("Particle.position",
                            "",
                            GL_FALSE,
                            GL_STATIC_DRAW)));
        _bodyChannelMap.insert(
            BodyChannelMap::value_type(
                "velocity",
                BodyChannel("Particle.velocity",
                            "",
                            GL_FALSE,
                            GL_STATIC_DRAW)));
        _bodyChannelMap.insert(
            BodyChannelMap::value_type(
                "colorChannel",
                BodyChannel("Particle.velocity",
                            "",
                            GL_FALSE,
                            GL_STATIC_DRAW)));
        _bodyChannelMap.insert(
            BodyChannelMap::value_type(
                "xgradient",
                BodyChannel("Field.fluid-distance",
                            "Particle.position",
                            GL_FALSE,
                            GL_STATIC_DRAW)));
    }

    virtual
    ~Ns3DParticleScope()
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
    { return "Particle-Scope"; }

    virtual void
    setContext(const QGLContext* context)
    {}

    void
    updateAttribChannel(const NtString     &paramName, 
                        const NtString     &shape, 
                        const NtString     &attribName,
                        const NtTimeBundle &tb)
    {
        NtString prm = param1s(paramName)->eval(tb);
        if ("" == prm.parent(".")) {
            prm = shape + NtString(".") + prm;
        }

        BodyChannelMap::iterator iter(_bodyChannelMap.find(attribName));
        if (iter != _bodyChannelMap.end()) {
            iter->second.bodyChannelName = prm;
        }
    }

    void
    frameBody(NsBodyObject *nsBody)
    {
        NtVec3f bmin(std::numeric_limits<float>::max(),
                     std::numeric_limits<float>::max(),
                     std::numeric_limits<float>::max());
        NtVec3f bmax(-std::numeric_limits<float>::max(),
                     -std::numeric_limits<float>::max(),
                     -std::numeric_limits<float>::max());

        nsBody->nbBody().bounds(bmin, bmax);

        if (bmin[0] < bmax[0] && bmin[1] < bmax[1] && bmin[2] < bmax[2]) {
            // Valid bounding box.

            const NtVec3f ctr(0.5f*(bmin + bmax));
            const NtVec3f dim(bmax - bmin);

            const QString ctrStr[] = {
                QString::number(ctr[0], 'g', 6),
                QString::number(ctr[1], 'g', 6),
                QString::number(ctr[2], 'g', 6)
            };

            const QString scaleStr[] = {
                QString::number(dim[0], 'g', 6),
                QString::number(dim[1], 'g', 6),
                QString::number(dim[2], 'g', 6)
            };

            const Nb::Value3f *ctrPrm(param3f("Translate"));
            const Nb::Value3f *scalePrm(param3f("Scale"));

            if (fromQStr(ctrStr[0]) != ctrPrm->expr(0).str() ||
                fromQStr(ctrStr[1]) != ctrPrm->expr(1).str() ||
                fromQStr(ctrStr[2]) != ctrPrm->expr(2).str() ||
                fromQStr(scaleStr[0]) != scalePrm->expr(0).str() ||
                fromQStr(scaleStr[1]) != scalePrm->expr(1).str() ||
                fromQStr(scaleStr[2]) != scalePrm->expr(2).str()) {
                setParam3f("Translate", ctr[0], ctr[1], ctr[2]);
                setParam3f("Scale", dim[0], dim[1], dim[2]);
            }
        }
    }

    void
    _drawPoints(const NtTimeBundle &tb, 
                const GLfloat       size, 
                const GLsizei       count,
                const bool          smooth)
    {

        // Setup OpenGL state

        //const GLfloat   prmPointSize(param1f("Pixel Radius")->eval(tb));
        //const bool      prmPointSmooth(false);
        //const NtVec3f prmPointAttenuation(1.f, 0.f, 0.f);

        Ngl::FlipState<GL_POINT_SMOOTH> pointSmoothState;
        Ngl::FlipState<GL_BLEND       > blendState;

        if (smooth) {
            Ngl::FlipState<GL_POINT_SMOOTH>::enable();
            Ngl::FlipState<GL_BLEND       >::enable();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        glPointSize(qMax(1.f, size));
        glDrawArrays(GL_POINTS, 0, count);
    }

    void
    _drawLines(const GLsizei count, GLvoid *indices)
    {
        glDrawRangeElements(GL_LINES,
                            0,
                            count,
                            count,
                            GL_UNSIGNED_INT,
                            indices);
                            //&_lineIndices[0]);
    }

    Ngl::ShaderProgram*
    _getShader(const NtString &shaderName)
    {
        //const NtString prmShader(param1e("Shader")->eval(cvftb));

        Ngl::ShaderProgram* shader(0);
        ShaderMap::iterator shit = _shaderMap.find(shaderName);
        if (_shaderMap.end() != shit) {
            shader = shit->second;
        }

        if (!shader) {
            NB_WARNING("Unknown shader '" << shaderName <<
                       "': defaulting to The Big Blue");
            shader = _shaderMap.find("The Big Blue")->second;
            EM_ASSERT(0 != shader);
        }

        return shader;
    }

    virtual bool
    draw(NsBodyObject          *nsBody,
         const Ns3DCameraScope *cam,
         const Ngl::Viewport   &vp)
    {
        if (0 == cam || 0 == nsBody->ns3DBody()) {
            return true; // Early exit!
        }

        const NtTimeBundle cvftb(queryCurrentVisibleFrameTimeBundle());

        if ("On" == param1e("Frame Bodies")->eval(cvftb)) {
            frameBody(nsBody);
        }

        // Setup OpenGL state

        //const GLfloat   prmPointSize(
        //    param1f("Pixel Radius")->eval(cvftb));
        //const bool      prmPointSmooth(false);
        ////const NtVec3f prmPointAttenuation(1.f, 0.f, 0.f);

        //Ngl::FlipState<GL_POINT_SMOOTH> pointSmoothState;
        //Ngl::FlipState<GL_BLEND       > blendState;

        //if (prmPointSmooth) {
        //    Ngl::FlipState<GL_POINT_SMOOTH>::enable();
        //    Ngl::FlipState<GL_BLEND       >::enable();
        //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //}

        //glPointSize(prmPointSize);
        ////glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,
        ////                   &prmPointAttenuation[0]);




        // Set shader

        Ngl::ShaderProgram* shader(_getShader(param1e("Shader")->eval(cvftb)));

        // Update channel info in attribute mapping. [hard-coded!]
        
        updateAttribChannel("Gradient Channel", "Field", "xgradient", cvftb);
        updateAttribChannel("Color Channel", "Particle", "colorChannel", cvftb);

        //Nb::String gc = param1s("Gradient Channel")->eval(Nb::ZeroTimeBundle);
        //if(gc.parent(".")=="") gc = Nb::String("Field.") + gc;
        //_bodyChannelMap.find("xgradient")->second.bodyChannelName = gc;

        //Nb::String cc = param1s("Color Channel")->eval(Nb::ZeroTimeBundle);
        //if(cc.parent(".")=="") gc = Nb::String("Particle.") + cc;
        //_bodyChannelMap.find("colorChannel")->second.bodyChannelName = cc; 



        // Set shader attributes

        const bool drawLines("On" == param1e("Velocity Vectors")->eval(cvftb));
        const GLsizei minCount(
            connectShaderAttribs(*shader, nsBody, cvftb, drawLines));

        if (drawLines) {
            if (_lineIndices.size() != minCount) {
                _lineIndices.resize(minCount, 0);
                const int halfMinCount(minCount/2);
                for (int i(0); i < halfMinCount; ++i) {
                    _lineIndices[2*i    ] = i;
                    _lineIndices[2*i + 1] = i + halfMinCount;
                }
            }
        }
        else {
            _lineIndices.clear();
        }

        setShaderUniforms(*shader); // Set uniform shader variables.
        shader->use();              // Enable shader
        shader->uploadUniforms(cvftb);

        // Draw points, only draw the first half of VBO's.

        EM_ASSERT(Ngl::Error::check());
        _drawPoints(cvftb, 
                    param1f("Pixel Radius")->eval(cvftb),
                    drawLines ? minCount/2 : minCount,
                    false);
        EM_ASSERT(Ngl::Error::check());

        if (drawLines) {
            // Draw lines.

            if (_lineIndices.size() != minCount) {
                _lineIndices.resize(minCount, 0);
                const int halfMinCount(minCount/2);
                for (int i(0); i < halfMinCount; ++i) {
                    _lineIndices[2*i]     = i;
                    _lineIndices[2*i + 1] = i + halfMinCount;
                }
            }

            _drawLines(minCount, &_lineIndices[0]);
            //glDrawRangeElements(GL_LINES,
            //                    0,
            //                    minCount,
            //                    minCount,
            //                    GL_UNSIGNED_INT,
            //                    &_lineIndices[0]);
        }

        // Disable shader

        shader->unuse();

        disconnectShaderAttribs(*shader);

        /*
        GLuint samplerHandle=
            Ngl::glGetUniformLocationARB(shaderProgram->handle(), "envSampler");
        Ngl::glActiveTexture(GL_TEXTURE0+0);
        glBindTexture(GL_TEXTURE_2D, envMapHandle);
        Ngl::glUniform1i(samplerHandle,0);
        //shaderProgram->setUniform1i(samplerHandle,0);
        */

        ssHud << "Body: '" << fromQStr(nsBody->name())
              << "': " << (drawLines ? minCount/2 : minCount) << " particles\n";

        return true;
    }

protected:

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
                    fromNbStr(name()),
                    paramName,
                    QString::number(value0, 'g', prec),
                    QString::number(value1, 'g', prec),
                    QString::number(value2, 'g', prec)));
    }

    //! TODO: Set NEL context!?
    virtual NtVec3f
    getParam3f(const QString &paramName, const Nb::TimeBundle &tb) const
    {
        const Nb::Value3f *prm(param3f(fromQStr(paramName)));
        return NtVec3f(prm->eval(tb, 0), prm->eval(tb, 1), prm->eval(tb, 2));
    }

private:     // TMP!!

    struct BodyChannel
    {
        BodyChannel(const NtString&  bcn,
                    const NtString&  scn  = NtString(""),
                    const GLboolean  norm = false,
                    const GLenum     use  = GL_STATIC_DRAW)
            : bodyChannelName(bcn),
              sampleChannelName(scn),
              normalized(norm),
              usage(use)
        {}

        NtString bodyChannelName;
        NtString sampleChannelName;
        GLboolean  normalized;
        GLenum     usage;
    };

    // Key: Shader Attribute
    // Val: Nb::Body channel name

    typedef std::map<NtString, BodyChannel> BodyChannelMap;

private:     // Member variables

    typedef std::map<NtString, Ngl::ShaderProgram*> ShaderMap;

    ShaderMap           _shaderMap;
    std::vector<GLuint> _lineIndices;
    NtVec3f             _bodyMin;
    NtVec3f             _bodyMax;

    std::map<NtString, Ngl::VertexBuffer*> _pointVbo;
    std::map<NtString, Ngl::VertexBuffer*> _lineVbo;

    BodyChannelMap _bodyChannelMap;

private:    // Utility functions

    Ngl::VertexBuffer*
    _queryVbo(Ns3DBody       *ns3DBody, 
              const NtString &bodyChannelName, 
              const NtString &sampleChannelName,
              const bool      drawLines)
    {
        Ngl::VertexBuffer *vbo(0);

        if (drawLines) {
            Ngl::VertexBuffer *pointVbo(
                ns3DBody->queryMutableVertexBufferByChannel(
                    bodyChannelName, sampleChannelName));

            if (0 != pointVbo) {
                // We are drawing lines, don't need point VBO.

                ns3DBody->destroyVertexBuffer(
                    bodyChannelName,
                    sampleChannelName);
            }

            // Query the buffer we want.

            vbo = ns3DBody->queryMutableVertexBufferByChannel(
                bodyChannelName + NtString("-line"),
                sampleChannelName);
        }
        else {
            Ngl::VertexBuffer *lineVbo(
                ns3DBody->queryMutableVertexBufferByChannel(
                    bodyChannelName + NtString("-line"), sampleChannelName));

            if (0 != lineVbo) {
                // We are drawing points, don't need line VBO.

                ns3DBody->destroyVertexBuffer(
                    bodyChannelName + NtString("-line"),
                    sampleChannelName);
            }

            // Query the buffer we want.

            vbo = ns3DBody->queryMutableVertexBufferByChannel(
                bodyChannelName,
                sampleChannelName);
        }

        return vbo;
    }

    GLsizei
    connectShaderAttribs(const Ngl::ShaderProgram &shader,
                         NsBodyObject             *nsBody,
                         const NtTimeBundle       &tb,
                         const bool                drawLines)
    {
        if (shader.attribMap().empty()) {
            return 0; // Shader has no inputs!?
        }

        const NtString blockVisibility(param1e("Blocks Visibility")->eval(tb));
        const int startBlock(param1i("Block Range Start")->eval(tb));
        const int endBlock(param1i("Block Range End")->eval(tb));

        const int block0(blockVisibility=="All" ? 0         : startBlock);
        const int block1(blockVisibility=="All" ? 200000000 : endBlock);

        const float uscale(param1f("Velocity Display Scale")->eval(tb));
        const float scaledDt(uscale/evalParam1i("Global.Fps", tb));

        const bool gradLighting("On" == param1e("Gradient Lighting")->eval(tb));

        qDebug() << "Gradient Lighting: " << gradLighting;
        qDebug() << "Draw Lines: " << drawLines;

        Ns3DBody *ns3DBody = nsBody->ns3DBody();

        GLsizei minCount((std::numeric_limits<GLsizei>::max)());

        Ngl::ShaderProgram::AttribMap::const_iterator iter(
            shader.attribMap().begin());
        const Ngl::ShaderProgram::AttribMap::const_iterator iend(
            shader.attribMap().end());
        for (; iter != iend; ++iter) {
            // For each shader attribute.

            Ngl::VertexBuffer *vbo(0);

            switch (iter->second.type()) {
            case GL_FLOAT:
            {
                NB_THROW("Unsupported attribute type: GL_FLOAT");
                break;
            }
            case GL_FLOAT_VEC3:
            {
                // vec3 attribute.

                // Look up body channel for this attribute and see if there
                // is a suitable buffer available already.

                const BodyChannelMap::const_iterator chIter(
                    _bodyChannelMap.find(iter->first));
                
                vbo = _queryVbo(
                    ns3DBody, 
                    chIter->second.bodyChannelName, 
                    chIter->second.sampleChannelName, 
                    drawLines);

                if (0 != vbo) {
                    // VBO exists but may need to be rebuilt.

                    EM_ASSERT(vbo->hasMetaData1i("block0"));
                    EM_ASSERT(vbo->hasMetaData1i("block1"));
                    EM_ASSERT(vbo->hasMetaData1f("scaledDt"));

                    if (vbo->metaData1i("block0") != block0 ||
                        vbo->metaData1i("block1") != block1) {
                        ns3DBody->destroyVertexBuffer(
                            chIter->second.bodyChannelName + 
                                (drawLines ? NtString("-line") : NtString("")),
                            chIter->second.sampleChannelName);
                        vbo = 0; // Create new VBO below.
                    }
                    else if (vbo->metaData1f("scaledDt") != scaledDt) {
                        //qDebug() << scaledDt;
                        ns3DBody->destroyVertexBuffer(
                            chIter->second.bodyChannelName + 
                                (drawLines ? NtString("-line") : NtString("")),
                            chIter->second.sampleChannelName);
                        vbo = 0; // Create new VBO below.
                    }

                    if (!gradLighting && ("xgradient" == iter->first)) {
                        if (drawLines) {
                            nsBody->ns3DBody()->destroyVertexBuffer(
                                chIter->second.bodyChannelName + NtString("-line"),
                                chIter->second.sampleChannelName);
                            vbo =
                                nsBody->ns3DBody()->queryMutableVertexBufferByChannel(
                                "Particle.position-line","");
                        }
                        else {
                            nsBody->ns3DBody()->destroyVertexBuffer(
                                chIter->second.bodyChannelName,
                                chIter->second.sampleChannelName);
                            vbo =
                                nsBody->ns3DBody()->queryMutableVertexBufferByChannel(
                                "Particle.position","");
                        }
                        EM_ASSERT(0 != vbo);
                    }
                }

                if (0 == vbo) {
                    // No suitable VBO available, we need to create one.

                    if (chIter->second.sampleChannelName.empty()) {
                        // No sampling channel provided. Create vertex attribute
                        // from a body channel.
                        
                        switch (ns3DBody->channelType(chIter->second.bodyChannelName)) {
                        case Nb::ValueBase::Vec3fType:// vec3 channel -> vec3 attribute.
                            if (drawLines) {
                                if ("Particle.position" == chIter->second.bodyChannelName) {
                                    
                                    // May throw.
                                    const Nb::ParticleShape& particle(
                                        nsBody->nbBody().constParticleShape()); 
                                    
                                    vbo =
                                        ns3DBody->createParticleLinePositions(
                                            particle.constBlocks3f("position"),
                                            particle.constBlocks3f("velocity"),
                                            scaledDt,
                                            block0,
                                            block1,
                                            chIter->second.usage);
                                } 
                                else {
                                    vbo =
                                        ns3DBody->createChannel3fVertexBufferLine(
                                            chIter->second.bodyChannelName,
                                            block0,
                                            block1,
                                            chIter->second.usage);
                                }
                            }
                            else {
                                vbo =
                                    ns3DBody->createChannel3fVertexBuffer(
                                        chIter->second.bodyChannelName,
                                        block0,
                                        block1,
                                        chIter->second.usage);
                            }
                            break;
                        case Nb::ValueBase::FloatType:
                            if (drawLines) {
                                vbo =
                                    ns3DBody->createChannel3fFrom1fVertexBufferLine(
                                        chIter->second.bodyChannelName,
                                        block0,
                                        block1,
                                        chIter->second.usage);
                            }
                            else {
                                vbo =
                                    ns3DBody->createChannel3fFrom1fVertexBuffer(
                                        chIter->second.bodyChannelName,
                                        block0,
                                        block1,
                                        chIter->second.usage);
                            }
                            break;
                        case Nb::ValueBase::IntType:
                            if (drawLines) {
                                vbo =
                                    ns3DBody->
                                    createChannel3fFrom1iVertexBufferLine(
                                        chIter->second.bodyChannelName,
                                        block0,
                                        block1,
                                        chIter->second.usage);
                            }
                            else {
                                NB_THROW("Unsupported channel type");
                                /*
                                vtxBuf =
                                    ns3DBody->
                                    createChannel3fFrom1iVertexBuffer(
                                        chIter->second.bodyChannelName,
                                        block0,
                                        block1,
                                        chIter->second.usage);
                                        */
                            }
                            break;
                        case Nb::ValueBase::Int64Type:
                            if (drawLines) {
                                vbo =
                                    ns3DBody->createChannel3fFrom1i64VertexBufferLine(
                                        chIter->second.bodyChannelName,
                                        block0,
                                        block1,
                                        chIter->second.usage);
                            }
                            else {
                                vbo =
                                    ns3DBody->createChannel3fFrom1i64VertexBuffer(
                                        chIter->second.bodyChannelName,
                                        block0,
                                        block1,
                                        chIter->second.usage);
                            }
                            break;
                        default:
                            NB_THROW("Unsupported channel type");
                        }
                    }
                    else {
                        // Sampling channel provided. Sample a body channel
                        // at locations given in sampling channel.

                        if (gradLighting) {
                            if (drawLines) {
                                vbo =
                                    ns3DBody->sampleChannel3fVertexBufferLine(
                                        chIter->second.bodyChannelName,
                                        chIter->second.sampleChannelName,
                                        block0,
                                        block1,
                                        chIter->second.usage);
                            }
                            else {
                                vbo =
                                    ns3DBody->sampleChannel3fVertexBuffer(
                                        chIter->second.bodyChannelName,
                                        chIter->second.sampleChannelName,
                                        block0,
                                        block1,
                                        chIter->second.usage);
                            }
                        }
                    }

                    if(vbo) {
                        vbo->attachMetaData1i("block0", block0);
                        vbo->attachMetaData1i("block1", block1);
                        vbo->attachMetaData1f("scaledDt", scaledDt);
                    }
                }
                break;
            }
            default:
            {
                NB_THROW("Unsupported attribute type: " << iter->second.type());
            }
            }

            if (vbo) {
                const GLsizei count(
                    Ngl::VertexAttrib::connect(iter->second, *vbo));
                minCount = std::min(minCount, count);
            }
        }

        return minCount;

#if 0
        std::cerr   << "Attrib Query:\n"
                    << "Name: '" << iter->first << "'\n"
                    << "Type: " << Ngl::GLType::str(iter->second.type()) << "\n"
                    << "Size: " << iter->second.size() << "\n"
                    << "Location: " << iter->second.location() << "\n"
                    << "Count: " << count << "\n\n";
#endif
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
    setShaderUniforms(Ngl::ShaderProgram& shader)
    {
        // Compute clip-box matrices.

        em::glmat44f clipXform;
        em::glmat44f invClipXform;
        computeClipBoxXforms(
            param3f("Translate"),
            param3f("Rotate"),
            param3f("Scale"),
            clipXform,
            invClipXform);

        em::glmat44f modelViewXform;
        em::glmat44f projectionXform;

        //int cvf;
        //queryCurrentVisibleFrame(&cvf);
        //std::cerr << "Frame: " << cvf << "\n";
        //std::cerr << invClipXform << "\n";

//Frame: 186
// 0.00337276  0          -0.000479562  -7.85714
//-0.000274658 0.00925926 -6.86646e-005 -1.125
//-0.000366211 0           0.00402367   -1.5
// 0.000244141 0           6.10352e-005  1
//
//
//Frame: 187
//0.00493827 0          -0.000449626  -7.36667
//0          0.00925926 -6.86646e-005 -1.125
//0          0           0.00402367   -1.5
//0          0           6.10352e-005  1


        glGetFloatv(GL_PROJECTION_MATRIX, &projectionXform[0][0]);
        glGetFloatv(GL_MODELVIEW_MATRIX, &modelViewXform[0][0]);

        shader.storeUniform4m("modelview",  &modelViewXform[0][0]);
        shader.storeUniform4m("projection", &projectionXform[0][0]);
        shader.storeUniform4m("worldToBox", &invClipXform[0][0]);

        // Read values from GUI

        const bool prmGradientLighting(
            param1e("Gradient Lighting")->eval(Nb::ZeroTimeBundle) == "On"
            );
        const bool prmNormalizedGrad(
            param1e("Normalized Gradient")->eval(Nb::ZeroTimeBundle) == "On"
            );
        const bool prmScalarChannel(
            param1e("Channel Type")->eval(Nb::ZeroTimeBundle) == "Scalar"
            );
        const GLfloat prmAmbientBrightness(
            param1f("Ambient Brightness")->eval(Nb::ZeroTimeBundle)
            );
        const NtVec3f prmLightDirection(
            em::normalized(
                NtVec3f(
                    param3f("Light Direction")->eval(Nb::ZeroTimeBundle,0),
                    param3f("Light Direction")->eval(Nb::ZeroTimeBundle,1),
                    param3f("Light Direction")->eval(Nb::ZeroTimeBundle,2))
                )
            );
        const GLfloat prmWhitewaterSpeed(
            param1f("Whitewater Speed")->eval(Nb::ZeroTimeBundle)
            );
        const GLfloat prmMinMag(
            param1f("Min Visible Magnitude")->eval(Nb::ZeroTimeBundle)
            );
        const GLfloat prmMaxMag(
            param1f("Max Visible Magnitude")->eval(Nb::ZeroTimeBundle)
            );
        const GLint prmNormalizedRange(
            param1e("Normalized Range")->eval(Nb::ZeroTimeBundle)=="On"
            );

        // Pass values to shader

        shader.storeUniform1i("gradLighting",     prmGradientLighting ? 1 : 0);
        shader.storeUniform1i("normGrad",         prmNormalizedGrad ? 1 : 0);
        shader.storeUniform1i("scalarChannel",    prmScalarChannel);
        shader.storeUniform1f("ambient",          prmAmbientBrightness);
        shader.storeUniform3f("lightDir",        -prmLightDirection);
        shader.storeUniform1f("whiteWaterSpeed",  prmWhitewaterSpeed);
        shader.storeUniform1f("minMag",           prmMinMag);
        shader.storeUniform1f("maxMag",           prmMaxMag);
        shader.storeUniform1i("normalizedRange",  prmNormalizedRange);
    }

};










//if (chIter->second.bodyChannelName == "Particle/position") {
//    // HACK!!
//    bool ok;
//    const float uscale =
//        param1f("Velocity Scale")->eval(
//            Nb::ZeroTimeBundle
//            );
//    const float scaled_dt=
//        uscale/Ns::opParameter(
//            "Global","Fps"
//            ).toInt(&ok);

//    const Nb::ParticleShape& particle(
//        nsBody->nbBody()->constParticleShape()); // May throw

//    const em::block3_array3f& position3f(
//        particle.constBlocks3f("position"));
//    const em::block3_array3f& velocity3f(
//        particle.constBlocks3f("velocity"));

//    std::vector<em::vec<3,GLfloat> > positions;
//    //positions.reserve(2*position3f);

//    for (int b(0); b < position3f.block_count(); ++b) {
//        const em::block3vec3f &pos(position3f(b));
//        const em::block3vec3f &vel(velocity3f(b));
//        for (int p(0); p < pos.size(); ++p) {
//            // HACK! Create to positions for each particle,
//            // one for the particle and one for particle + velocity.
//            positions.push_back(
//                pos(p)
//                );
//            positions.push_back(
//                pos(p) + scaled_dt*vel(p)
//                );
//        }
//    }

//// HACK: Hard-coded!
////static const NtString vtxBufName("velocity-position");

//// HACK: Must be destroyed before created?!?!
//nsBody->ns3DBody()->destroyVertexBuffer(
//        "SHARED",
//        chIter->second.bodyChannelName);

//vtxBuf
//    = nsBody->ns3DBody()->createVertexBuffer(
//        "SHARED",
//        chIter->second.bodyChannelName,//vtxBufName,
//        sizeof(em::vec<3,GLfloat>)*positions.size(),
//        &positions[0]
//        );
//}
//else {












// -----------------------------------------------------------------------------
