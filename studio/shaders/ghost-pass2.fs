#version 130

// -----------------------------------------------------------------------------
//
// ghost-pass2.fs
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

uniform float     samplingRate;
uniform float     minVal;
uniform float     invValRange;
uniform sampler3D ghostTex;

in vec3 fragGhostTex;   // Ghost Texture coords
in vec3 fragClipBox;    // Clip-box coords.

out vec4 fragColor;     // Final color.


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

    vec4 val = texture(ghostTex, fragGhostTex);
    
    // RGB(f) = (0,0,0)
    // A(f) = normalize [0,1]

    float alpha = (val.a - minVal)*invValRange;   
    float opacity = 1.0 - pow((1.0 - alpha), samplingRate);

    fragColor = vec4(0.0, 0.0, 0.0, opacity);
}

