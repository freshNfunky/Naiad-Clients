#version 130

// -----------------------------------------------------------------------------
//
// ghost.vs
//
// Vertex shader for volmetric rendering of unlit density fields
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

uniform mat4  eyeModelview;
uniform mat4  eyeProjection;
uniform mat4  lightModelview;
uniform mat4  lightProjection;
//uniform ivec4 lightViewport;
uniform mat4  wsxToClipBox;
//uniform vec4 lightPos;
uniform vec3  superTileMin;        // Super-tile bounds
uniform vec3  invSuperTileRange;


in vec3 wsx;


out vec3 fragGhostTex;          // Ghost texture coords.
out vec2 fragLightTex;          // Light texture coords.
out vec3 fragClipBox;           // Clip-box coords.
//out vec4 fragLightDir;
//out vec4 fragEyeDir;


void main(void)
{
    // Compute texture coordinates from world space position. World space
    // bounds must be set per rendered super-tile. Use the component-wise
    // operators.

    fragGhostTex = (wsx - superTileMin)*invSuperTileRange;
    //fragDataTex.x = (wsx.x - wsMin.x)*invWsRange.x;
    //fragDataTex.y = (wsx.y - wsMin.y)*invWsRange.y;
    //fragDataTex.z = (wsx.z - wsMin.z)*invWsRange.z;

    // Compute texture coordinates for light texture;

    vec4 lightVp = (lightProjection*lightModelview)*vec4(wsx, 1.0);
    lightVp.x /= lightVp.w;
    lightVp.y /= lightVp.w;
    //lightVp.z /= lightVp.w;

    fragLightTex.s = 0.5*(lightVp.x + 1.0);
    fragLightTex.t = 0.5*(lightVp.y + 1.0);
    //lightVp.x = lightViewport[0] + 0.5*lightViewport[2]*(lightVp.x + 1.0);
    //lightVp.y = lightViewport[1] + 0.5*lightViewport[3]*(lightVp.y + 1.0);

    //fragLightTex.s = (lightVp.x - lightViewport[0])/lightViewport[2];
    //fragLightTex.t = (lightVp.y - lightViewport[1])/lightViewport[3];

    //fragLightTex.p 
    //    = gl_DepthRange.near + 
    //        0.5*gl_DepthRange.far*(fragLightTex.p + gl_DepthRange.diff);

    // Compute clip-box coords. Ignore W (which should always be 1).

    fragClipBox = vec3(wsxToClipBox*vec4(wsx, 1.0));

    // Standard transforms

    //vec4 eye    = modelview*vec4(position, 1.0);

    //fragEyeDir   = -eye;
    //fragLightDir = (lightPos - eye);

    // Clip-space

    gl_Position = (eyeProjection*eyeModelview)*vec4(wsx, 1.0);    
}

