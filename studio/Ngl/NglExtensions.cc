/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "NglExtensions.h"

#include <iostream>

namespace Ngl
{
#if 1
// -----------------------------------------------------------------------------

#define RESOLVE_GL_FUNC(f) { \
ok &= bool((f = (_gl##f) context->getProcAddress(QLatin1String("gl" #f)))); \
if(!ok) std::cerr << "ERROR BINDING gl " << #f << std::endl; }

bool
GLExtensionFunctions::resolve(const QGLContext *context)
{
    bool ok = true;

    //---------------------
    // Vertex Specification
    //---------------------

    RESOLVE_GL_FUNC(VertexAttrib3fv)

    //-------------------
    // Shaders & Programs
    //-------------------

    // Shader Objects

    RESOLVE_GL_FUNC(CreateShader)
    RESOLVE_GL_FUNC(ShaderSource)
    RESOLVE_GL_FUNC(CompileShader)
    RESOLVE_GL_FUNC(DeleteShader)

    // Program Objects

    RESOLVE_GL_FUNC(CreateProgram)
    RESOLVE_GL_FUNC(AttachShader)
    RESOLVE_GL_FUNC(DetachShader)
    RESOLVE_GL_FUNC(LinkProgram)
    RESOLVE_GL_FUNC(UseProgram)
    RESOLVE_GL_FUNC(DeleteProgram)

    // Vertex Attributes

    RESOLVE_GL_FUNC(GetActiveAttrib)
    RESOLVE_GL_FUNC(GetAttribLocation)
    RESOLVE_GL_FUNC(BindAttribLocation)

    // Uniform Variables

    RESOLVE_GL_FUNC(GetUniformLocation)
//    RESOLVE_GL_FUNC(GetActiveUniformName)
    RESOLVE_GL_FUNC(GetActiveUniform)
//    RESOLVE_GL_FUNC(GetActiveUniformsiv)
    RESOLVE_GL_FUNC(Uniform1i)
    RESOLVE_GL_FUNC(Uniform1f)
//    RESOLVE_GL_FUNC(Uniform1ui)
    RESOLVE_GL_FUNC(Uniform3i)
    RESOLVE_GL_FUNC(Uniform3f)
    RESOLVE_GL_FUNC(Uniform4i)
    RESOLVE_GL_FUNC(Uniform4f)
    RESOLVE_GL_FUNC(Uniform4iv)
    RESOLVE_GL_FUNC(Uniform4fv)
    RESOLVE_GL_FUNC(UniformMatrix3fv)
    RESOLVE_GL_FUNC(UniformMatrix4fv)

    // Shader Execution

    RESOLVE_GL_FUNC(ValidateProgram)

    // Shader Queries

    RESOLVE_GL_FUNC(IsShader)
    RESOLVE_GL_FUNC(GetShaderiv)
    RESOLVE_GL_FUNC(GetAttachedShaders)
    RESOLVE_GL_FUNC(GetShaderInfoLog)
    RESOLVE_GL_FUNC(GetShaderSource)

    // Program Queries

    RESOLVE_GL_FUNC(IsProgram)
    RESOLVE_GL_FUNC(GetProgramiv)
    RESOLVE_GL_FUNC(GetProgramInfoLog)

    //-------------------

    //---------------
    // Buffer Objects
    //---------------

    // Buffer Objects

    RESOLVE_GL_FUNC(GenBuffers)
    RESOLVE_GL_FUNC(DeleteBuffers)
    RESOLVE_GL_FUNC(BindBuffer)
    RESOLVE_GL_FUNC(BufferData)
    RESOLVE_GL_FUNC(BufferSubData)
    RESOLVE_GL_FUNC(MapBuffer)
    RESOLVE_GL_FUNC(UnmapBuffer)
    RESOLVE_GL_FUNC(IsBuffer)
    RESOLVE_GL_FUNC(GetBufferParameteriv)
    RESOLVE_GL_FUNC(GetBufferSubData)

    // Vertex Arrays.

    RESOLVE_GL_FUNC(DrawRangeElements)

    // Vertex Array Objects

    //RESOLVE_GL_FUNC(GenVertexArrays)
    //RESOLVE_GL_FUNC(DeleteVertexArrays)
    //RESOLVE_GL_FUNC(BindVertexArray)
    //RESOLVE_GL_FUNC(IsVertexArray)

    //---------------


    RESOLVE_GL_FUNC(GenFramebuffersEXT)
    RESOLVE_GL_FUNC(GenRenderbuffersEXT)
    RESOLVE_GL_FUNC(BindRenderbufferEXT)
    RESOLVE_GL_FUNC(RenderbufferStorageEXT)
    RESOLVE_GL_FUNC(DeleteFramebuffersEXT)
    RESOLVE_GL_FUNC(DeleteRenderbuffersEXT)
    RESOLVE_GL_FUNC(BindFramebufferEXT)
    RESOLVE_GL_FUNC(FramebufferTexture2DEXT)
    RESOLVE_GL_FUNC(FramebufferRenderbufferEXT)
    RESOLVE_GL_FUNC(CheckFramebufferStatusEXT)

    RESOLVE_GL_FUNC(EnableVertexAttribArray)
    RESOLVE_GL_FUNC(DisableVertexAttribArray)
    RESOLVE_GL_FUNC(VertexAttribPointer)

    RESOLVE_GL_FUNC(ActiveTexture)
    RESOLVE_GL_FUNC(DrawBuffers)
    RESOLVE_GL_FUNC(TexImage3D)

    return ok;
}


bool
GLExtensionFunctions::glslSupported() {
    return (
        CreateShader         &&      // Shader objects
        ShaderSource         &&
        CompileShader        &&
        DeleteShader         &&
        CreateProgram        &&      // Program objects
        AttachShader         &&
        DetachShader         &&
        LinkProgram          &&
        UseProgram           &&
        DeleteProgram        &&
        GetActiveAttrib      &&      // Vertex attributes
        GetAttribLocation    &&
        BindAttribLocation   &&
        GetUniformLocation   &&      // Uniform variables
//        GetActiveUniformName &&
        GetActiveUniform     &&
//        GetActiveUniformsiv  &&
        Uniform1i            &&
        Uniform1f            &&
//        Uniform1ui           &&
        Uniform3i            &&
        Uniform3f            &&
        Uniform4i            &&
        Uniform4f            &&
        Uniform4iv           &&
        Uniform4fv           &&
        UniformMatrix3fv     &&
        UniformMatrix4fv     &&
        ValidateProgram      &&      // Shader execution
        IsShader             &&      // Shader queries
        GetShaderiv          &&
        GetAttachedShaders   &&
        GetShaderInfoLog     &&
        GetShaderSource      &&
        IsProgram            &&      // Program queries
        GetProgramiv         &&
        GetProgramInfoLog
    );
}


bool GLExtensionFunctions::fboSupported() {
    return GenFramebuffersEXT
        && GenRenderbuffersEXT
        && BindRenderbufferEXT
        && RenderbufferStorageEXT
        && DeleteFramebuffersEXT
        && DeleteRenderbuffersEXT
        && BindFramebufferEXT
        && FramebufferTexture2DEXT
        && FramebufferRenderbufferEXT
        && CheckFramebufferStatusEXT;
}

bool GLExtensionFunctions::openGL15Supported() {
    return ActiveTexture
        && TexImage3D
            && GenBuffers
        && BindBuffer
        && BufferData
            && BufferSubData
        && DeleteBuffers
        && MapBuffer
        && UnmapBuffer;
}

#undef RESOLVE_GL_FUNC

#endif

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.
