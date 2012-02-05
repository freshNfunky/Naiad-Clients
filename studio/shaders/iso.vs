#version 130

// -----------------------------------------------------------------------------
//
// iso.vs
//
// Vertex shader for rendering an iso-surface in a scalar field.
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

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 worldToBox;
uniform vec4 lightPos;
uniform vec3 wsMin;        // Super-tile bounds
uniform vec3 invWsRange;


in vec3 position;


out vec3 fragTex;           // Texture coords
out vec4 fragBox;           // Clip-box coords
out vec4 fragLightDir;
out vec4 fragEyeDir;


void main()
{
    // Compute texture coordinates from world space position. World space
    // bounds must be set per rendered super-tile.

    fragTex.x = (position.x - wsMin.x)*invWsRange.x;
    fragTex.y = (position.y - wsMin.y)*invWsRange.y;
    fragTex.z = (position.z - wsMin.z)*invWsRange.z;

    // Compute clip-box coords.

    fragBox = worldToBox*vec4(position, 1.0);

    // Standard transforms

    vec4 eye    = modelview*vec4(position, 1.0);

    fragEyeDir   = -eye;
    fragLightDir = (lightPos - eye);

    // Clip-space

    gl_Position = projection*eye;    
}

