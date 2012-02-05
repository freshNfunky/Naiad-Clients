#version 130

// -----------------------------------------------------------------------------
//
// iso.fs
//
// Fragment shader for rendering an iso-surface in a scalar field.
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

uniform float     isoValue;     // Iso
uniform float     isoLowerBand;
uniform float     isoUpperBand;
uniform vec4      matFrontAmb;  // Material
uniform vec4      matFrontDiff;
uniform vec4      matFrontSpec;
uniform float     matFrontShine;
uniform vec4      matBackAmb; 
uniform vec4      matBackDiff; 
uniform vec4      matBackSpec; 
uniform float     matBackShine;
uniform vec4      lightAmb;     // Lighting
uniform vec4      lightDiff;   
uniform vec4      lightSpec;   

uniform sampler3D phiTex;
uniform mat3      normalMatrix;


in vec3 fragTex;                // Texture coords
in vec4 fragBox;                // Clip-box coords
in vec4 fragLightDir;
in vec4 fragEyeDir;


out vec4 fragColor;             // Final color.


void main(void)
{
    if (!(-0.5 <= fragBox.x && fragBox.x <= 0.5 &&
          -0.5 <= fragBox.y && fragBox.y <= 0.5 &&
          -0.5 <= fragBox.z && fragBox.z <= 0.5 &&
           0.0 <= fragTex.x && fragTex.x <= 1.0 &&
           0.0 <= fragTex.y && fragTex.y <= 1.0 &&
           0.0 <= fragTex.z && fragTex.z <= 1.0)) {
        discard;
    }

    vec4 phi = texture(phiTex, fragTex);
    float isoPhi = phi.a - isoValue;

    if ((isoValue - isoLowerBand) <= isoPhi && 
        (isoValue + isoUpperBand) >= isoPhi) {

        vec3 normal   = normalize(normalMatrix*(phi.xyz));
        //vec3 normal   = normalMatrix*(phi.rgb);
        //vec3 normal   = phi.rgb;
        vec3 lightDir = normalize(fragLightDir.xyz);
        vec3 eyeDir   = normalize(fragEyeDir.xyz);

        // Start accumulating from ambient contribution.

        fragColor = vec4(0.0);   

        // Front

        float diff = dot(normal, lightDir);
        float spec;

        if (diff > 0.0) {
            spec = pow(max(0.0, dot(reflect(-lightDir, normal), eyeDir)), 
                       matFrontShine);

            fragColor += matFrontAmb*lightAmb          +
                         diff*(matFrontDiff*lightDiff) +
                         spec*(matFrontSpec*lightSpec);
        }

        // Back, flip normal

        normal = -normal;
        diff = dot(normal, lightDir);

        if (diff > 0.0) {
            spec = pow(max(0.0, dot(reflect(-lightDir, normal), eyeDir)), 
                       matBackShine);

            fragColor += matBackAmb*lightAmb          +
                         diff*(matBackDiff*lightDiff) +
                         spec*(matBackSpec*lightSpec);
        }
    }
    else {
        discard;
    }
}

