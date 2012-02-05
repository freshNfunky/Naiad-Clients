// -----------------------------------------------------------------------------
//
// NglSuperTile.cc
//
// A collection of Ng::Tiles
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

#include "NglSuperTile.h"
#include "NglUtils.h"

#include <NbTile.h>
#include <Nbx.h>
#include <em_glmat44.h>


namespace Ngl
{
// -----------------------------------------------------------------------------

// SuperTile
// ---------
//! Constructor.

SuperTile::SuperTile(const Nb::Tile& tile)
    : _tileVec(1, tile),
      _tijkMin(tile.ti(), tile.tj(), tile.tk()),
      _tijkMax(_tijkMin),
      _wsMin(0.f, 0.f, 0.f),
      _wsMax(0.f, 0.f, 0.f),
      _merged(false)
{
    _tileVec[0].bounds(_wsMin, _wsMax);
}


// SuperTile
// ---------
//! Constructor.

SuperTile::SuperTile(const float clipXform[16])
    : _wsMin(em::glmat44f(clipXform)*NtVec3f(-1,-1,-1)),
      _wsMax(em::glmat44f(clipXform)*NtVec3f( 1, 1, 1)),
      _merged(false)
{
    //Ngl::multMat44fVec3f(clipXform,NtVec3f(-1,-1,-1),&_wsMin);
    //Ngl::multMat44fVec3f(clipXform,NtVec3f( 1, 1, 1),&_wsMax);
}


// tileCount
// ---------
//! Returns the number of tiles in super-tile.

int
SuperTile::tileCount() const
{
    return static_cast<int>(_tileVec.size());
}


// tile
// ----
//! Return tile at index t.

const Nb::Tile&
SuperTile::tile(const int t) const
{
    if (0 > t || t >= tileCount()) {
        NB_THROW("Invalid super-tile tile index: " << t);
    }

    return _tileVec[t];
}


// tijkBounds
// ----------
//! Retrieve bounds in tile coords.

void
SuperTile::tijkBounds(NtVec3i& tijkMin,
                      NtVec3i& tijkMax) const
{
    tijkMin = _tijkMin;
    tijkMax = _tijkMax;
}


// bounds
// ------
//! Retrieve bounds in world-space.

void
SuperTile::bounds(NtVec3f& min,
                  NtVec3f& max) const
{
    min = _wsMin;
    max = _wsMax;

//    // Super-tile always contains at least one tile.
//
//    _tileVec[0].bounds(min, max);
//
//    NtVec3f tmin;
//    NtVec3f tmax;
//    for (std::size_t t(1); t < _tileVec.size(); ++t) {
//        _tileVec[t].bounds(tmin, tmax);
//        min[0] = min(min[0], tmin[0]);
//        min[1] = min(min[1], tmin[1]);
//        min[2] = min(min[2], tmin[2]);
//        max[0] = std::max(max[0], tmax[0]);
//        max[1] = std::max(max[1], tmax[1]);
//        max[2] = std::max(max[2], tmax[2]);
//    }
}


// connected
// ---------
//! Return true if given tile is connected to this SuperTile.

bool
SuperTile::connected(const Nb::Tile& tile, const int connectivity) const
{
    // TODO: Provide padding based on connectivity!

    if (!_insideBBox(tile)) {
        return false;
    }

    const int conn(_validConnectivity(connectivity));   // May throw.

#if 1
    // Sequential version:
    //
    // It may be faster to check existing tiles in reverse order, but
    // there are no guarantees for this. The reasoning would be that
    // if tiles are given in some sort of ordered sequence it would
    // be more likely for consecutive tiles to be connected, so check
    // those first.

    std::vector<Nb::Tile>::const_reverse_iterator riter(_tileVec.rbegin());
    const std::vector<Nb::Tile>::const_reverse_iterator rend(_tileVec.rend());

    while (riter != rend) {
        // Check if given tile is connected to any of
        // the tiles in the super-tile. If connection is found return
        // immediately.

        if (_connected(*riter, tile, conn)) {
            // Given tile is connected to a tile in the super-tile.

            return true;
        }
        ++riter;
    }

    // Given tile is not connected to super-tile.

    return false;
#else
    // Parallel version:

    bool found(false);

    #pragma omp parallel
    {
        std::size_t t;

        #pragma omp for
        for(t = 0; t < _tileVec.size(); ++t) {

            #pragma omp flush (found)
            if (!found && _connected(_tileVec[t], tile, conn)) {
                //if (_connected(_tileVec[t], tile, conn)) {
                    // Given tile is connected to a tile in the super-tile.
                    found = true;
                    #pragma omp flush (found)
                //}
            }
        }
    }

    return found;
#endif
}


// addTile
// -------
//! Add given tile to super-tile, update bounds.

void
SuperTile::addTile(const Nb::Tile& tile)
{
    _tileVec.push_back(tile);   // May throw.

    const NtVec3i tijk(tile.ti(), tile.tj(), tile.tk());

    _tijkMin[0] = std::min<int>(_tijkMin[0], tijk[0]);
    _tijkMin[1] = std::min<int>(_tijkMin[1], tijk[1]);
    _tijkMin[2] = std::min<int>(_tijkMin[2], tijk[2]);

    _tijkMax[0] = std::max<int>(_tijkMax[0], tijk[0]);
    _tijkMax[1] = std::max<int>(_tijkMax[1], tijk[1]);
    _tijkMax[2] = std::max<int>(_tijkMax[2], tijk[2]);

    NtVec3f wsMin;
    NtVec3f wsMax;
    tile.bounds(wsMin, wsMax);

    _wsMin[0] = std::min<float>(_wsMin[0], wsMin[0]);
    _wsMin[1] = std::min<float>(_wsMin[1], wsMin[1]);
    _wsMin[2] = std::min<float>(_wsMin[2], wsMin[2]);

    _wsMax[0] = std::max<float>(_wsMax[0], wsMax[0]);
    _wsMax[1] = std::max<float>(_wsMax[1], wsMax[1]);
    _wsMax[2] = std::max<float>(_wsMax[2], wsMax[2]);
}


// merge
// -----
//! Merge provided super-tile into this one.

void
SuperTile::merge(SuperTile& rhs)
{
    // May throw.

    _tileVec.insert(_tileVec.end(), rhs._tileVec.begin(), rhs._tileVec.end());

    // Update bounds.

    _tijkMin[0] = std::min<int>(_tijkMin[0], rhs._tijkMin[0]);
    _tijkMin[1] = std::min<int>(_tijkMin[1], rhs._tijkMin[1]);
    _tijkMin[2] = std::min<int>(_tijkMin[2], rhs._tijkMin[2]);

    _tijkMax[0] = std::max<int>(_tijkMax[0], rhs._tijkMax[0]);
    _tijkMax[1] = std::max<int>(_tijkMax[1], rhs._tijkMax[1]);
    _tijkMax[2] = std::max<int>(_tijkMax[2], rhs._tijkMax[2]);

    _wsMin[0] = std::min<float>(_wsMin[0], rhs._wsMin[0]);
    _wsMin[1] = std::min<float>(_wsMin[1], rhs._wsMin[1]);
    _wsMin[2] = std::min<float>(_wsMin[2], rhs._wsMin[2]);

    _wsMax[0] = std::max<float>(_wsMax[0], rhs._wsMax[0]);
    _wsMax[1] = std::max<float>(_wsMax[1], rhs._wsMax[1]);
    _wsMax[2] = std::max<float>(_wsMax[2], rhs._wsMax[2]);

    rhs._merged = true;
}


// merged
// ------
//! Return merged-flag.

bool
SuperTile::merged() const
{
    return _merged;
}


// _insideBBox
// -----------
//! Returns true if given tile is inside padded bounding box
//! of super-tile.

bool
SuperTile::_insideBBox(const Nb::Tile& tile,
                       const NtVec3i& minPadding,
                       const NtVec3i& maxPadding) const
{
    const NtVec3i tijk(tile.ti(), tile.tj(), tile.tk());

    return (_tijkMin[0] + minPadding[0] <= tijk[0] &&
            _tijkMin[1] + minPadding[1] <= tijk[1] &&
            _tijkMin[2] + minPadding[2] <= tijk[2] &&
            _tijkMax[0] + maxPadding[0] >= tijk[0] &&
            _tijkMax[1] + maxPadding[1] >= tijk[1] &&
            _tijkMax[2] + maxPadding[2] >= tijk[2]);
}


// _connected
// ----------
//! Returns true if the two tiles are connected.

bool
SuperTile::_connected(const Nb::Tile& t0,
                      const Nb::Tile& t1,
                      const int       connectivity)
{
    const NtVec3i tijk0(t0.ti(), t0.tj(), t0.tk());
    const NtVec3i tijk1(t1.ti(), t1.tj(), t1.tk());

    if (tijk0 == tijk1) {
        // A tile is not connected to itself. This prevents multiple
        // copies of the same tile from being stored in a super-tile.

        return false;
    }

    // NB: Assumes valid connectivity.

    for(int c(0); c < connectivity; ++c) {
        if (tijk0 == (tijk1 + _cmask[c])) {
            // If the first tile is a neighbor of the second
            // the tiles are connected.

            return true;
        }
    }

    // Tiles are not connected.

    return false;
}


// _validConnectivity
// ------------------
//! Throws if input is invalid, otherwise returns input.

int
SuperTile::_validConnectivity(const int connectivity)
{
#if 1
    if ( 6 != connectivity &&
        18 != connectivity &&
        26 != connectivity) {
        NB_THROW("Invalid super tile connectivity: " << connectivity);
    }
#endif

    return connectivity;
}


// Neighbor offsets for connection checking.

const NtVec3i SuperTile::_cmask[] = {
    NtVec3i( 1,  0,  0),  // X+ Y  Z
    NtVec3i(-1,  0,  0),  // X- Y  Z
    NtVec3i( 0,  1,  0),  // X  Y+ Z
    NtVec3i( 0, -1,  0),  // X  Y- Z
    NtVec3i( 0,  0,  1),  // X  Y  Z+
    NtVec3i( 0,  0, -1),  // X  Y  Z-     6-connectivity

    NtVec3i( 1,  1,  0),  // X+ Y+ Z
    NtVec3i(-1,  1,  0),  // X- Y+ Z
    NtVec3i( 0,  1,  1),  // X  Y+ Z+
    NtVec3i( 0,  1, -1),  // X  Y+ Z-
    NtVec3i( 1,  0,  0),  // X+ Y  Z
    NtVec3i(-1,  0,  0),  // X- Y  Z
    NtVec3i( 0,  0,  1),  // X  Y  Z+
    NtVec3i( 0,  0, -1),  // X  Y  Z-
    NtVec3i( 1, -1,  0),  // X+ Y- Z
    NtVec3i(-1, -1,  0),  // X- Y- Z
    NtVec3i( 0, -1,  1),  // X  Y- Z+
    NtVec3i( 0, -1, -1),  // X  Y- Z-     18-connectivity

    NtVec3i( 1,  1,  1),  // X+ Y+ Z+
    NtVec3i(-1,  1,  1),  // X- Y+ Z+
    NtVec3i( 1,  1, -1),  // X+ Y+ Z-
    NtVec3i(-1,  1, -1),  // X- Y+ Z-
    NtVec3i( 1, -1,  1),  // X+ Y- Z+
    NtVec3i(-1, -1,  1),  // X- Y- Z+
    NtVec3i( 1, -1, -1),  // X+ Y- Z-
    NtVec3i(-1, -1, -1)   // X- Y- Z-     26-connectivity
};


//// _merge
//// ------
////! Merge provided range of super-tiles into this one. Super-tiles in
////! the range are marked as dirty.
//
//void
//SuperTile::_merge(const std::vector<SuperTile>::iterator begin,
//                  const std::vector<SuperTile>::iterator end)
//{
//    int newSize(size());
//    for(std::vector<SuperTile>::iterator iter = begin; iter != end; ++iter) {
//        newSize += iter->size();
//    }
//
//    // Reserve does not affect current data.
//    // Resize one extra since we will be adding the "current" tile as well.
//
//    _tileVec.reserve(newSize + 1);
//
//    for(std::vector<SuperTile>::iterator iter = begin; iter != end; ++iter) {
//        _tileVec.insert(_tileVec.end(),
//                        iter->_tileVec.begin(),
//                        iter->_tileVec.end());
//
//        // Update bounds.
//
//        _tiMin = min<int>(_tiMin, iter->tiMin());
//        _tiMax = max<int>(_tiMax, iter->tiMax());
//        _tjMin = min<int>(_tjMin, iter->tjMin());
//        _tjMax = max<int>(_tjMax, iter->tjMax());
//        _tkMin = min<int>(_tkMin, iter->tkMin());
//        _tkMax = max<int>(_tkMax, iter->tkMax());
//
//        iter->_setDirty(true);
//    }
//}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.
