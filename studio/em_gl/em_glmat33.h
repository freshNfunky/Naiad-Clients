// -----------------------------------------------------------------------------
//
// em_glmat33.h
//
// Defines a dense 3x3 matrix. Suitable for use with OpenGL.
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

#ifndef EM_GLMAT33
#define EM_GLMAT33

// TODO: change to "" when placed in proper em folder.
#include <em_assert.h>
#include <em_mat33.h>
#include <em_vec.h>

namespace em {

// -----------------------------------------------------------------------------

template<typename T>
class glmat33
{
public:

    //! CTOR.
    explicit
    glmat33(const T v = 0)
        : _m(v)
    {}

    //! CTOR. Convert from em::mat33<S>.
    template<typename S>
    explicit
    glmat33(const mat33<S>& rhs)
        : _m(rhs)
    {}

    //! CTOR. Convert from array.
    explicit
    glmat33(const T m[9])
    {
        _m[0][0] = m[0]; _m[1][0] = m[3]; _m[2][0] = m[6];
        _m[0][1] = m[1]; _m[1][1] = m[4]; _m[2][1] = m[7];
        _m[0][2] = m[2]; _m[1][2] = m[5]; _m[2][2] = m[8];
    }

    //! CTOR. Construct from values.
    explicit
    glmat33(const T a0, const T a3, const T a6,
            const T a1, const T a4, const T a7,
            const T a2, const T a5, const T a8)
    {
        _m[0][0] = a0; _m[1][0] = a3; _m[2][0] = a6;
        _m[0][1] = a1; _m[1][1] = a4; _m[2][1] = a7;
        _m[0][2] = a2; _m[1][2] = a5; _m[2][2] = a8;
    }

    //! CTOR. Construct from column vectors.
    template<typename S>
    explicit
    glmat33(const vec<3,S>& col0,
            const vec<3,S>& col1,
            const vec<3,S>& col2)
    {
        _m[0][0] = col0[0]; _m[1][0] = col1[0]; _m[2][0] = col2[0];
        _m[0][1] = col0[1]; _m[1][1] = col1[1]; _m[2][1] = col2[1];
        _m[0][2] = col0[2]; _m[1][2] = col1[2]; _m[2][2] = col2[2];
    }

    //! Copy CTOR.
    glmat33(const glmat33<T>& rhs)
        : _m(rhs._m)
    {}

    //! Assign. No self-check.
    glmat33<T>&
    operator=(const glmat33<T>& rhs)
    {
        _m = rhs._m;
        return *this;
    }

    //! Assign.
    glmat33<T>&
    operator=(const mat33<T>& rhs)
    {
        _m = rhs;
        return *this;
    }


    //! Return column. No bounds checking in release mode.
    T*
    operator[](const int col)
    {
        EM_ASSERT(0 <= col && col < 3);
        return _m[col];
    }

    //! Return column. No bounds checking in release mode.
    const T*
    operator[](const int col) const
    {
        EM_ASSERT(0 <= col && col < 3);
        return _m[col];
    }

    //! Return element. No bounds checking in release mode.
    T&
    operator()(const int col, const int row)
    {
        EM_ASSERT(0 <= col && col < 3);
        EM_ASSERT(0 <= row && row < 3);
        return _m[col][row];
    }

    //! Return element. No bounds checking in release mode.
    const T&
    operator()(const int col, const int row) const
    {
        EM_ASSERT(0 <= col && col < 3);
        EM_ASSERT(0 <= row && row < 3);
        return _m[col][row];
    }

    //! Return implementation.
    mat33<T>&
    to_mat33()
    { return _m; }

    const mat33<T>&
    to_mat33() const
    { return _m; }

    template<typename S>
    glmat33<T>&
    operator*=(const S s)
    {
        _m *= s;
        return *this;
    }

private:    // Member variables.

    mat33<T> _m;
};

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------

//! Return scaled matrix.
template<typename T, typename S> inline
glmat33<T>
operator*(const S lhs, const glmat33<T>& rhs)
{
    return glmat33<T>(lhs*rhs.to_mat33());
}

// -----------------------------------------------------------------------------

//! Row vector multiplied by matrix.
//! v' = v*M
template<typename T, typename S> inline
vec<3,S>
operator*(const vec<3,S>& lhs, const glmat33<T>& rhs)
{
    return vec<3,S>(
        lhs[0]*rhs[0][0] + lhs[1]*rhs[0][1] + lhs[2]*rhs[0][2],
        lhs[0]*rhs[1][0] + lhs[1]*rhs[1][1] + lhs[2]*rhs[1][2],
        lhs[0]*rhs[2][0] + lhs[1]*rhs[2][1] + lhs[2]*rhs[2][2]);
}

// -----------------------------------------------------------------------------

//! Column vector multiplied by matrix.
//! v' = M*v
template<typename T, typename S> inline
vec<3,S>
operator*(const glmat33<T>& lhs, const vec<3,S>& rhs)
{
    return vec<3,S>(
        lhs[0][0]*rhs[0] + lhs[1][0]*rhs[1] + lhs[2][0]*rhs[2],
        lhs[0][1]*rhs[0] + lhs[1][1]*rhs[1] + lhs[2][1]*rhs[2],
        lhs[0][2]*rhs[0] + lhs[1][2]*rhs[1] + lhs[2][2]*rhs[2]);
}

// -----------------------------------------------------------------------------

//! Matrix addition.
template<typename T> inline
glmat33<T>
operator+(const glmat33<T>& a, const glmat33<T>& b)
{
    return glmat33<T>(a.to_mat33() + b.to_mat33());
}

// -----------------------------------------------------------------------------

//! Matrix subtraction.
template<typename T> inline
glmat33<T>
operator-(const glmat33<T>& a, const glmat33<T>& b)
{
    return glmat33<T>(a.to_mat33() - b.to_mat33());
}

// -----------------------------------------------------------------------------

//! Matrix multiplication.
template<typename T, typename S> inline
glmat33<T>
operator*(const glmat33<T>& lhs, const glmat33<S>& rhs)
{
    return glmat33<T>(
        lhs[0][0]*rhs[0][0] + lhs[1][0]*rhs[0][1] + lhs[2][0]*rhs[0][2],
        lhs[0][0]*rhs[1][0] + lhs[1][0]*rhs[1][1] + lhs[2][0]*rhs[1][2],
        lhs[0][0]*rhs[2][0] + lhs[1][0]*rhs[2][1] + lhs[2][0]*rhs[2][2],
        lhs[0][1]*rhs[0][0] + lhs[1][1]*rhs[0][1] + lhs[2][1]*rhs[0][2],
        lhs[0][1]*rhs[1][0] + lhs[1][1]*rhs[1][1] + lhs[2][1]*rhs[1][2],
        lhs[0][1]*rhs[2][0] + lhs[1][1]*rhs[2][1] + lhs[2][1]*rhs[2][2],
        lhs[0][2]*rhs[0][0] + lhs[1][2]*rhs[0][1] + lhs[2][2]*rhs[0][2],
        lhs[0][2]*rhs[1][0] + lhs[1][2]*rhs[1][1] + lhs[2][2]*rhs[1][2],
        lhs[0][2]*rhs[2][0] + lhs[1][2]*rhs[2][1] + lhs[2][2]*rhs[2][2]);

//    mat33<T> c;
//    for(int i(0); i < 3; ++i) {
//        for(int j(0); j < 3; ++j) {
//            c[i][j] = a[i][0]*b[0][j] + a[i][1]*b[1][j] + a[i][2]*b[2][j];
//        }
//    }

//    return c;
}

// -----------------------------------------------------------------------------

template<typename T> inline
std::ostream&
operator<<(std::ostream& os, const glmat33<T>& m)
{
    os << m[0][0] << " " << m[1][0] << " " << m[2][0] << "\n"
       << m[0][1] << " " << m[1][1] << " " << m[2][1] << "\n"
       << m[0][2] << " " << m[1][2] << " " << m[2][2] << "\n";
    return os;
}

// -----------------------------------------------------------------------------

typedef glmat33<float>  glmat33f;
typedef glmat33<double> glmat33d;

// -----------------------------------------------------------------------------

}  // namespace em

#endif  // EM_GLMAT33
