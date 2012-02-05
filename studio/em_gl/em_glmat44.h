// -----------------------------------------------------------------------------
//
// em_glmat44.h
//
// Defines a dense 4x4 matrix suitable for basic linear algebra tasks such
// as vector transformations etc. Suitable for use with OpenGL.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
//
// This material contains the confidential and proprietary information of
// Exotic Matter AB and may not be disclosed, copied or duplicated in
// any form, electronic or hardcopy, in whole or in part, without the
// express prior written consent of Exotic Matter AB. This copyright notice
// does not imply publication.
//
// -----------------------------------------------------------------------------

#ifndef EM_GLMAT44
#define EM_GLMAT44

// TODO: change to "" when placed in proper em folder.
#include <em_assert.h>
#include <em_mat44.h>
#include <em_quaternion.h>
#include <em_vec.h>

#include <cstring>

namespace em {

// -----------------------------------------------------------------------------

template<typename T>
class glmat44
{
public:

    //! CTOR.
    explicit
    glmat44(const T v = 0)
        : _m(v)
    {}

    //! CTOR. Convert from em::mat44<S>.
    template<typename S>
    explicit
    glmat44(const mat44<S>& m)
        : _m(m)
    {}

    //! CTOR. Convert from array.
    explicit
    glmat44(const T m[16])
    {
        _m[0][0] = m[0]; _m[1][0] = m[4]; _m[2][0] = m[8];  _m[3][0] = m[12];
        _m[0][1] = m[1]; _m[1][1] = m[5]; _m[2][1] = m[9];  _m[3][1] = m[13];
        _m[0][2] = m[2]; _m[1][2] = m[6]; _m[2][2] = m[10]; _m[3][2] = m[14];
        _m[0][3] = m[3]; _m[1][3] = m[7]; _m[2][3] = m[11]; _m[3][3] = m[15];

    }

    //! CTOR. Construct from values.
    explicit
    glmat44(const T a0, const T a4, const T a8,  const T a12,
            const T a1, const T a5, const T a9,  const T a13,
            const T a2, const T a6, const T a10, const T a14,
            const T a3, const T a7, const T a11, const T a15)
    {
        _m[0][0] = a0; _m[1][0] = a4; _m[2][0] = a8;  _m[3][0] = a12;
        _m[0][1] = a1; _m[1][1] = a5; _m[2][1] = a9;  _m[3][1] = a13;
        _m[0][2] = a2; _m[1][2] = a6; _m[2][2] = a10; _m[3][2] = a14;
        _m[0][3] = a3; _m[1][3] = a7; _m[2][3] = a11; _m[3][3] = a15;
    }

    //! CTOR. Construct from quaternion.
    explicit
    glmat44(const quaternion<T>& q)
        : _m(q)
    {}

    //! Copy CTOR.
    glmat44(const glmat44<T>& rhs)
        : _m(rhs._m)
    {}

    //! Assign. No self-check.
    glmat44<T>&
    operator=(const glmat44<T>& rhs)
    {
        _m = rhs._m;
        return *this;
    }

    //! Assign.
    glmat44<T>&
    operator=(const mat44<T>& rhs)
    {
        _m = rhs;
        return *this;
    }


    //! Return column. No bounds checking in release mode.
    T*
    operator[](const int col)
    {
        EM_ASSERT(0 <= col && col < 4);
        return _m[col];
    }

    //! Return column. No bounds checking in release mode.
    const T*
    operator[](const int col) const
    {
        EM_ASSERT(0 <= col && col < 4);
        return _m[col];
    }

    //! Return element. No bounds checking in release mode.
    T&
    operator()(const int col, const int row)
    {
        EM_ASSERT(0 <= col && col < 4);
        EM_ASSERT(0 <= row && row < 4);
        return _m[col][row];
    }

    //! Return element. No bounds checking in release mode.
    const T&
    operator()(const int col, const int row) const
    {
        EM_ASSERT(0 <= col && col < 4);
        EM_ASSERT(0 <= row && row < 4);
        return _m[col][row];
    }


    //! Return implementation.
    mat44<T>&
    to_mat44()
    { return _m; }

    const mat44<T>&
    to_mat44() const
    { return _m; }

//    void
//    set_translate(const vec<3,T>& v)
//    { _m.set_translate(v); }

//    void
//    set_rotate(const quaternion<T>& q)
//    { _m.set_rotate(q); }

//    void
//    set_scale(const vec<3,T>& v)
//    { _m.set_scale(v); }

//    void
//    make_identity()
//    { set_scale(vec<3,T>(1,1,1)); }

private:     // Member variables.

    mat44<T> _m;
};

// -----------------------------------------------------------------------------

//! Return transposed matrix.
template<typename T> inline
glmat44<T>
transpose(const glmat44<T>& m)
{
    return glmat44<T>(transpose(m.to_mat44()));
}

// -----------------------------------------------------------------------------

//! Return scaled matrix.
template<typename T, typename S> inline
glmat44<T>
operator*(const S s, const glmat44<T>& m)
{
    return glmat44<T>(s*m.to_mat44());
}

// -----------------------------------------------------------------------------

//! Row vector multiplied by matrix, assume w = 1.
//! v' = [x, y, z, 1]*M
template<typename T, typename S> inline
vec<3,S>
operator*(const vec<3,S>& lhs, const glmat44<T>& rhs)
{
    return vec<3,S>(
        lhs[0]*rhs[0][0]+lhs[1]*rhs[0][1]+lhs[2]*rhs[0][2]+rhs[0][3],
        lhs[0]*rhs[1][0]+lhs[1]*rhs[1][1]+lhs[2]*rhs[1][2]+rhs[1][3],
        lhs[0]*rhs[2][0]+lhs[1]*rhs[2][1]+lhs[2]*rhs[2][2]+rhs[2][3],
        lhs[0]*rhs[3][0]+lhs[1]*rhs[3][1]+lhs[2]*rhs[3][2]+rhs[3][3]);
}

// -----------------------------------------------------------------------------

//! Row vector multiplied by matrix.
//! v' = v*M
template<typename T, typename S> inline
vec<4,S>
operator*(const vec<4,S>& lhs, const glmat44<T>& rhs)
{
    return vec<4,S>(
        lhs[0]*rhs[0][0]+lhs[1]*rhs[0][1]+lhs[2]*rhs[0][2]+lhs[3]*rhs[0][3],
        lhs[0]*rhs[1][0]+lhs[1]*rhs[1][1]+lhs[2]*rhs[1][2]+lhs[3]*rhs[1][3],
        lhs[0]*rhs[2][0]+lhs[1]*rhs[2][1]+lhs[2]*rhs[2][2]+lhs[3]*rhs[2][3],
        lhs[0]*rhs[3][0]+lhs[1]*rhs[3][1]+lhs[2]*rhs[3][2]+lhs[3]*rhs[3][3]);
}

// -----------------------------------------------------------------------------

//! Column vector multiplied by matrix, assume w = 1.
//! v' = M*[x, y, z, 1]
template<typename T, typename S> inline
vec<3,S>
operator*(const glmat44<T>& lhs, const vec<3,S>& rhs)
{
    return vec<3,S>(
        lhs[0][0]*rhs[0] + lhs[1][0]*rhs[1] + lhs[2][0]*rhs[2] + lhs[3][0],
        lhs[0][1]*rhs[0] + lhs[1][1]*rhs[1] + lhs[2][1]*rhs[2] + lhs[3][1],
        lhs[0][2]*rhs[0] + lhs[1][2]*rhs[1] + lhs[2][2]*rhs[2] + lhs[3][2]);
}

// -----------------------------------------------------------------------------

//! Column vector multiplied by matrix.
//! v' = M*v
template<typename T, typename S> inline
vec<4,S>
operator*(const glmat44<T>& lhs, const vec<4,S>& rhs)
{
    return vec<4,S>(
        lhs[0][0]*rhs[0]+lhs[1][0]*rhs[1]+lhs[2][0]*rhs[2]+lhs[3][0]*rhs[3],
        lhs[0][1]*rhs[0]+lhs[1][1]*rhs[1]+lhs[2][1]*rhs[2]+lhs[3][1]*rhs[3],
        lhs[0][2]*rhs[0]+lhs[1][2]*rhs[1]+lhs[2][2]*rhs[2]+lhs[3][2]*rhs[3],
        lhs[0][3]*rhs[0]+lhs[1][3]*rhs[1]+lhs[2][3]*rhs[2]+lhs[3][3]*rhs[3]);
}

// -----------------------------------------------------------------------------

//! Matrix addition.
template<typename T> inline
glmat44<T>
operator+(const glmat44<T>& a, const glmat44<T>& b)
{
    return glmat44<T>(a.to_mat44() + b.to_mat44());
}

// -----------------------------------------------------------------------------

//! Matrix subtraction.
template<typename T> inline
glmat44<T>
operator-(const glmat44<T>& a, const glmat44<T>& b)
{
    return glmat44<T>(a.to_mat44() - b.to_mat44());
}

// -----------------------------------------------------------------------------

//! Matrix multiplication.
template<typename T, typename S> inline
glmat44<T>
operator*(const glmat44<T>& a, const glmat44<S>& b)
{
    return glmat44<T>(
        a[0][0]*b[0][0] + a[1][0]*b[0][1] + a[2][0]*b[0][2] + a[3][0]*b[0][3],
        a[0][0]*b[1][0] + a[1][0]*b[1][1] + a[2][0]*b[1][2] + a[3][0]*b[1][3],
        a[0][0]*b[2][0] + a[1][0]*b[2][1] + a[2][0]*b[2][2] + a[3][0]*b[2][3],
        a[0][0]*b[3][0] + a[1][0]*b[3][1] + a[2][0]*b[3][2] + a[3][0]*b[3][3],
        a[0][1]*b[0][0] + a[1][1]*b[0][1] + a[2][1]*b[0][2] + a[3][1]*b[0][3],
        a[0][1]*b[1][0] + a[1][1]*b[1][1] + a[2][1]*b[1][2] + a[3][1]*b[1][3],
        a[0][1]*b[2][0] + a[1][1]*b[2][1] + a[2][1]*b[2][2] + a[3][1]*b[2][3],
        a[0][1]*b[3][0] + a[1][1]*b[3][1] + a[2][1]*b[3][2] + a[3][1]*b[3][3],
        a[0][2]*b[0][0] + a[1][2]*b[0][1] + a[2][2]*b[0][2] + a[3][2]*b[0][3],
        a[0][2]*b[1][0] + a[1][2]*b[1][1] + a[2][2]*b[1][2] + a[3][2]*b[1][3],
        a[0][2]*b[2][0] + a[1][2]*b[2][1] + a[2][2]*b[2][2] + a[3][2]*b[2][3],
        a[0][2]*b[3][0] + a[1][2]*b[3][1] + a[2][2]*b[3][2] + a[3][2]*b[3][3],
        a[0][3]*b[0][0] + a[1][3]*b[0][1] + a[2][3]*b[0][2] + a[3][3]*b[0][3],
        a[0][3]*b[1][0] + a[1][3]*b[1][1] + a[2][3]*b[1][2] + a[3][3]*b[1][3],
        a[0][3]*b[2][0] + a[1][3]*b[2][1] + a[2][3]*b[2][2] + a[3][3]*b[2][3],
        a[0][3]*b[3][0] + a[1][3]*b[3][1] + a[2][3]*b[3][2] + a[3][3]*b[3][3]);

//    glmat44<T> c;
//    const T* ap = &a[0][0];
//    const T* bp = &b[0][0];
//    T*       cp = &c[0][0];

//    cp[0]  = ap[0]*bp[0]  + ap[4]*bp[1]  + ap[8]*bp[2]   + ap[12]*bp[3];
//    cp[4]  = ap[0]*bp[4]  + ap[4]*bp[5]  + ap[8]*bp[6]   + ap[12]*bp[7];
//    cp[8]  = ap[0]*bp[8]  + ap[4]*bp[9]  + ap[8]*bp[10]  + ap[12]*bp[11];
//    cp[12] = ap[0]*bp[12] + ap[4]*bp[13] + ap[8]*bp[14]  + ap[12]*bp[15];
//    cp[1]  = ap[1]*bp[0]  + ap[5]*bp[1]  + ap[9]*bp[2]   + ap[13]*bp[3];
//    cp[5]  = ap[1]*bp[4]  + ap[5]*bp[5]  + ap[9]*bp[6]   + ap[13]*bp[7];
//    cp[9]  = ap[1]*bp[8]  + ap[5]*bp[9]  + ap[9]*bp[10]  + ap[13]*bp[11];
//    cp[13] = ap[1]*bp[12] + ap[5]*bp[13] + ap[9]*bp[14]  + ap[13]*bp[15];
//    cp[2]  = ap[2]*bp[0]  + ap[6]*bp[1]  + ap[10]*bp[2]  + ap[14]*bp[3];
//    cp[6]  = ap[2]*bp[4]  + ap[6]*bp[5]  + ap[10]*bp[6]  + ap[14]*bp[7];
//    cp[10] = ap[2]*bp[8]  + ap[6]*bp[9]  + ap[10]*bp[10] + ap[14]*bp[11];
//    cp[14] = ap[2]*bp[12] + ap[6]*bp[13] + ap[10]*bp[14] + ap[14]*bp[15];
//    cp[3]  = ap[3]*bp[0]  + ap[7]*bp[1]  + ap[11]*bp[2]  + ap[15]*bp[3];
//    cp[7]  = ap[3]*bp[4]  + ap[7]*bp[5]  + ap[11]*bp[6]  + ap[15]*bp[7];
//    cp[11] = ap[3]*bp[8]  + ap[7]*bp[9]  + ap[11]*bp[10] + ap[15]*bp[11];
//    cp[15] = ap[3]*bp[12] + ap[7]*bp[13] + ap[11]*bp[14] + ap[15]*bp[15];

//    return c;
}

// -----------------------------------------------------------------------------

//! Overloaded ostream operator.
template<typename T> inline
std::ostream&
operator<<(std::ostream& os, const glmat44<T>& m)
{
    os << m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << "\n"
       << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << "\n"
       << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << "\n"
       << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3] << "\n";
    return os;
}

// -----------------------------------------------------------------------------

// Convenient types.

typedef glmat44<float>  glmat44f;
typedef glmat44<double> glmat44d;

// -----------------------------------------------------------------------------

}  // namespace em

#endif  // EM_GLMAT44
