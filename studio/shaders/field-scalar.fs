#version 130

// -----------------------------------------------------------------------------
//
// field-scalar.fs
//
// Fragment shader used by the Field-Scalar-Scope.
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

uniform sampler3D phiTex;
uniform float     minVisible;
uniform float     maxVisible;

in vec3 fragTex;                // Texture coords
in vec4 fragBox;                // Clip-box coords
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
    float value=phi.a;

    if(value<minVisible || value>maxVisible)
       discard;

    fragColor = vec4(value,0,0,1);
}

