// -----------------------------------------------------------------------------
//
// em_glmat33_algo.h
//
// Various useful matrix algorithms.
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

#ifndef EM_GLMAT33_ALGO
#define EM_GLMAT33_ALGO

#include "em_glmat33.h"

namespace em {

// -----------------------------------------------------------------------------

//! Return transposed matrix.
template<typename T> inline
glmat33<T>
transpose(const glmat33<T>& m)
{
    return glmat33<T>(transpose(m.to_mat33()));
}

// -----------------------------------------------------------------------------

template<typename T> inline
T
det(const glmat33<T>& m)
{
    return det(m.to_mat33());
}

// -----------------------------------------------------------------------------

}   // Namespace: em.

#endif  // EM_GLMAT33_ALGO
