// -----------------------------------------------------------------------------
//
// Ns3DLocator.cc
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

#include "Ns3DLocator.h"

#include <cmath>

namespace {

GLfloat
deg2rad(const GLfloat deg)
{
    return ((static_cast<GLfloat>(M_PI)/180.0f)*deg);
}

}   // Namespace: anonymous.

// -----------------------------------------------------------------------------

void
Ns3DLocator::setTranslationMat(mat44f *m, const vec3f &t)
{
    (*m)[3][0] = t[0];
    (*m)[3][1] = t[1];
    (*m)[3][2] = t[2];
}


void
Ns3DLocator::setScaleMat(mat44f *m, const vec3f &s)
{
    (*m)[0][0] = s[0];
    (*m)[1][1] = s[1];
    (*m)[2][2] = s[2];
}


void
Ns3DLocator::setRotationMatX(mat44f *m, GLfloat rx)
{
    (*m)[1][1] = cos(deg2rad(rx));
    (*m)[1][2] = sin(deg2rad(rx));
    (*m)[2][1] = -(*m)[1][2];
    (*m)[2][2] = (*m)[1][1];
}


void
Ns3DLocator::setRotationMatY(mat44f *m, GLfloat ry)
{
    (*m)[0][0] = cos(deg2rad(ry));
    (*m)[0][2] = -sin(deg2rad(ry));
    (*m)[2][0] = -(*m)[0][2];
    (*m)[2][2] = (*m)[0][0];
}


void
Ns3DLocator::setRotationMatZ(mat44f *m, GLfloat rz)
{
    (*m)[0][0] = cos(deg2rad(rz));
    (*m)[0][1] = -sin(deg2rad(rz));
    (*m)[1][1] = (*m)[0][0];
    (*m)[1][0] = -(*m)[0][1];
}

// -----------------------------------------------------------------------------

void
Ns3DLocator::setRotation(const vec3f &rot)
{
    _rotationAngles = rot;

    for (int r = 0; r < 3; ++r) {
        while (_rotationAngles[r] < -180.0f)
            _rotationAngles[r] += 360.0f;
        while (_rotationAngles[r] > 180.0f)
            _rotationAngles[r] -= 360.0f;
    }

    Ns3DLocator::setRotationMatX(&_rotationMatX, _rotationAngles[0]);
    Ns3DLocator::setRotationMatY(&_rotationMatY, _rotationAngles[1]);
    Ns3DLocator::setRotationMatZ(&_rotationMatZ, _rotationAngles[2]);

    updateRotation();
    updateXForm();
}


void
Ns3DLocator::setRotationX(const GLfloat rx)
{
    _rotationAngles[0] = rx;

    while (_rotationAngles[0] < -180.0f)
        _rotationAngles[0] += 360.0f;
    while (_rotationAngles[0] > 180.0f)
        _rotationAngles[0] -= 360.0f;

    Ns3DLocator::setRotationMatX(&_rotationMatX, _rotationAngles[0]);

    updateRotation();
    updateXForm();
}


void
Ns3DLocator::setRotationY(const GLfloat ry)
{
    _rotationAngles[1] = ry;

    while (_rotationAngles[1] < -180.0f)
        _rotationAngles[1] += 360.0f;
    while (_rotationAngles[1] > 180.0f)
        _rotationAngles[1] -= 360.0f;

    Ns3DLocator::setRotationMatY(&_rotationMatY, _rotationAngles[1]);

    updateRotation();
    updateXForm();
}


void
Ns3DLocator::setRotationZ(const GLfloat rz)
{
    _rotationAngles[2] = rz;

    while (_rotationAngles[2] < -180.0f)
        _rotationAngles[2] += 360.0f;
    while (_rotationAngles[2] > 180.0f)
        _rotationAngles[2] -= 360.0f;

    Ns3DLocator::setRotationMatZ(&_rotationMatZ, _rotationAngles[2]);

    updateRotation();
    updateXForm();
}


void
Ns3DLocator::set(const vec3f &t,
             const vec3f &r,
             const vec3f &s,
             const vec3f &pivot)
{
    Ns3DLocator::setTranslationMat(&_translationMat, t);
    Ns3DLocator::setScaleMat(&_scaleMat, s);

    _rotationAngles = r;

    for (int r = 0; r < 3; ++r) {
        while (_rotationAngles[r] < -180.0f)
            _rotationAngles[r] += 360.0f;
        while (_rotationAngles[r] > 180.0f)
            _rotationAngles[r] -= 360.0f;
    }

    Ns3DLocator::setRotationMatX(&_rotationMatX, _rotationAngles[0]);
    Ns3DLocator::setRotationMatY(&_rotationMatY, _rotationAngles[1]);
    Ns3DLocator::setRotationMatZ(&_rotationMatZ, _rotationAngles[2]);

    _pivot = pivot;

    updateRotation();
    updateXForm();
}


void
Ns3DLocator::updateRotation()
{
    _rotationMat = _rotationMatZ*_rotationMatY*_rotationMatX;
}

void
Ns3DLocator::updateXForm()
{
//    mat44f originToPivot(1.0f);       // Identity
//    originToPivot[3][0] = -_pivot[0];
//    originToPivot[3][1] = -_pivot[1];
//    originToPivot[3][2] = -_pivot[2];
//
//    mat44f pivotToOrigin(1.0f);       // Identity
//    pivotToOrigin[3][0] = _pivot[0];
//    pivotToOrigin[3][1] = _pivot[1];
//    pivotToOrigin[3][2] = _pivot[2];

    _xformMat = _scaleMat*_rotationMat*_translationMat;
}
