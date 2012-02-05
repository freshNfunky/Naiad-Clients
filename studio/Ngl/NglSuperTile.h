// -----------------------------------------------------------------------------
//
// NglSuperTile.h
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

#ifndef NGL_SUPERTILE_H
#define NGL_SUPERTILE_H

#include <NbValueType.h>
#include <NbTile.h>

#include <vector>

#include <Ni.h>

namespace Ngl
{
// -----------------------------------------------------------------------------

// A super-tile is a collection of Ng::Tile's. It is not strictly enforced
// that these tiles be connected in any way, nor that the collection
// contains only unique tiles. However, it is possible to enforce this
// through the interface. It is always valid to add a tile to a super-tile.
// The function connected() provides a way of checking if a tile is
// connected to the super-tile. This function returns false for tiles that
// are not connected to the super-tile and for tiles that already exist. Note
// that it is possible to check different tiles for different connectivity.
// A super tile can never be empty, it must always have at least one tile.

class SuperTile
{
public:     // Interface.

    explicit
    SuperTile(const Nb::Tile& tile);

    explicit
    SuperTile(const float clipXform[16]);

    // Default copy, assign & destruct.

    int tileCount() const;

    const Nb::Tile& tile(int t) const;

    void tijkBounds(NtVec3i& tijkMin, NtVec3i& tijkMax) const;

    void bounds(NtVec3f& min, NtVec3f& max) const;

    bool connected(const Nb::Tile& tile, int connectivity = 26) const;

    void addTile(const Nb::Tile& tile);

    void merge(SuperTile& rhs);

    bool merged() const;

private:    // Member variables.

    std::vector<Nb::Tile> _tileVec;
    NtVec3i             _tijkMin;     // Bounds in tile coordinates.
    NtVec3i             _tijkMax;
    NtVec3f             _wsMin;       // Bounds in world-space.
    NtVec3f             _wsMax;
    bool                  _merged;      // Merged into another super-tile?

private:    // Utility functions.

    bool _insideBBox(const Nb::Tile&  tile,
                     const NtVec3i& minPadding = NtVec3i(-1, -1, -1),
                     const NtVec3i& maxPadding = NtVec3i( 1,  1,  1)) const;

    static
    bool _connected(const Nb::Tile& t0,
                    const Nb::Tile& t1,
                    int             connectivity = 26);

    static
    int _validConnectivity(int connectivity);

    static
    const NtVec3i _cmask[];

private:    // Disabled.

    SuperTile();
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_SUPERTILE_H
