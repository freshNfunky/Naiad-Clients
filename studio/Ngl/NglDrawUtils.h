// -----------------------------------------------------------------------------
//
// NglDrawUtils.h
//
// Drawing Utility functions.
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

#ifndef NGL_TEXTURE_UTILS_H
#define NGL_TEXTURE_UTILS_H

#include "NglTexture3D.h"

namespace Ngl
{

// -----------------------------------------------------------------------------

inline void
drawTexture3DDeltas(const NtVec3i&                 texDim,
                    const em::vec<3,GLfloat>&        wsMin,
                    const em::vec<3,GLfloat>&        wsMax,
                    em::array3<em::vec<4,GLfloat> >& texData)
{
    // Reset and allocate memory for texture data. Texture dimensions must
    // be at least 2x2x2.

    texData.resize(std::max(2, texDim[0]),
                   std::max(2, texDim[1]),
                   std::max(2, texDim[2]));

#if 0
    std::cerr << "Tex Dim: "
            << texData.ni << "x"
            << texData.nj << "x"
            << texData.nk << "\n";
#endif

    // We sample along the edges of the tile and internally. This means we
    // can set the texture coordinates at the corners to be [0,1].

    const em::vec<3,GLfloat> wsDelta(
            (wsMax[0] - wsMin[0])/(texData.ni - 1),
            (wsMax[1] - wsMin[1])/(texData.nj - 1),
            (wsMax[2] - wsMin[2])/(texData.nk - 1));

    return wsDelta;
}

// -----------------------------------------------------------------------------

inline Texture3D*
createTexture3DFromField1f(const Nb::TileLayout&     layout,
                           const Nb::Field1f&        fld,
                           const NtVec3i&          texDim,
                           const em::vec<3,GLfloat>& wsMin,
                           const em::vec<3,GLfloat>& wsMax,
                           const bool                computeGradient=true,
                           em::vec<2,GLfloat>*       valRange=0)
{
    em::array3<em::vec<4,GLfloat> > texData;
    const em::vec<3,GLfloat> wsDelta = initTexture3DDeltas(
        texDim,wsMin,wsMax,texData
        );

    const int nnk(texData.nk);
    const int nnj(texData.nj);
    const int nni(texData.ni);

#pragma omp parallel for
    for (int k = 0; k < nnk; ++k) {
        for (int j = 0; j < nnj; ++j) {
            for (int i = 0; i < nni; ++i) {
                const em::vec<3,GLfloat> wsx(wsMin[0] + i*wsDelta[0],
                                             wsMin[1] + j*wsDelta[1],
                                             wsMin[2] + k*wsDelta[2]);

                if(computeGradient) {

                    // Sample both field & compute gradient.
                    
                    em::vec<3,GLfloat> grad;
                    GLfloat val =
                        Nb::sampleFieldGradient1f(wsx, layout, fld, grad);

                    // Texture is RGBA, where RGB is the gradient (nx, ny, nz)
                    // and A is the sampled value.
                    
                    texData(i, j, k)
                        = em::vec<4,GLfloat>(grad[0], grad[1], grad[2], val);

                } else {

                    // Just sample the field.

                    GLfloat val =
                        Nb::sampleField1f(wsx, layout, fld);
                    
                    // Texture is RGBA, value is duplicated across all channels.
                    
                    texData(i, j, k)
                        = em::vec<4,GLfloat>(val, val, val, val);
                }

            }
        }
    }

    if (0 != valRange) {
        (*valRange)[0] =  (std::numeric_limits<GLfloat>::max)();
        (*valRange)[1] = -(std::numeric_limits<GLfloat>::max)();

#pragma omp parallel for
        for (int k = 0; k < nnk; ++k) {
            for (int j = 0; j < nnj; ++j) {
                for (int i = 0; i < nni; ++i) {
                    GLfloat val = texData(i, j, k)[3];
                    (*valRange)[0] = std::min((*valRange)[0], val);
                    (*valRange)[1] = std::max((*valRange)[1], val);
                }
            }
        }
    }

    return new Ngl::Texture3D(
        &texData(0, 0, 0),texData.ni,texData.nj,texData.nk
        );
}


// -----------------------------------------------------------------------------

inline Texture3D*
createTexture3DFrom1f(const float               value,
                      const NtVec3i&          texDim,
                      const em::vec<3,GLfloat>& wsMin,
                      const em::vec<3,GLfloat>& wsMax)
{
    em::array3<em::vec<4,GLfloat> > texData;
    const em::vec<3,GLfloat> wsDelta = initTexture3DDeltas(
        texDim,wsMin,wsMax,texData
        );

    const int nnk(texData.nk);
    const int nnj(texData.nj);
    const int nni(texData.ni);

#pragma omp parallel for
    for (int k = 0; k < nnk; ++k) {
        for (int j = 0; j < nnj; ++j) {
            for (int i = 0; i < nni; ++i) {
                const em::vec<3,GLfloat> wsx(wsMin[0] + i*wsDelta[0],
                                             wsMin[1] + j*wsDelta[1],
                                             wsMin[2] + k*wsDelta[2]);
                
                // Texture is RGBA, value is duplicated across all channels.
                
                texData(i, j, k)
                    = em::vec<4,GLfloat>(value, value, value, value);
            }           
        }
    }

    return new Ngl::Texture3D(
        &texData(0, 0, 0),texData.ni,texData.nj,texData.nk
        );
}

// -----------------------------------------------------------------------------

inline Texture3D*
createTexture3DFromField3f(const Nb::TileLayout&     layout,
                           const Nb::Field1f&        fld0,
                           const Nb::Field1f&        fld1,
                           const Nb::Field1f&        fld2,
                           const NtVec3i&          texDim,
                           const em::vec<3,GLfloat>& wsMin,
                           const em::vec<3,GLfloat>& wsMax)
{
    em::array3<em::vec<4,GLfloat> > texData;
    const em::vec<3,GLfloat> wsDelta = initTexture3DDeltas(
        texDim,wsMin,wsMax,texData
        );

    const int nnk(texData.nk);
    const int nnj(texData.nj);
    const int nni(texData.ni);

#pragma omp parallel for
    for (int k = 0; k < nnk; ++k) {
        for (int j = 0; j < nnj; ++j) {
            for (int i = 0; i < nni; ++i) {
                const em::vec<3,GLfloat> wsx(wsMin[0] + i*wsDelta[0],
                                             wsMin[1] + j*wsDelta[1],
                                             wsMin[2] + k*wsDelta[2]);
                
                // Sample all three fields.
                
                GLfloat val0 = Nb::sampleField1f(wsx, layout, fld0);
                GLfloat val1 = Nb::sampleField1f(wsx, layout, fld1);
                GLfloat val2 = Nb::sampleField1f(wsx, layout, fld2);
                
                // Texture is RGBA, values are copies across RGB, setting
                // A to 1
                
                texData(i, j, k) = em::vec<4,GLfloat>(val0, val1, val2, 1);
            }
        }
    }

    return new Ngl::Texture3D(
        &texData(0, 0, 0),texData.ni,texData.nj,texData.nk
        );
}

// -----------------------------------------------------------------------------

inline Texture3D*
createTexture3DFrom3f(const NtVec3f&          value,
                      const NtVec3i&          texDim,
                      const em::vec<3,GLfloat>& wsMin,
                      const em::vec<3,GLfloat>& wsMax)
{
    em::array3<em::vec<4,GLfloat> > texData;
    const em::vec<3,GLfloat> wsDelta = initTexture3DDeltas(
        texDim,wsMin,wsMax,texData
        );

    const int nnk(texData.nk);
    const int nnj(texData.nj);
    const int nni(texData.ni);

#pragma omp parallel for
    for (int k = 0; k < nnk; ++k) {
        for (int j = 0; j < nnj; ++j) {
            for (int i = 0; i < nni; ++i) {
                const em::vec<3,GLfloat> wsx(wsMin[0] + i*wsDelta[0],
                                             wsMin[1] + j*wsDelta[1],
                                             wsMin[2] + k*wsDelta[2]);
                
                // Texture is RGBA, value is duplicated across all channels.
                
                texData(i, j, k)
                    = em::vec<4,GLfloat>(value[0], value[1], value[2], 1);
            }           
        }
    }

    return new Ngl::Texture3D(
        &texData(0, 0, 0),texData.ni,texData.nj,texData.nk
        );
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif // NGL_TEXTURE_UTILS_H
