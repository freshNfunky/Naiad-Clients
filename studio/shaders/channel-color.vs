#version 130

// -----------------------------------------------------------------------------
//
// big-blue.vs
//
// Vertex Shader for  Naiad Studio's "Channel Color"
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
uniform mat4 worldToBox;

in vec3 position;
in vec3 xgradient;
in vec3 colorChannel;

out vec3 fragGradient;
out vec3 fragColorChannel;
out vec4 fragBox;           // Clip-box coords.

void main() 
{
    gl_Position=projection*modelview*vec4(position,1.0);
    fragGradient=xgradient;
    fragColorChannel=colorChannel;

    // Compute clip-box coords.

    fragBox = worldToBox*vec4(position, 1.0);
}
