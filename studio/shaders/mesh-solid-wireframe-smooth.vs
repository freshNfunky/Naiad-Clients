#version 130

// -----------------------------------------------------------------------------
//
// mesh-solid-wireframe-smooth.vs
//
// Fragment shader for rendering wireframe on top of smooth shaded solid 
// in a single pass.
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

uniform mat4  projection;
uniform mat4  modelview;
uniform mat3  normalMatrix;
uniform vec3  lightPosition;
uniform ivec4 viewport;

in vec3 position;
in vec3 normal;
in vec3 triVtx0;
in vec3 triVtx1;
in vec3 triVtx2;

out vec3  fragNormal;
out vec3  fragLightDir;
out vec3  fragEyeDir;
out float fragDist0;
out float fragDist1;
out float fragDist2;
out float fragDistMult;


void main()
{
    // Standard transforms

    vec4 eye    = modelview*vec4(position, 1.0);

    // Transform normal into eye-space. 

    vec4 lightPos = modelview*vec4(lightPosition, 1.0);    

    fragNormal   = normalMatrix*normal;
    fragLightDir = vec3(lightPos - eye);
    fragEyeDir   = -vec3(eye);

    gl_Position = projection*eye;    // Clip-space

    // vtx{N} are the three vertex positions of the triangle that the 
    // current vertex (position) belongs to.
    // Project the triangle vertices into viewport (screen) space.

    mat4 mvp = projection*modelview;

    vec4 vp0 = mvp*vec4(triVtx0, 1.0);
    vp0.x /= vp0.w;
    vp0.y /= vp0.w;
    vp0.z /= vp0.w;
    vp0.x  = viewport.x + 0.5*viewport.z*(vp0.x + 1.0);
    vp0.y  = viewport.y + 0.5*viewport.w*(vp0.y + 1.0);
    vp0.z  = gl_DepthRange.near + 
                0.5*gl_DepthRange.far*(vp0.z + gl_DepthRange.diff);
	
    vec4 vp1 = mvp*vec4(triVtx1, 1.0);
    vp1.x /= vp1.w;
    vp1.y /= vp1.w;
    vp1.z /= vp1.w;
    vp1.x  = viewport.x + 0.5*viewport.z*(vp1.x + 1.0);
    vp1.y  = viewport.y + 0.5*viewport.w*(vp1.y + 1.0);
    vp1.z  = gl_DepthRange.near + 
                0.5*gl_DepthRange.far*(vp1.z + gl_DepthRange.diff);

    vec4 vp2 = mvp*vec4(triVtx2, 1.0);
    vp2.x /= vp2.w;
    vp2.y /= vp2.w;
    vp2.z /= vp2.w;
    vp2.x  = viewport.x + 0.5*viewport.z*(vp2.x + 1.0);
    vp2.y  = viewport.y + 0.5*viewport.w*(vp2.y + 1.0);
    vp2.z  = gl_DepthRange.near + 
                0.5*gl_DepthRange.far*(vp2.z + gl_DepthRange.diff);

    // Project current vertex into viewport (screen) space.

    vec4 vp = gl_Position;
    vp.x /= vp.w;
    vp.y /= vp.w;
    vp.z /= vp.w;
    vp.x  = viewport.x + 0.5*viewport.z*(vp.x + 1.0);
    vp.y  = viewport.y + 0.5*viewport.w*(vp.y + 1.0);
    vp.z  = gl_DepthRange.near + 
                0.5*gl_DepthRange.far*(vp.z + gl_DepthRange.diff);

    // Compute the distance in viewport (screen) space to each of the triangle
    // edges. These distances are pre-multiplied by the perspective component
    // of the vertex to avoid errors with perspective correction. The inverse
    // perspective component is interpolated and used in the fragment shader.

    vec2 delta = vec2(vp1.x - vp0.x, vp1.y - vp0.y);
    fragDist0  = abs(delta.x*(vp0.y-vp.y)-delta.y*(vp0.x-vp.x))/length(delta);
    fragDist0 *= vp.w; 

    delta.x = vp2.x - vp1.x;
    delta.y = vp2.y - vp1.y;
    fragDist1  = abs(delta.x*(vp1.y- vp.y)-delta.y*(vp1.x-vp.x))/length(delta);
    fragDist1 *= vp.w;

    delta.x = vp0.x - vp2.x;
    delta.y = vp0.y - vp2.y;    
    fragDist2  = abs(delta.x*(vp2.y-vp.y)-delta.y*(vp2.x-vp.x))/length(delta);
    fragDist2 *= vp.w;	// Perspective pre-multiplication.

    fragDistMult = 1.0/vp.w; // Varying post-multiplication for fragment shader.
}

