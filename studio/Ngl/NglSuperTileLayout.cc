// -----------------------------------------------------------------------------
//
// NglSuperTileLayout.cc
//
// A collection of super-tiles
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

#include "NglSuperTileLayout.h"
#include "NglSuperTile.h"

#include <NbTile.h>
#include <NbTileLayout.h>
#include <Nbx.h>

#include <limits>


namespace
{
// -----------------------------------------------------------------------------

// operator<
// ---------
//! Compare iterators to super-tiles using tile counts.

bool
operator<(const std::vector<Ngl::SuperTile>::iterator& lhs,
          const std::vector<Ngl::SuperTile>::iterator& rhs)
{
    return lhs->tileCount() < rhs->tileCount();
}


// Functor for std::remove_if, used when getting rid of merged tiles.

class RemoveMerged
{
public:

    bool
    operator()(const Ngl::SuperTile& rhs) const
    {
        return rhs.merged();
    }
};

// -----------------------------------------------------------------------------
}   // Namespace: Anonymous.


namespace Ngl
{
// -----------------------------------------------------------------------------

// SuperTileLayout
// ---------------
//! Constructor.

SuperTileLayout::SuperTileLayout(const Nb::TileLayout* layout,
                                 const float           clipXform[16],
                                 const int             connectivity)
    : _wsMin( (std::numeric_limits<float>::max)()),
      _wsMax(-(std::numeric_limits<float>::max)())
{
    typedef std::vector<SuperTile> SuperTileVec;

    if (0 == layout) { // Handle an empty layout!

        // Just make a single super-tile to cover the clip-box!

        _superTileVec.push_back(SuperTile(clipXform));

    } else { // we do have a tile-layout, so build super-tiles from it...

        if (0 == layout->fineTileCount()) {
            NB_WARNING("Empty tile layout");
        }

        // Store iterators to connected super-tiles as we examine tiles.

        std::vector<SuperTileVec::iterator> connSuperTileVec;
        connSuperTileVec.reserve(connectivity);
        
        for(int t(0); t < layout->fineTileCount(); ++t) {
            
            connSuperTileVec.clear();
            
            const Nb::Tile tile(layout->fineTile(t));
            
            // Check if fine tile is connected to any existing super-tile.
            
            for(SuperTileVec::iterator iter = _superTileVec.begin();
                iter != _superTileVec.end();
                ++iter)
                {
                    if (iter->connected(tile, connectivity)) {
                        // Current tile is connected to super-tile, store 
                        // connection.
                        
                        connSuperTileVec.push_back(iter);
                    }
                }
            
            if (connSuperTileVec.empty()) {
                // Current tile is not connected to any existing super-tiles.
                // Start new super tile.
                
                _superTileVec.push_back(SuperTile(tile));
            }
            else if (1 == connSuperTileVec.size()) {
                // Current tile is connected to exactly one existing super-tile.
                // Add tile to super-tile.
                
                connSuperTileVec[0]->addTile(tile);
            }
            else if (2 == connSuperTileVec.size()) {
                // Current tile is connected to two existing super-tiles. Merge
                // these two super-tiles and add current tile. Remove super-tile
                // used for merging.
                
                connSuperTileVec[0]->addTile(tile);
                connSuperTileVec[0]->merge(*connSuperTileVec[1]);
                _superTileVec.erase(connSuperTileVec[1]);
            }
            else {
                // Current tile is connected to one or more existing 
                // super-tiles.  All connected super-tiles must be merged and
                // current tile added to result. We use the largest super tile
                // as a merge target in order to minmize memory reallocations
                // and transfer. Merged super-tiles are subsequently removed.
                
                //std::sort(connSuperTileVec.begin(), connSuperTileVec.end());
                connSuperTileVec[0]->addTile(tile);
                for(std::size_t cst(1); cst < connSuperTileVec.size(); ++cst) {
                    connSuperTileVec[0]->merge(*connSuperTileVec[cst]);
                }
                
                _superTileVec.erase(
                    std::remove_if(_superTileVec.begin(),
                                   _superTileVec.end(),
                                   RemoveMerged()),
                    _superTileVec.end());
                
//            std::sort(connSuperTileVec.begin(), connSuperTileVec.end());
//
//            connSuperTileVec.back()->_merge(
//                *connSuperTileVec.begin(),
//                *connSuperTileVec.begin() + (connSuperTileVec.size() - 1));
//
//            connSuperTileVec.back()->addTile(tile);
//
//            // Remove tiles that were merged, except target.
//
//            superTileVec.erase(
//                std::remove_if(superTileVec.begin(),
//                               superTileVec.end(),
//                               _removeDirty()), superTileVec.end());
            }
        }
    }        

    // Compute bounds of all super-tiles.

    NtVec3f stMin;
    NtVec3f stMax;
    for (std::size_t st(0); st < _superTileVec.size(); ++st) {
        _superTileVec[st].bounds(stMin, stMax);
        _wsMin[0] = std::min(_wsMin[0], stMin[0]);
        _wsMin[1] = std::min(_wsMin[1], stMin[1]);
        _wsMin[2] = std::min(_wsMin[2], stMin[2]);
        _wsMax[0] = std::max(_wsMax[0], stMax[0]);
        _wsMax[1] = std::max(_wsMax[1], stMax[1]);
        _wsMax[2] = std::max(_wsMax[2], stMax[2]);
    }
}


// superTileCount
// --------------
//! Return number of super-tiles.

int
SuperTileLayout::superTileCount() const
{
    return static_cast<int>(_superTileVec.size());
}


// superTile
// ---------
//! Return super-tile at index t.

const SuperTile&
SuperTileLayout::superTile(const int t) const
{
    if (0 > t || t >= superTileCount()) {
        NB_THROW("Invalid super tile index: " << t);
    }

    return _superTileVec[t];
}


// bounds
// ------
//! Return bounds in world-space.

void
SuperTileLayout::bounds(NtVec3f &min, NtVec3f &max) const
{
    min = _wsMin;
    max = _wsMax;
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.
