#version 130

// -----------------------------------------------------------------------------
//
// channel-color.fs
//
// Fragment Shader for Naiad Studio's "Channel Color"
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

uniform int   scalarChannel;
uniform int   gradLighting;
uniform int   normGrad;
uniform float ambient;
uniform vec3  lightDir;
uniform float minMag;
uniform float maxMag;
uniform int   normalizedRange;

in vec3 fragColorChannel;
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

    float mag;
    if(scalarChannel==1)
	mag=fragColorChannel.x;
    else
	mag=length(fragColorChannel);

    if(mag<minMag || mag>maxMag)
       discard;
   
    vec3 grad=fragGradient;
    float scale=1;

    if(gradLighting==1) {
        if(normGrad==1)
	    grad=normalize(grad);    	
	scale=ambient+dot(lightDir,grad)*(1.f-ambient);
    }

    if(normalizedRange==1)
	scale *= (mag-minMag)/(maxMag-minMag);
	
    color=vec4(fragColorChannel,1.f);
    color*=scale;
}
