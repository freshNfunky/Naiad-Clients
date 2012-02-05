#version 130

// -----------------------------------------------------------------------------
//
// ghost.fs
//
// Fragment shader for volmetric rendering of unlit density fields
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

//uniform vec4      matAmb;  // Material
//uniform vec4      matDiff;
//uniform vec4      matSpec;
//uniform float     matShine;
//uniform vec4 lightPos;
//uniform float        lightInt;
//uniform vec4      lightAmb;     // Lighting
//uniform vec4      lightDiff;   
//uniform vec4      lightSpec;   

uniform float samplingRate;

uniform sampler3D ghostTex;
//uniform sampler2D lightTex;
//uniform mat3      normalMatrix;


in vec3 fragGhostTex;   // Ghost Texture coords
in vec2 fragLightTex;   // Light texture coords.
in vec3 fragClipBox;    // Clip-box coords.
//in vec4 fragLightDir;
//in vec4 fragEyeDir;


out vec4 fragColor;             // Final color.


const float kA = 0.6;
const float kD = 0.3;
const float kS = 0.1;
const float kN = 2.0;

const float minF = 0.0;
const float invRangeF = 1.0/8.0;

const vec3 color = vec3(0.1, 0.1, 0.1);

const vec3 lightColor = vec3(1.0, 1.0, 1.0);


void main(void)
{
    if (!(-0.5 <= fragClipBox.x  && fragClipBox.x  <= 0.5 &&
          -0.5 <= fragClipBox.y  && fragClipBox.y  <= 0.5 &&
          -0.5 <= fragClipBox.z  && fragClipBox.z  <= 0.5 &&
           0.0 <= fragGhostTex.x && fragGhostTex.x <= 1.0 &&
           0.0 <= fragGhostTex.y && fragGhostTex.y <= 1.0 &&
           0.0 <= fragGhostTex.z && fragGhostTex.z <= 1.0)) {
        discard;
    }

    //const vec4 emissiveColor = vec4(0.5, 0.5, 0.5, 1.0);
    //float a = tex3D(texCoord, dataTex);  // Read 3D data texture
    //color = a * emissiveColor;           // Multiply color by opacity

    vec4 f = texture(ghostTex, fragGhostTex);
    
    // RGB(f) = constant
    // A(f) = normalize [0,1]
   
    float opacity = 1.0 - pow((1.0 - (f.a - minF)*invRangeF), samplingRate);

    //vec3 normal   = normalMatrix*(f.xyz);
    //vec3 lightDir = normalize(fragLightDir.xyz);
    //vec3 eyeDir   = normalize(fragEyeDir.xyz);

    //vec3 half = normalize(lightDir + eyeDir);
    //float i=kA+lightInt*(kD*dot(normal,lightDir)+kS*pow(dot(half,normal),kN));

    fragColor = vec4(opacity*color, opacity);
    //fragColor = vec4(fragLightTex.s, fragLightTex.t, 0.0, 1.0);
}

