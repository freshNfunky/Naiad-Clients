// -----------------------------------------------------------------------------
//
// Ns3DLocator.h
//
// Describe the object to world transformation of an object.
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

#ifndef NS3D_LOCATOR_H
#define NS3D_LOCATOR_H

#include <em_vec.h>
#include <em_mat44.h>

#include <QtOpenGL>

class Ns3DLocator
{
public:

    typedef em::vec<3, GLfloat> vec3f;
    typedef em::mat44<GLfloat> mat44f;

public:

    // NB: Assumes that matrix arguments are set to identity.

    static void setTranslationMat(mat44f *m, const vec3f &t);
    static void setScaleMat(mat44f *m, const vec3f &s);
    static void setRotationMatX(mat44f *m, GLfloat rx);
    static void setRotationMatY(mat44f *m, GLfloat ry);
    static void setRotationMatZ(mat44f *m, GLfloat rz);

public:

    Ns3DLocator()
        : _xformMat(1.0f),      // Identity
          _translationMat(1.0f),
          _scaleMat(1.0f),
          _rotationMat(1.0f),
          _rotationMatX(1.0f),
          _rotationMatY(1.0f),
          _rotationMatZ(1.0f),
          _rotationAngles(0.0f, 0.0f, 0.0f),
          _pivot(0.0f, 0.0f, 0.0f)
    {
    }

    // Default copy, assign, destruct

    vec3f
    translationVec() const
    {
        return vec3f(_translationMat[3][0],
                     _translationMat[3][1],
                     _translationMat[3][2]);
    }

    GLfloat translationX() const { return _translationMat[3][0]; }
    GLfloat translationY() const { return _translationMat[3][1]; }
    GLfloat translationZ() const { return _translationMat[3][2]; }
    const GLfloat *translation() const { return &_translationMat[0][0]; }
    const mat44f &translationMat() const { return _translationMat; }

    vec3f scaleVec() const
    { return vec3f(_scaleMat[0][0], _scaleMat[1][1], _scaleMat[2][2]); }

    GLfloat scaleX() const { return _scaleMat[0][0]; }
    GLfloat scaleY() const { return _scaleMat[1][1]; }
    GLfloat scaleZ() const { return _scaleMat[2][2]; }
    const GLfloat *scale() const { return &_scaleMat[0][0]; }
    const mat44f &scaleMat() const { return _scaleMat; }

    const vec3f &rotationAngles() const { return _rotationAngles; }
    GLfloat rotationAngleX() const { return _rotationAngles[0]; }
    GLfloat rotationAngleY() const { return _rotationAngles[1]; }
    GLfloat rotationAngleZ() const { return _rotationAngles[2]; }
    const mat44f &rotationMat() const { return _rotationMat; }
    const mat44f &rotationMatX() const { return _rotationMatX; }
    const mat44f &rotationMatY() const { return _rotationMatY; }
    const mat44f &rotationMatZ() const { return _rotationMatZ; }
    const GLfloat *rotation() const { return &_rotationMat[0][0]; }
    const GLfloat *rotationX() const { return &_rotationMatX[0][0]; }
    const GLfloat *rotationY() const { return &_rotationMatY[0][0]; }
    const GLfloat *rotationZ() const { return &_rotationMatZ[0][0]; }

    const vec3f &pivot() const { return _pivot; }
    GLfloat pivotX() const { return _pivot[0]; }
    GLfloat pivotY() const { return _pivot[1]; }
    GLfloat pivotZ() const { return _pivot[2]; }

    const GLfloat *xform() const { return &_xformMat[0][0]; }
    const mat44f &xformMat() const { return _xformMat; }

// -----------------------------------------------------------------------------

    void
    setTranslation(const vec3f &t)
    {
        Ns3DLocator::setTranslationMat(&_translationMat, t);
        updateXForm();
    }


    void
    setTranslationX(const GLfloat tx)
    {
        _translationMat[3][0] = tx;
        updateXForm();
    }


    void
    setTranslationY(const GLfloat ty)
    {
        _translationMat[3][1] = ty;
        updateXForm();
    }


    void
    setTranslationZ(const GLfloat tz)
    {
        _translationMat[3][2] = tz;
        updateXForm();
    }

// -----------------------------------------------------------------------------

    void
    setScale(const vec3f &s)
    {
        Ns3DLocator::setScaleMat(&_scaleMat, s);
        updateXForm();
    }


    void
    setScaleX(const GLfloat sx)
    {
        _scaleMat[0][0] = sx;
        updateXForm();
    }


    void
    setScaleY(const GLfloat sy)
    {
        _scaleMat[1][1] = sy;
        updateXForm();
    }


    void
    setScaleZ(const GLfloat sz)
    {
        _scaleMat[2][2] = sz;
        updateXForm();
    }

// -----------------------------------------------------------------------------

    void setRotation(const vec3f &rot);
    void setRotationX(GLfloat rx);
    void setRotationY(GLfloat ry);
    void setRotationZ(GLfloat rz);

// -----------------------------------------------------------------------------

    void
    setPivot(const vec3f &piv)
    {
        _pivot = piv;
        updateXForm();
    }


    void
    setPivotX(const GLfloat px)
    {
        _pivot[0] = px;
        updateXForm();
    }


    void
    setPivotY(const GLfloat py)
    {
        _pivot[1] = py;
        updateXForm();
    }


    void
    setPivotZ(const GLfloat pz)
    {
        _pivot[2] = pz;
        updateXForm();
    }

// -----------------------------------------------------------------------------

    void set(const vec3f &t,
             const vec3f &r,
             const vec3f &s,
             const vec3f &pivot);

private:

    void updateXForm();
    void updateRotation();

private:

    mat44f _xformMat;
    mat44f _translationMat;
    mat44f _scaleMat;
    mat44f _rotationMat;
    mat44f _rotationMatX;
    mat44f _rotationMatY;
    mat44f _rotationMatZ;
    //vec3f _translation;
    vec3f _rotationAngles;
    //vec3f _scale;
    vec3f _pivot;
};

#endif // NS3D_LOCATOR_H
