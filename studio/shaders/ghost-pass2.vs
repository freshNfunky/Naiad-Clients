#version 130

// -----------------------------------------------------------------------------
//
// ghost-pass2.vs
//
// ...
//
// Copyright (c) 2010 Exotic Matter AB.  All rights reserved.
//
// This file is part of Naiad Studio.
//
// Naiad Studio is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// Naiad Studio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// If you did not receive a copy of the GNU General Public License
// along with Naiad Studio, please see <http://www.gnu.org/licenses/>.
// -----------------------------------------------------------------------------

uniform mat4  lightModelview;
uniform mat4  lightProjection;
uniform mat4  wsxToClipBox;
uniform vec3  superTileMin;        // Super-tile bounds
uniform vec3  invSuperTileRange;

in vec3 wsx;                    // Vertex positions in world-space.

out vec3 fragGhostTex;          // Ghost texture coords.
out vec3 fragClipBox;           // Clip-box coords.


void main(void)
{
    // Compute texture coordinates from world space position. World space
    // bounds must be set per rendered super-tile. Use the component-wise
    // operators.

    fragGhostTex = (wsx - superTileMin)*invSuperTileRange;

    // Compute clip-box coords. Ignore W (which should always be 1).

    fragClipBox = vec3(wsxToClipBox*vec4(wsx, 1.0));

    // Raster clip-space.

    gl_Position = (lightProjection*lightModelview)*vec4(wsx, 1.0);    
}

