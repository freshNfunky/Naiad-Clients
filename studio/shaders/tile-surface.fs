#version 130

// -----------------------------------------------------------------------------
//
// tile-surface.fs
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
//
// -----------------------------------------------------------------------------

uniform vec4  matAmbient;
uniform vec4  matDiffuse;
uniform vec4  matSpecular;

in vec3 fragEye;

out vec4 fragColor;


void main()
{
    const float matShininess = 16.0;
    const vec4 lightAmbient  = vec4(1.0, 1.0, 1.0, 1.0);
    const vec4 lightDiffuse  = vec4(1.0, 1.0, 1.0, 1.0);
    const vec4 lightSpecular = vec4(1.0, 1.0, 1.0, 1.0);

    vec4 color = vec4(0.0);

    // Interpolated directions need to be normalized

    vec3 N = normalize(cross(dFdx(fragEye),dFdy(fragEye)));

    //vec3 N = normalize(vec3(dFdx(fragEye.z), dFdy(fragEye.z), 1));
    //vec3 N = vec3(dFdx(fragEye.z), dFdy(fragEye.z), 0.0);
    //vec3 N = normalize(vec3(dFdx(gl_FragCoord.z), dFdy(gl_FragCoord.z), 1));
    
    vec3 L = normalize(-fragEye);
    vec3 E = normalize(-fragEye);

    if (N.z > 0.0) {
        // Front-facing.
    
        color += matAmbient*lightAmbient;
        color += vec4(vec3(max(dot(N, L), 0.0)*(matDiffuse.rgb*lightDiffuse.rgb)), 
                      matDiffuse.a*lightDiffuse.a);

        float specular = pow(max(dot(reflect(-L, N), E), 0.0), matShininess);
        color += vec4(vec3(specular*(matSpecular.rgb*lightSpecular.rgb)), 
                      matSpecular.a*lightSpecular.a);
    }
    else {
        // Back-facing. Flip normal in calculations.

        color += matAmbient*lightAmbient;
        color += vec4(vec3(max(dot(-N, L), 0.0)*(matDiffuse.rgb*lightDiffuse.rgb)),
                      matDiffuse.a*lightDiffuse.a);

        float specular = pow(max(dot(reflect(-L, -N), E), 0.0), matShininess);
        color += vec4(vec3(specular*(matSpecular.rgb*lightSpecular.rgb)), 
                      matSpecular.a*lightSpecular.a);
    }

    fragColor = color;    // Assign fragment color
    //fragColor = vec4(abs(N.x), abs(N.y), abs(N.z), 1.0);
    //fragColor = vec4(abs(fragEye.z), abs(fragEye.z), abs(fragEye.z), 1.0);
    //fragColor = vec4(fragEye.x, fragEye.y, fragEye.z, 1.0);
    //fragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
}
