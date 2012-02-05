#version 130

// -----------------------------------------------------------------------------
//
// mesh-solid-flat.fs
//
// Fragment shader for rendering a triangle mesh with per face lighting.
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

uniform vec4  frontMatAmbient;
uniform vec4  frontMatDiffuse;
uniform vec4  frontMatSpecular;
uniform float frontMatShininess;
uniform vec4  backMatAmbient;
uniform vec4  backMatDiffuse;
uniform vec4  backMatSpecular;
uniform float backMatShininess;
uniform vec4  lightAmbient;
uniform vec4  lightDiffuse;
uniform vec4  lightSpecular;

in vec3 fragNormal;
in vec3 fragLightDir;
in vec3 fragEyeDir;

out vec4 fragColor;


void main()
{
    vec4 color = vec4(0.0);

    // Interpolated directions need to be normalized

    vec3 N = normalize(fragNormal);
    vec3 L = normalize(fragLightDir);
    vec3 E = normalize(fragEyeDir);

    if (fragNormal.z > 0.0) {
        // Front-facing.
	
	color += frontMatAmbient*lightAmbient;	
	color += max(dot(N, L), 0.0)*(frontMatDiffuse*lightDiffuse);

	float specular = pow(max(dot(reflect(-L, N), E), 0.0), frontMatShininess);
	color += specular*(frontMatSpecular*lightSpecular);
    }
    else {
	// Back-facing. Flip normal in calculations.

	color += backMatAmbient*lightAmbient;	
	color += max(dot(-N, L), 0.0)*(backMatDiffuse*lightDiffuse);

	float specular = pow(max(dot(reflect(-L, -N), E), 0.0), backMatShininess);
	color += specular*(backMatSpecular*lightSpecular);
    }

    fragColor = color;    // Assign fragment color
}

