#version 130

// -----------------------------------------------------------------------------
//
// false-color.fs
//
// Fragment Shader for Naiad Studio's "False Color"
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

uniform int normGrad;

in vec3 fragGradient;
in vec4 fragBox;                // Clip-box coords

out vec4 color;

void main(void)
{    
    if (!(-0.5 <= fragBox.x && fragBox.x <= 0.5 &&
          -0.5 <= fragBox.y && fragBox.y <= 0.5 &&
          -0.5 <= fragBox.z && fragBox.z <= 0.5)) {
        discard;
    }

    vec3 grad=fragGradient;
    if(normGrad==1)
	grad=normalize(grad);

    grad += vec3(1.0, 1.0, 1.0);
    grad *= 0.5;

    color=vec4(grad,1.0);
}
