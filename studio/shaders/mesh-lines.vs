#version 130

// -----------------------------------------------------------------------------
//
// mesh-lines.vs
//
// Vertex shader for rendering lines with per vertex colors.
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

in vec3 position;
in vec4 color;

out vec4 fragColor;

void main()
{
    fragColor   = color;
    gl_Position = (projection*modelview)*vec4(position, 1.0);
}

