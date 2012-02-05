#version 130

// -----------------------------------------------------------------------------
//
// image-plane.fs
//
// Fragment Shader for Naiad Studio's image plane
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

uniform sampler2D image;    

in vec2 fragTex;

out vec4 fragColor;


void main(void)
{
    fragColor = texture(image, fragTex);
    //fragColor = vec4(fragTex, 0.0, 1.0);
}
