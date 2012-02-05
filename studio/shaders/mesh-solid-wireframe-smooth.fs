#version 130

// -----------------------------------------------------------------------------
//
// mesh-solid-wireframe-smooth.fs
//
// Fragment shader for rendering wireframe on top of smooth shaded solid 
// in a single pass.
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

uniform vec4  frontMatAmbient;
uniform vec4  frontMatDiffuse;
uniform vec4  frontMatSpecular;
uniform float frontMatShininess;
uniform vec4  backMatAmbient;
uniform vec4  backMatDiffuse;
uniform vec4  backMatSpecular;
uniform float backMatShininess;
uniform vec4  lightAmbient;
uniform vec4  lightDiffuse;
uniform vec4  lightSpecular;
uniform float lineWidth;
uniform float lineExp;
uniform vec4  lineColor;

in vec3  fragNormal;
in vec3  fragLightDir;
in vec3  fragEyeDir;
in float fragDist0;
in float fragDist1;
in float fragDist2;
in float fragDistMult;

out vec4 fragColor;

void main()
{
    vec4 triColor = vec4(0.0);

    // Interpolated directions need to be normalized

    vec3 N = normalize(fragNormal);
    vec3 L = normalize(fragLightDir);
    vec3 E = normalize(fragEyeDir);

    if (fragNormal.z > 0.0) {
        // Front-facing.
	
	triColor += frontMatAmbient*lightAmbient;	
	triColor += max(dot(N, L), 0.0)*(frontMatDiffuse*lightDiffuse);

	float specular = pow(max(dot(reflect(-L, N), E), 0.0), frontMatShininess);
	triColor += specular*(frontMatSpecular*lightSpecular);
    }
    else {
	// Back-facing. Flip normal in calculations.

	triColor += backMatAmbient*lightAmbient;	
	triColor += max(dot(-N, L), 0.0)*(backMatDiffuse*lightDiffuse);

	float specular = pow(max(dot(reflect(-L, -N), E), 0.0), backMatShininess);
	triColor += specular*(backMatSpecular*lightSpecular);
    }

    // Find smallest distance to triangle edge.
    // Post-multiply distances to avoid perspective correction 
    // from interpolation.

    float minDist =
        min(fragDist0*fragDistMult, 
	        min(fragDist1*fragDistMult, fragDist2*fragDistMult));
	
    // Compare fragment closest distance to edge with
    // desired line width.

    float alpha = minDist - 0.5*lineWidth + 1.0;
	
    if( alpha <= 0.0 ) {	
        // Within line width, assign line color.

	fragColor = lineColor;
    }
    else if( 0.0 < alpha && alpha < 2.0 ) {	
        // On line boundary, fade between line color and triangle color

        float t = exp2(-(lineExp*alpha*alpha));
	fragColor = mix(triColor, lineColor, t);
    }
    else {	
	// Outside line.

	fragColor = triColor;
    }
}

