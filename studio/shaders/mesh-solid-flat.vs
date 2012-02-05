#version 130

// -----------------------------------------------------------------------------
//
// mesh-solid-flat.vs
//
// Vertex shader for rendering a triangle mesh with per face lighting.
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
uniform mat3 normalMatrix;
uniform vec3 lightPosition;

in vec3 position;
in vec3 triVtx0;
in vec3 triVtx1;
in vec3 triVtx2;

out vec3 fragNormal;
out vec3 fragLightDir;
out vec3 fragEyeDir;


void main()
{
    // Standard transforms

    vec4 esx     = modelview*vec4(position, 1.0);
    vec3 normal  = cross(triVtx1 - triVtx0, triVtx2 - triVtx0);

    // Transform normal into eye-space. 

    vec4 lightPos = modelview*vec4(lightPosition, 1.0);    

    fragNormal   = normalMatrix*normal;
    fragLightDir = vec3(lightPos - esx);
    fragEyeDir   = -vec3(esx);

    gl_Position  = projection*esx;    // Clip-space
}

