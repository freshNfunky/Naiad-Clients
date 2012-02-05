// -----------------------------------------------------------------------------
//
// NglExtensions.h
//
// Class for loading OpenGL extensions.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
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

#ifndef NGL_EXTENSIONS_H
#define NGL_EXTENSIONS_H

#include "NglInclude.h"
#include "NglTypes.h"

// -----------------------------------------------------------------------------

// Note: It has been found that mixing ARB with non-ARB function calls
//       sometimes causes errors. Therefore, stick with the non-ARB versions.

#ifndef Q_WS_MAC
# ifndef APIENTRYP
#   ifdef APIENTRY
#     define APIENTRYP APIENTRY *
#   else
#     define APIENTRY
#     define APIENTRYP *
#   endif
# endif
#else
# define APIENTRY
# define APIENTRYP *
#endif

#ifndef GL_VERSION_1_2
#define GL_TEXTURE_3D     0x806F
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_CLAMP_TO_EDGE  0x812F
#define GL_BGRA           0x80E1
#endif

#ifndef GL_VERSION_1_3
#define GL_TEXTURE0                    0x84C0
#define GL_TEXTURE1                    0x84C1
#define GL_TEXTURE2                    0x84C2
#define GL_TEXTURE_CUBE_MAP            0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
//#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
//#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
//#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
//#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
//#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#endif

#ifndef GL_VERSION_1_5
typedef ptrdiff_t GLsizeiptr;

#define GL_ARRAY_BUFFER         0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_READ_WRITE           0x88BA
#define GL_STATIC_DRAW          0x88E4
#endif

#ifndef GL_EXT_framebuffer_object
#define GL_RENDERBUFFER_EXT         0x8D41
#define GL_FRAMEBUFFER_EXT          0x8D40
#define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5
#define GL_COLOR_ATTACHMENT0_EXT    0x8CE0
#define GL_DEPTH_ATTACHMENT_EXT     0x8D00
#endif

#ifndef GL_ARB_vertex_shader
#define GL_VERTEX_SHADER_ARB 0x8B31
#endif

#ifndef GL_ARB_fragment_shader
#define GL_FRAGMENT_SHADER_ARB 0x8B30
#endif

#ifndef GL_ARB_shader_objects
typedef char GLcharARB;
typedef unsigned int GLhandleARB;
#define GL_OBJECT_COMPILE_STATUS_ARB  0x8B81
#define GL_OBJECT_LINK_STATUS_ARB     0x8B82
#define GL_OBJECT_INFO_LOG_LENGTH_ARB 0x8B84
#endif

// -----------------------------------------------------------------------------

// Function prototypes

//---------------------
// Vertex Specification
//---------------------

typedef GLvoid (APIENTRY *_glVertexAttrib3fv) (GLuint, const GLfloat *);

//-------------------
// Shaders & Programs
//-------------------

// Shader Objects

typedef GLuint (APIENTRY *_glCreateShader) (GLuint);
typedef void   (APIENTRY *_glShaderSource) (GLuint, GLsizei, const GLchar **, GLint *);
typedef void   (APIENTRY *_glCompileShader) (GLuint);
typedef void   (APIENTRY *_glDeleteShader) (GLuint);

// Program Objects

typedef GLuint (APIENTRY *_glCreateProgram) ();
typedef void   (APIENTRY *_glAttachShader) (GLuint, GLuint);
typedef void   (APIENTRY *_glDetachShader) (GLuint, GLuint);
typedef void   (APIENTRY *_glLinkProgram) (GLuint);
typedef void   (APIENTRY *_glUseProgram) (GLuint);
typedef void   (APIENTRY *_glDeleteProgram) (GLuint);

// Vertex Attributes

typedef void  (APIENTRY *_glGetActiveAttrib) (GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *);
typedef GLint (APIENTRY *_glGetAttribLocation) (GLuint, const GLchar *);
typedef void  (APIENTRY *_glBindAttribLocation) (GLuint, GLuint, const GLchar *);

// Uniform Variables

typedef GLint (APIENTRY *_glGetUniformLocation) (GLuint, const GLchar *);
//typedef void  (APIENTRY *_glGetActiveUniformName) (GLuint, GLuint, GLsizei, GLsizei *, GLchar *);
typedef void  (APIENTRY *_glGetActiveUniform) (GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *);
//typedef void  (APIENTRY *_glGetActiveUniformsiv) (GLuint, GLsizei, const GLuint *, GLenum, GLint *);
typedef void  (APIENTRY *_glUniform1i) (GLint, GLint);
typedef void  (APIENTRY *_glUniform1f) (GLint, GLfloat);
//typedef void  (APIENTRY *_glUniform1ui) (GLint, GLuint);
typedef void  (APIENTRY *_glUniform3i) (GLint, GLint, GLint, GLint);
typedef void  (APIENTRY *_glUniform3f) (GLint, GLfloat, GLfloat, GLfloat);
typedef void  (APIENTRY *_glUniform4i) (GLint, GLint, GLint, GLint, GLint);
typedef void  (APIENTRY *_glUniform4f) (GLint, GLfloat, GLfloat, GLfloat, GLfloat);
typedef void  (APIENTRY *_glUniform4iv) (GLint, GLsizei, const GLint *);
typedef void  (APIENTRY *_glUniform4fv) (GLint, GLsizei, const GLfloat *);
typedef void  (APIENTRY *_glUniformMatrix3fv) (GLint, GLuint, GLboolean, const GLfloat *);
typedef void  (APIENTRY *_glUniformMatrix4fv) (GLint, GLuint, GLboolean, const GLfloat *);

// Shader Execution

typedef void (APIENTRY *_glValidateProgram) (GLuint);

// Shader Queries

typedef GLboolean (APIENTRY *_glIsShader) (GLuint);
typedef void      (APIENTRY *_glGetShaderiv) (GLuint, GLenum, GLint *);
typedef void      (APIENTRY *_glGetAttachedShaders) (GLuint, GLsizei, GLsizei *, GLuint *);
typedef void      (APIENTRY *_glGetShaderInfoLog) (GLuint, GLsizei, GLsizei *, GLchar *);
typedef void      (APIENTRY *_glGetShaderSource) (GLuint, GLsizei, GLsizei *, GLchar *);

// Program Queries.

typedef GLboolean (APIENTRY *_glIsProgram) (GLuint);
typedef void      (APIENTRY *_glGetProgramiv) (GLuint, GLenum, GLint *);
typedef void      (APIENTRY *_glGetProgramInfoLog) (GLuint, GLsizei, GLsizei *, GLchar *);

// Buffer Objects.

typedef void      (APIENTRY *_glGenBuffers) (GLsizei, GLuint *);
typedef void      (APIENTRY *_glDeleteBuffers) (GLsizei, const GLuint *);
typedef void      (APIENTRY *_glBindBuffer) (GLenum, GLuint);
typedef void      (APIENTRY *_glBufferData) (GLenum, GLsizeiptr, const GLvoid *, GLenum);
typedef void      (APIENTRY *_glBufferSubData) (GLenum, GLintptr, GLsizeiptr, const GLvoid *);
typedef void*     (APIENTRY *_glMapBuffer) (GLenum, GLenum);
typedef GLboolean (APIENTRY *_glUnmapBuffer) (GLenum);
typedef GLboolean (APIENTRY *_glIsBuffer) (GLuint);
typedef void      (APIENTRY *_glGetBufferParameteriv) (GLenum, GLenum, GLint *);
typedef void      (APIENTRY *_glGetBufferSubData) (GLenum, GLintptr, GLsizeiptr, GLvoid*);

// Vertex Arrays.

typedef void (APIENTRY *_glDrawRangeElements)(GLenum, GLuint, GLuint, GLsizei, GLenum, GLvoid*);

// Vertex Array Objects

//typedef void      (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint *);
//typedef void      (APIENTRY *_glDeleteVertexArrays) (GLsizei, const GLuint *);
//typedef void      (APIENTRY *_glBindVertexArray) (GLuint);
//typedef GLboolean (APIENTRY *_glIsVertexArray) (GLuint);

//---------------



// TODO: Sort extensions below by functionality...

typedef void        (APIENTRY *_glGenFramebuffersEXT) (GLsizei, GLuint *);
typedef void        (APIENTRY *_glGenRenderbuffersEXT) (GLsizei, GLuint *);
typedef void        (APIENTRY *_glBindRenderbufferEXT) (GLenum, GLuint);
typedef void        (APIENTRY *_glRenderbufferStorageEXT) (GLenum, GLenum, GLsizei, GLsizei);
typedef void        (APIENTRY *_glDeleteFramebuffersEXT) (GLsizei, const GLuint*);
typedef void        (APIENTRY *_glDeleteRenderbuffersEXT) (GLsizei, const GLuint*);
typedef void        (APIENTRY *_glBindFramebufferEXT) (GLenum, GLuint);
typedef void        (APIENTRY *_glFramebufferTexture2DEXT) (GLenum, GLenum, GLenum, GLuint, GLint);
typedef void        (APIENTRY *_glFramebufferRenderbufferEXT) (GLenum, GLenum, GLenum, GLuint);
typedef GLenum      (APIENTRY *_glCheckFramebufferStatusEXT) (GLenum);


typedef void        (APIENTRY *_glEnableVertexAttribArray) (GLuint);
typedef void        (APIENTRY *_glDisableVertexAttribArray) (GLuint);
typedef void        (APIENTRY *_glVertexAttribPointer) (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);

typedef void        (APIENTRY *_glActiveTexture) (GLenum);
typedef void        (APIENTRY *_glDrawBuffers) (GLsizei, const GLenum *);
typedef void        (APIENTRY *_glTexImage3D) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);

// -----------------------------------------------------------------------------

namespace Ngl
{

struct GLExtensionFunctions
{
public:

    bool resolve(const QGLContext *context);

    bool
    glslSupported();

    bool
    fboSupported();

    bool
    openGL15Supported(); // the rest: multi-texture, 3D-texture, VBOs

public: // Member variables.

    //---------------------
    // Vertex Specification
    //---------------------

    _glVertexAttrib3fv VertexAttrib3fv;

    //-------------------
    // Shaders & Programs
    //-------------------

    // Shader Objects

    _glCreateShader  CreateShader;
    _glShaderSource  ShaderSource;
    _glCompileShader CompileShader;
    _glDeleteShader  DeleteShader;

    // Program Objects

    _glCreateProgram CreateProgram;
    _glAttachShader  AttachShader;
    _glDetachShader  DetachShader;
    _glLinkProgram   LinkProgram;
    _glUseProgram    UseProgram;
    _glDeleteProgram DeleteProgram;

    // Vertex Attributes

    _glGetActiveAttrib    GetActiveAttrib;
    _glGetAttribLocation  GetAttribLocation;
    _glBindAttribLocation BindAttribLocation;

    // Uniform Variables

    _glGetUniformLocation   GetUniformLocation;
//    _glGetActiveUniformName GetActiveUniformName;
    _glGetActiveUniform     GetActiveUniform;
//    _glGetActiveUniformsiv  GetActiveUniformsiv;
    _glUniform1i            Uniform1i;
    _glUniform1f            Uniform1f;
//    _glUniform1ui           Uniform1ui;
    _glUniform3i            Uniform3i;
    _glUniform3f            Uniform3f;
    _glUniform4i            Uniform4i;
    _glUniform4f            Uniform4f;
    _glUniform4iv           Uniform4iv;
    _glUniform4fv           Uniform4fv;
    _glUniformMatrix3fv     UniformMatrix3fv;
    _glUniformMatrix4fv     UniformMatrix4fv;

    // Shader Execution

    _glValidateProgram ValidateProgram;

    // Shader Queries

    _glIsShader           IsShader;
    _glGetShaderiv        GetShaderiv;
    _glGetAttachedShaders GetAttachedShaders;
    _glGetShaderInfoLog   GetShaderInfoLog;
    _glGetShaderSource    GetShaderSource;

    // Program Queries

    _glIsProgram         IsProgram;
    _glGetProgramiv      GetProgramiv;
    _glGetProgramInfoLog GetProgramInfoLog;

    //-------------------

    //---------------
    // Buffer Objects
    //---------------

    // Buffer Objects

    _glGenBuffers           GenBuffers;
    _glDeleteBuffers        DeleteBuffers;
    _glBindBuffer           BindBuffer;
    _glBufferData           BufferData;
    _glBufferSubData        BufferSubData;
    _glMapBuffer            MapBuffer;
    _glUnmapBuffer          UnmapBuffer;
    _glIsBuffer             IsBuffer;
    _glGetBufferParameteriv GetBufferParameteriv;
    _glGetBufferSubData     GetBufferSubData;

    // Vertex Array Objects

    //_glGenVertexArrays    GenVertexArrays;
    //_glDeleteVertexArrays DeleteVertexArrays;
    //_glBindVertexArray    BindVertexArray;
    //_glIsVertexArray      IsVertexArray;

    //---------------

    // Vertex Arrays

    _glDrawRangeElements    DrawRangeElements;

    // etc.

    _glGenFramebuffersEXT GenFramebuffersEXT;
    _glGenRenderbuffersEXT GenRenderbuffersEXT;
    _glBindRenderbufferEXT BindRenderbufferEXT;
    _glRenderbufferStorageEXT RenderbufferStorageEXT;
    _glDeleteFramebuffersEXT DeleteFramebuffersEXT;
    _glDeleteRenderbuffersEXT DeleteRenderbuffersEXT;
    _glBindFramebufferEXT BindFramebufferEXT;
    _glFramebufferTexture2DEXT FramebufferTexture2DEXT;
    _glFramebufferRenderbufferEXT FramebufferRenderbufferEXT;
    _glCheckFramebufferStatusEXT CheckFramebufferStatusEXT;

    _glEnableVertexAttribArray  EnableVertexAttribArray;
    _glDisableVertexAttribArray DisableVertexAttribArray;
    _glVertexAttribPointer      VertexAttribPointer;

    _glActiveTexture ActiveTexture;
    _glDrawBuffers   DrawBuffers;
    _glTexImage3D TexImage3D;
};

inline GLExtensionFunctions&
getGLExtensionFunctions()
{
    static GLExtensionFunctions funcs;
    return funcs;
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.


//---------------------
// Vertex Specification
//---------------------

#define glVertexAttrib3fv  Ngl::getGLExtensionFunctions().VertexAttrib3fv

//-------------------
// Shaders & Programs
//-------------------

// Shader Objects

#define glCreateShader  Ngl::getGLExtensionFunctions().CreateShader
#define glShaderSource  Ngl::getGLExtensionFunctions().ShaderSource
#define glCompileShader Ngl::getGLExtensionFunctions().CompileShader
#define glDeleteShader  Ngl::getGLExtensionFunctions().DeleteShader

// Program Objects

#define glCreateProgram Ngl::getGLExtensionFunctions().CreateProgram
#define glAttachShader  Ngl::getGLExtensionFunctions().AttachShader
#define glDetachShader  Ngl::getGLExtensionFunctions().DetachShader
#define glLinkProgram   Ngl::getGLExtensionFunctions().LinkProgram
#define glUseProgram    Ngl::getGLExtensionFunctions().UseProgram
#define glDeleteProgram Ngl::getGLExtensionFunctions().DeleteProgram

// Vertex Attributes

#define glGetActiveAttrib    Ngl::getGLExtensionFunctions().GetActiveAttrib
#define glGetAttribLocation  Ngl::getGLExtensionFunctions().GetAttribLocation
#define glBindAttribLocation Ngl::getGLExtensionFunctions().BindAttribLocation

// Uniform Variables

#define glGetUniformLocation   Ngl::getGLExtensionFunctions().GetUniformLocation
//#define glGetActiveUniformName Ngl::getGLExtensionFunctions().GetActiveUniformName
#define glGetActiveUniform     Ngl::getGLExtensionFunctions().GetActiveUniform
//#define glGetActiveUniformsiv  Ngl::getGLExtensionFunctions().GetActiveUniformsiv
#define glUniform1i            Ngl::getGLExtensionFunctions().Uniform1i
#define glUniform1f            Ngl::getGLExtensionFunctions().Uniform1f
//#define glUniform1ui           Ngl::getGLExtensionFunctions().Uniform1ui
#define glUniform3i            Ngl::getGLExtensionFunctions().Uniform3i
#define glUniform3f            Ngl::getGLExtensionFunctions().Uniform3f
#define glUniform4i            Ngl::getGLExtensionFunctions().Uniform4i
#define glUniform4f            Ngl::getGLExtensionFunctions().Uniform4f
#define glUniform4iv           Ngl::getGLExtensionFunctions().Uniform4iv
#define glUniform4fv           Ngl::getGLExtensionFunctions().Uniform4fv
#define glUniformMatrix3fv     Ngl::getGLExtensionFunctions().UniformMatrix3fv
#define glUniformMatrix4fv     Ngl::getGLExtensionFunctions().UniformMatrix4fv

// Shader Execution

#define glValidateProgram Ngl::getGLExtensionFunctions().ValidateProgram

// Shader Queries

#define glIsShader           Ngl::getGLExtensionFunctions().IsShader
#define glGetShaderiv        Ngl::getGLExtensionFunctions().GetShaderiv
#define glGetAttachedShaders Ngl::getGLExtensionFunctions().GetAttachedShaders
#define glGetShaderInfoLog   Ngl::getGLExtensionFunctions().GetShaderInfoLog
#define glGetShaderSource    Ngl::getGLExtensionFunctions().GetShaderSource

// Program Queries

#define glIsProgram         Ngl::getGLExtensionFunctions().IsProgram
#define glGetProgramiv      Ngl::getGLExtensionFunctions().GetProgramiv
#define glGetProgramInfoLog Ngl::getGLExtensionFunctions().GetProgramInfoLog

//-------------------

// Buffer Objects

#define glGenBuffers           Ngl::getGLExtensionFunctions().GenBuffers
#define glDeleteBuffers        Ngl::getGLExtensionFunctions().DeleteBuffers
#define glBindBuffer           Ngl::getGLExtensionFunctions().BindBuffer
#define glBufferData           Ngl::getGLExtensionFunctions().BufferData
#define glBufferSubData        Ngl::getGLExtensionFunctions().BufferSubData
#define glMapBuffer            Ngl::getGLExtensionFunctions().MapBuffer
#define glUnmapBuffer          Ngl::getGLExtensionFunctions().UnmapBuffer
#define glIsBuffer             Ngl::getGLExtensionFunctions().IsBuffer
#define glGetBufferParameteriv Ngl::getGLExtensionFunctions().GetBufferParameteriv
#define glGetBufferSubData     Ngl::getGLExtensionFunctions().GetBufferSubData

// Vertex Arrays.

#define glDrawRangeElements    Ngl::getGLExtensionFunctions().DrawRangeElements

// Vertex Array Objects

//#define glGenVertexArrays    Ngl::getGLExtensionFunctions().GenVertexArrays
//#define glDeleteVertexArrays Ngl::getGLExtensionFunctions().DeleteVertexArrays
//#define glBindVertexArray    Ngl::getGLExtensionFunctions().BindVertexArray
//#define glIsVertexArray      Ngl::getGLExtensionFunctions().IsVertexArray

//---------------


// TODO: sort the extensions below by functionality...

#define glGenFramebuffersEXT Ngl::getGLExtensionFunctions().GenFramebuffersEXT
#define glGenRenderbuffersEXT Ngl::getGLExtensionFunctions().GenRenderbuffersEXT
#define glBindRenderbufferEXT Ngl::getGLExtensionFunctions().BindRenderbufferEXT
#define glRenderbufferStorageEXT Ngl::getGLExtensionFunctions().RenderbufferStorageEXT
#define glDeleteFramebuffersEXT Ngl::getGLExtensionFunctions().DeleteFramebuffersEXT
#define glDeleteRenderbuffersEXT Ngl::getGLExtensionFunctions().DeleteRenderbuffersEXT
#define glBindFramebufferEXT Ngl::getGLExtensionFunctions().BindFramebufferEXT
#define glFramebufferTexture2DEXT Ngl::getGLExtensionFunctions().FramebufferTexture2DEXT
#define glFramebufferRenderbufferEXT Ngl::getGLExtensionFunctions().FramebufferRenderbufferEXT
#define glCheckFramebufferStatusEXT Ngl::getGLExtensionFunctions().CheckFramebufferStatusEXT

#define glEnableVertexAttribArray Ngl::getGLExtensionFunctions().EnableVertexAttribArray
#define glDisableVertexAttribArray Ngl::getGLExtensionFunctions().DisableVertexAttribArray
#define glVertexAttribPointer Ngl::getGLExtensionFunctions().VertexAttribPointer

#define glActiveTexture Ngl::getGLExtensionFunctions().ActiveTexture
#define glDrawBuffers   Ngl::getGLExtensionFunctions().DrawBuffers
#define glTexImage3D Ngl::getGLExtensionFunctions().TexImage3D

#endif  // NGL_EXTENSIONS_H
