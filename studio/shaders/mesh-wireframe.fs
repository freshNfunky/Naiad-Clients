#version 130

// -----------------------------------------------------------------------------
//
// mesh-solid-wireframe-flat.fs
//
// Fragment shader for rendering a triangle mesh as wireframe.
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

uniform float lineWidth;
uniform float lineExp;
uniform vec4  lineColor;

in float fragDist0;
in float fragDist1;
in float fragDist2;
in float fragDistMult;

out vec4 fragColor;

void main()
{
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
        float fragAlpha = mix(0.0, lineColor.a, t);
        fragColor = vec4(vec3(lineColor), fragAlpha);
    }
    else {	
        // Outside line.

    	//fragColor = triColor;
        discard;
    }
}

